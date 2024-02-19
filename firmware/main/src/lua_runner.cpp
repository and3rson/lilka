#include <lua.hpp>
#include "clib/u8g2.h"
#include <lilka.h>

Arduino_GFX* getDrawable(lua_State* L) {
    // Check if display is buffered
    lua_getfield(L, LUA_REGISTRYINDEX, "isBuffered");
    bool isBuffered = lua_toboolean(L, -1);
    lua_pop(L, 1);

    if (isBuffered) {
        // Return canvas pointer from registry
        lua_getfield(L, LUA_REGISTRYINDEX, "canvas");
        lilka::Canvas* canvas = (lilka::Canvas*)lua_touserdata(L, -1);
        lua_pop(L, 1);
        return canvas;
    } else {
        return &lilka::display;
    }
}

int lualilka_display_setBuffered(lua_State* L) {
    bool buffered = lua_toboolean(L, 1);

    // Store isBuffered in registry

    lua_pushboolean(L, buffered);
    lua_setfield(L, LUA_REGISTRYINDEX, "isBuffered");

    return 0;
}

int lualilka_display_setCursor(lua_State* L) {
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    getDrawable(L)->setCursor(x, y);
    return 0;
}

int lualilka_display_print(lua_State* L) {
    int n = lua_gettop(L);
    for (int i = 1; i <= n; i++) {
        if (lua_isstring(L, i)) {
            getDrawable(L)->print(lua_tostring(L, i));
        } else if (lua_isnumber(L, i)) {
            getDrawable(L)->print(lua_tonumber(L, i));
        } else {
            getDrawable(L)->print(lua_typename(L, lua_type(L, i)));
        }
    }
    return 0;
}

int lualilka_display_drawLine(lua_State* L) {
    int x0 = luaL_checkinteger(L, 1);
    int y0 = luaL_checkinteger(L, 2);
    int x1 = luaL_checkinteger(L, 3);
    int y1 = luaL_checkinteger(L, 4);
    uint16_t color = luaL_checkinteger(L, 5);
    getDrawable(L)->drawLine(x0, y0, x1, y1, color);
    return 0;
}

int lualilka_display_fillRect(lua_State* L) {
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int w = luaL_checkinteger(L, 3);
    int h = luaL_checkinteger(L, 4);
    uint16_t color = luaL_checkinteger(L, 5);
    getDrawable(L)->fillRect(x, y, w, h, color);
    return 0;
}

int lualilka_display_render(lua_State* L) {
    // Check if display is buffered
    lua_getfield(L, LUA_REGISTRYINDEX, "isBuffered");
    bool isBuffered = lua_toboolean(L, -1);
    lua_pop(L, 1);
    if (!isBuffered) {
        // Throw error
        return luaL_error(L, "Display is not buffered, no need to call render");
    }
    lua_getfield(L, LUA_REGISTRYINDEX, "canvas");
    lilka::Canvas* canvas = (lilka::Canvas*)lua_touserdata(L, -1);
    lua_pop(L, 1);
    lilka::display.renderCanvas(*canvas);
    return 0;
}

// int lualilka_display_drawBitmap(lua_State* L) {
//     int x = luaL_checkinteger(L, 1);
//     int y = luaL_checkinteger(L, 2);
//     int w = luaL_checkinteger(L, 3);
//     int h = luaL_checkinteger(L, 4);
//     const uint8_t* bitmap = (const uint8_t*)luaL_checkstring(L, 5);
//     getDrawable(L)->drawBitmap(x, y, w, h, bitmap);
//     return 0;
// }

static const luaL_Reg lualilka_display[] = {
    {"set_buffered", lualilka_display_setBuffered}, {"set_cursor", lualilka_display_setCursor}, {"print", lualilka_display_print}, {"draw_line", lualilka_display_drawLine}, {"fill_rect", lualilka_display_fillRect}, {"render", lualilka_display_render}, {NULL, NULL},
};

int luaopen_lilka_display(lua_State* L) {
    // Set isBuffered to true by default in registry
    lua_pushboolean(L, true);
    lua_setfield(L, LUA_REGISTRYINDEX, "isBuffered");

    luaL_newlib(L, lualilka_display);
    // Add display width & height as library properties
    lua_pushinteger(L, LILKA_DISPLAY_WIDTH);
    lua_setfield(L, -2, "width");
    lua_pushinteger(L, LILKA_DISPLAY_HEIGHT);
    lua_setfield(L, -2, "height");
    return 1;
}

int lualilka_console_print(lua_State* L) {
    int n = lua_gettop(L);
    for (int i = 1; i <= n; i++) {
        if (lua_isstring(L, i)) {
            Serial.printf("%s", lua_tostring(L, i));
        } else if (lua_isnumber(L, i)) {
            Serial.printf("%g", lua_tonumber(L, i));
        } else {
            Serial.printf("%s", lua_typename(L, lua_type(L, i)));
        }
        if (i < n) {
            Serial.printf("\t");
        }
    }
    return 0;
}

int lualilka_console_println(lua_State* L) {
    lualilka_console_print(L);
    Serial.printf("\n");
    return 0;
}

static const luaL_Reg lualilka_console[] = {
    {"print", lualilka_console_print},
    {"println", lualilka_console_println},
    {NULL, NULL},
};

int luaopen_lilka_console(lua_State* L) {
    luaL_newlib(L, lualilka_console);
    return 1;
}

int lualilka_controller_getState(lua_State* L) {
    lilka::State state = lilka::controller.getState();
    lua_createtable(L, 0, 10);
    // Push up, down, left, right, a, b, c, d, select, start to the table
    const char* keys[] = {"up", "down", "left", "right", "a", "b", "c", "d", "select", "start"};
    for (int i = 0; i < 10; i++) {
        lua_pushstring(L, keys[i]);
        // Push value as table with keys {pressed, just_pressed, just_released}
        lua_createtable(L, 0, 2);
        lua_pushstring(L, "pressed");
        lua_pushboolean(L, state.buttons[i].pressed);
        lua_settable(L, -3);
        lua_pushstring(L, "just_pressed");
        lua_pushboolean(L, state.buttons[i].justPressed);
        lua_settable(L, -3);
        lua_pushstring(L, "just_released");
        lua_pushboolean(L, state.buttons[i].justReleased);
        lua_settable(L, -3);
        lua_settable(L, -3);
    }

    // Return table
    return 1;
}

static const luaL_Reg lualilka_controller[] = {
    {"get_state", lualilka_controller_getState},
    {NULL, NULL},
};

int luaopen_lilka_controller(lua_State* L) {
    luaL_newlib(L, lualilka_controller);
    return 1;
}

int lualilka_util_random(lua_State* L) {
    // If no args - return random value from 0 to max int
    // If 1 arg - return random value from 0 to arg
    // If 2 args - return random value from arg1 to arg2

    int n = lua_gettop(L);

    if (n == 0) {
        lua_pushinteger(L, random());
        return 1;
    } else if (n == 1) {
        int max = luaL_checkinteger(L, 1);
        lua_pushinteger(L, random(max));
        return 1;
    } else if (n == 2) {
        int min = luaL_checkinteger(L, 1);
        int max = luaL_checkinteger(L, 2);
        lua_pushinteger(L, random(min, max));
        return 1;
    } else {
        return luaL_error(L, "Invalid number of arguments");
    }

    return 0;
}

int lualilka_util_delay(lua_State* L) {
    int ms = luaL_checkinteger(L, 1);
    delay(ms);
    return 0;
}

static const luaL_Reg lualilka_util[] = {
    {"random", lualilka_util_random},
    {"delay", lualilka_util_delay},
    {NULL, NULL},
};

int luaopen_lilka_util(lua_State* L) {
    luaL_newlib(L, lualilka_util);
    return 1;
}

int lua_run(String path) {
    lilka::serial_log("Init Lua libs");

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    lilka::serial_log("Init Lua libs: display");
    luaL_requiref(L, "display", luaopen_lilka_display, 1);
    lua_pop(L, 1);
    lilka::serial_log("Init Lua libs: console");
    luaL_requiref(L, "console", luaopen_lilka_console, 1);
    lua_pop(L, 1);
    lilka::serial_log("Init Lua libs: controller");
    luaL_requiref(L, "controller", luaopen_lilka_controller, 1);
    lua_pop(L, 1);
    lilka::serial_log("Init Lua libs: util");
    luaL_requiref(L, "util", luaopen_lilka_util, 1);
    lua_pop(L, 1);

    lilka::serial_log("Init canvas");
    lilka::Canvas canvas;
    lilka::display.setFont(u8g2_font_10x20_t_cyrillic);
    canvas.setFont(u8g2_font_10x20_t_cyrillic);
    canvas.begin();
    // Store canvas in registry with "canvas" key
    lilka::serial_log("Store canvas in registry");
    lua_pushlightuserdata(L, &canvas);
    lua_setfield(L, LUA_REGISTRYINDEX, "canvas");

    lilka::serial_log("Run lua script");
    int retCode = luaL_dofile(L, path.c_str());
    if (retCode) {
        const char* err = lua_tostring(L, -1);
        lilka::ui_alert("Lua", String("Помилка: ") + err);
    }

    lua_close(L);
    return retCode;
}

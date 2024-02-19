#include <lua.hpp>
#include "lauxlib.h"
#include <lilka.h>

int lualilka_print(lua_State* L) {
    int n = lua_gettop(L);
    for (int i = 1; i <= n; i++) {
        if (lua_isstring(L, i)) {
            printf("%s", lua_tostring(L, i));
        } else if (lua_isnumber(L, i)) {
            printf("%g", lua_tonumber(L, i));
        } else {
            printf("%s", lua_typename(L, lua_type(L, i)));
        }
        if (i < n) {
            printf("\t");
        }
    }
    printf("\n");
    return 0;
}

int lualilka_display_setCursor(lua_State* L) {
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    lilka::display.setCursor(x, y);
    return 0;
}

int lualilka_display_print(lua_State* L) {
    int n = lua_gettop(L);
    for (int i = 1; i <= n; i++) {
        if (lua_isstring(L, i)) {
            lilka::display.print(lua_tostring(L, i));
        } else if (lua_isnumber(L, i)) {
            lilka::display.print(lua_tonumber(L, i));
        } else {
            lilka::display.print(lua_typename(L, lua_type(L, i)));
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
    lilka::display.drawLine(x0, y0, x1, y1, color);
    return 0;
}

int lualilka_display_fillRect(lua_State* L) {
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int w = luaL_checkinteger(L, 3);
    int h = luaL_checkinteger(L, 4);
    uint16_t color = luaL_checkinteger(L, 5);
    lilka::display.fillRect(x, y, w, h, color);
    return 0;
}

static const luaL_Reg lualilka_display[] = {
    {"set_cursor", lualilka_display_setCursor}, {"print", lualilka_display_print}, {"draw_line", lualilka_display_drawLine}, {"fill_rect", lualilka_display_fillRect}, {NULL, NULL},
};

int luaopen_lilka_display(lua_State* L) {
    luaL_newlib(L, lualilka_display);
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

static const luaL_Reg lualilka_util[] = {
    {"random", lualilka_util_random},
    {NULL, NULL},
};

int luaopen_lilka_util(lua_State* L) {
    luaL_newlib(L, lualilka_util);
    return 1;
}

int lua_run(String path) {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    luaL_requiref(L, "display", luaopen_lilka_display, 1);
    lua_pop(L, 1);
    luaL_requiref(L, "console", luaopen_lilka_console, 1);
    lua_pop(L, 1);
    luaL_requiref(L, "controller", luaopen_lilka_controller, 1);
    lua_pop(L, 1);
    luaL_requiref(L, "util", luaopen_lilka_util, 1);
    lua_pop(L, 1);

    int retCode = luaL_dofile(L, path.c_str());
    if (retCode) {
        const char* err = lua_tostring(L, -1);
        lilka::ui_alert("Lua", String("Помилка: ") + err);
    }

    lua_close(L);
    return retCode;
}

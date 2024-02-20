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

int lualilka_display_color565(lua_State* L) {
    int r = luaL_checkinteger(L, 1);
    int g = luaL_checkinteger(L, 2);
    int b = luaL_checkinteger(L, 3);
    uint16_t color = lilka::display.color565(r, g, b);
    lua_pushinteger(L, color);
    return 1;
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
        } else if (lua_islightuserdata(L, i)) {
            getDrawable(L)->printf("%p", lua_topointer(L, i));
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

int lualilka_display_drawImage(lua_State* L) {
    // Args are image handler, X & Y
    // First argument is pointer to image data
    uint16_t* pixels = (uint16_t*)lua_touserdata(L, 1);
    int16_t x = luaL_checkinteger(L, 2);
    int16_t y = luaL_checkinteger(L, 3);
    // Fourth, optional argument is transparent color
    int32_t transparent = -1;
    if (lua_gettop(L) == 4) {
        transparent = luaL_checkinteger(L, 4);
    }

    int16_t width = pixels[0];
    int16_t height = pixels[1];
    pixels += 2;

    if (transparent >= 0) {
        getDrawable(L)->draw16bitRGBBitmapWithTranColor(x, y, pixels, transparent, width, height);
    } else {
        getDrawable(L)->draw16bitRGBBitmap(x, y, pixels, width, height);
    }

    return 0;
};

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
    {"set_buffered", lualilka_display_setBuffered}, {"color565", lualilka_display_color565}, {"set_cursor", lualilka_display_setCursor}, {"print", lualilka_display_print}, {"draw_line", lualilka_display_drawLine}, {"fill_rect", lualilka_display_fillRect}, {"draw_image", lualilka_display_drawImage}, {"render", lualilka_display_render}, {NULL, NULL},
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
            printf("%s", lua_tostring(L, i));
        } else if (lua_isnumber(L, i)) {
            printf("%g", lua_tonumber(L, i));
        } else if (lua_islightuserdata(L, i)) {
            printf("%p", lua_topointer(L, i));
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

static const luaL_Reg lualilka_console[] = {
    {"print", lualilka_console_print},
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

int lualilka_resources_loadImage(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    // Get dir from registry
    lua_getfield(L, LUA_REGISTRYINDEX, "dir");
    const char* dir = lua_tostring(L, -1);
    lua_pop(L, 1);
    String fullPath = String(dir) + "/" + path;
    FILE* file = fopen(fullPath.c_str(), "r");
    if (!file) {
        return luaL_error(L, "File not found - %s", fullPath.c_str());
    }
    // Parse BMP image
    uint8_t fileheader[14];
    fread(fileheader, 1, 14, file);
    int dataOffset = fileheader[10] + (fileheader[11] << 8) + (fileheader[12] << 16) + (fileheader[13] << 24);
    uint8_t fileinfo[40];
    fread(fileinfo, 1, 40, file);
    int width = fileinfo[4] + (fileinfo[5] << 8) + (fileinfo[6] << 16) + (fileinfo[7] << 24);
    int height = fileinfo[8] + (fileinfo[9] << 8) + (fileinfo[10] << 16) + (fileinfo[11] << 24);
    // int bitsPerPixel = fileinfo[14] + (fileinfo[15] << 8);
    // uint16_t *pixels = new uint16_t[width * height];

    // First two elements are width and height
    uint16_t* pixels = new uint16_t[2 + width * height];
    pixels[0] = width;
    pixels[1] = height;
    lilka::serial_log("Lua: loading image %s, width: %d, height: %d", path, width, height);
    fseek(file, dataOffset, SEEK_SET);
    for (int y = height - 1; y >= 0; y--) {
        for (int x = 0; x < width; x++) {
            uint32_t color;
            fread(&color, 1, 3, file);
            pixels[2 + x + y * width] = lilka::display.color565((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
        }
    }
    // Append pixels to images table in registry
    lua_getfield(L, LUA_REGISTRYINDEX, "images");
    lua_pushlightuserdata(L, pixels);
    lua_setfield(L, -2, path);
    lua_pop(L, 1);

    fclose(file);

    // Return pointer to pixels
    lua_pushlightuserdata(L, pixels);

    return 1;
}

static const luaL_Reg lualilka_resources[] = {
    {"load_image", lualilka_resources_loadImage},
    {NULL, NULL},
};

int luaopen_lilka_resources(lua_State* L) {
    luaL_newlib(L, lualilka_resources);
    return 1;
}

int lua_run(String path) {
    lilka::serial_log("Lua: init libs");

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    lilka::serial_log("Lua: init display");
    luaL_requiref(L, "display", luaopen_lilka_display, 1);
    lua_pop(L, 1);
    lilka::serial_log("Lua: init console");
    luaL_requiref(L, "console", luaopen_lilka_console, 1);
    lua_pop(L, 1);
    lilka::serial_log("Lua: init controller");
    luaL_requiref(L, "controller", luaopen_lilka_controller, 1);
    lua_pop(L, 1);
    lilka::serial_log("Lua: init util");
    luaL_requiref(L, "util", luaopen_lilka_util, 1);
    lua_pop(L, 1);
    lilka::serial_log("Lua: init resources");
    luaL_requiref(L, "resources", luaopen_lilka_resources, 1);
    lua_pop(L, 1);

    // Get dir name from path (without the trailing slash)
    String dir = path.substring(0, path.lastIndexOf('/'));
    lilka::serial_log("Lua: script dir: %s", dir.c_str());
    // Store dir in registry with "dir" key
    lua_pushstring(L, dir.c_str());
    lua_setfield(L, LUA_REGISTRYINDEX, "dir");

    lilka::serial_log("Lua: init canvas");
    lilka::Canvas canvas;
    lilka::display.setFont(u8g2_font_10x20_t_cyrillic);
    canvas.setFont(u8g2_font_10x20_t_cyrillic);
    canvas.begin();
    // Store canvas in registry with "canvas" key
    lilka::serial_log("Lua: store canvas in registry");
    lua_pushlightuserdata(L, &canvas);
    lua_setfield(L, LUA_REGISTRYINDEX, "canvas");
    // Initialize table for image pointers
    lilka::serial_log("Lua: init memory for images");
    lua_newtable(L);
    lua_setfield(L, LUA_REGISTRYINDEX, "images");

    lilka::serial_log("Lua: run script");
    int retCode = luaL_dofile(L, path.c_str());
    if (retCode) {
        const char* err = lua_tostring(L, -1);
        lilka::ui_alert("Lua", String("Помилка: ") + err);
    }

    lilka::serial_log("Lua: cleanup");

    // Free images from registry
    lua_getfield(L, LUA_REGISTRYINDEX, "images");
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        // TODO: Is this correct?
        uint16_t* pixels = (uint16_t*)lua_touserdata(L, -1);
        delete[] pixels;
        lua_pop(L, 1);
    }

    lua_close(L);
    return retCode;
}

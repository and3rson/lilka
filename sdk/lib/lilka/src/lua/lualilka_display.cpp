#include "lualilka_display.h"

namespace lilka {

Arduino_GFX* getDrawable(lua_State* L) {
    // Check if display is buffered
    lua_getfield(L, LUA_REGISTRYINDEX, "isBuffered");
    bool isBuffered = lua_toboolean(L, -1);
    lua_pop(L, 1);

    if (isBuffered) {
        // Return canvas pointer from registry
        lua_getfield(L, LUA_REGISTRYINDEX, "canvas");
        Canvas* canvas = (Canvas*)lua_touserdata(L, -1);
        lua_pop(L, 1);
        return canvas;
    } else {
        return &display;
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
    uint16_t color = display.color565(r, g, b);
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

int lualilka_display_drawBitmap(lua_State* L) {
    // Args are bitmap handler, X & Y
    // First argument is pointer to bitmap data
    Bitmap* bitmap = (Bitmap*)lua_touserdata(L, 1);
    int16_t x = luaL_checkinteger(L, 2);
    int16_t y = luaL_checkinteger(L, 3);

    if (bitmap->transparentColor >= 0) {
        getDrawable(L)->draw16bitRGBBitmapWithTranColor(x, y, bitmap->pixels, bitmap->transparentColor, bitmap->width, bitmap->height);
    } else {
        getDrawable(L)->draw16bitRGBBitmap(x, y, bitmap->pixels, bitmap->width, bitmap->height);
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
    Canvas* canvas = (Canvas*)lua_touserdata(L, -1);
    lua_pop(L, 1);
    display.renderCanvas(*canvas);
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
    {"set_buffered", lualilka_display_setBuffered}, {"color565", lualilka_display_color565}, {"set_cursor", lualilka_display_setCursor}, {"print", lualilka_display_print}, {"draw_line", lualilka_display_drawLine}, {"fill_rect", lualilka_display_fillRect}, {"draw_bitmap", lualilka_display_drawBitmap}, {"render", lualilka_display_render}, {NULL, NULL},
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

}

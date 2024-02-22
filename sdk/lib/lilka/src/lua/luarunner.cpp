#include <lua.hpp>
#include "clib/u8g2.h"
#include <lilka.h>

#include "lualilka_display.h"
#include "lualilka_console.h"
#include "lualilka_controller.h"
#include "lualilka_util.h"
#include "lualilka_resources.h"

namespace lilka {

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
    // Initialize table for bitmap pointers
    lilka::serial_log("Lua: init memory for bitmaps");
    lua_newtable(L);
    lua_setfield(L, LUA_REGISTRYINDEX, "bitmaps");

    lilka::serial_log("Lua: run script");
    int retCode = luaL_dofile(L, path.c_str());
    if (retCode) {
        const char* err = lua_tostring(L, -1);
        lilka::ui_alert("Lua", String("Помилка: ") + err);
    }

    lilka::serial_log("Lua: cleanup");

    // Free bitmaps from registry
    lua_getfield(L, LUA_REGISTRYINDEX, "bitmaps");
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        // TODO: Is this correct?
        lilka::Bitmap* bitmap = (lilka::Bitmap*)lua_touserdata(L, -1);
        delete bitmap;
        lua_pop(L, 1);
    }

    lua_close(L);
    return retCode;
}

} // namespace lilka

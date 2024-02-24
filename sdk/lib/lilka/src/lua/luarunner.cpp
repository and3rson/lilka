#include <csetjmp>

#include <lua.hpp>
#include "clib/u8g2.h"
#include <lilka.h>

#include "lualilka_display.h"
#include "lualilka_console.h"
#include "lualilka_controller.h"
#include "lualilka_math.h"
#include "lualilka_util.h"
#include "lualilka_resources.h"

namespace lilka {

jmp_buf stopjmp;

int lua_run(String path) {
#ifndef LILKA_NO_LUA
    lilka::serial_log("lua: init libs");

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    // lilka::serial_log("Lua: init display");
    // luaL_requiref(L, "display", luaopen_lilka_display, 1);
    // lua_pop(L, 1);
    // lilka::serial_log("Lua: init console");
    // luaL_requiref(L, "console", luaopen_lilka_console, 1);
    // lua_pop(L, 1);
    // lilka::serial_log("Lua: init controller");
    // luaL_requiref(L, "controller", luaopen_lilka_controller, 1);
    // lua_pop(L, 1);
    // lilka::serial_log("Lua: init math");
    // luaL_requiref(L, "math", luaopen_lilka_math, 1);
    // lua_pop(L, 1);
    // lilka::serial_log("Lua: init util");
    // luaL_requiref(L, "util", luaopen_lilka_util, 1);
    // lua_pop(L, 1);
    // lilka::serial_log("Lua: init resources");
    // luaL_requiref(L, "resources", luaopen_lilka_resources, 1);
    // lua_pop(L, 1);

    lilka::serial_log("lua: init display");
    lualilka_display_register(L);
    lilka::serial_log("lua: init console");
    lualilka_console_register(L);
    lilka::serial_log("lua: init controller");
    lualilka_controller_register(L);
    lilka::serial_log("lua: init math");
    lualilka_math_register(L);
    lilka::serial_log("lua: init util");
    lualilka_util_register(L);
    lilka::serial_log("lua: init resources");
    lualilka_resources_register(L);

    // Get dir name from path (without the trailing slash)
    String dir = path.substring(0, path.lastIndexOf('/'));
    lilka::serial_log("lua: script dir: %s", dir.c_str());
    // Store dir in registry with "dir" key
    lua_pushstring(L, dir.c_str());
    lua_setfield(L, LUA_REGISTRYINDEX, "dir");

    lilka::serial_log("lua: init canvas");
    lilka::Canvas canvas;
    lilka::display.setFont(u8g2_font_10x20_t_cyrillic);
    canvas.setFont(u8g2_font_10x20_t_cyrillic);
    canvas.begin();
    // Store canvas in registry with "canvas" key
    lilka::serial_log("lua: store canvas in registry");
    lua_pushlightuserdata(L, &canvas);
    lua_setfield(L, LUA_REGISTRYINDEX, "canvas");
    // Initialize table for bitmap pointers
    lilka::serial_log("lua: init memory for bitmaps");
    lua_newtable(L);
    lua_setfield(L, LUA_REGISTRYINDEX, "bitmaps");

    lilka::serial_log("lua: run script");

    int jmpCode = setjmp(stopjmp);

    if (jmpCode == 0) {
        // Run script
        int retCode = luaL_dofile(L, path.c_str());
        if (retCode) {
            longjmp(stopjmp, retCode);
        }

        // Check if _update function exists and call it
        const uint32_t perfectDelta = 1000 / 30;
        uint32_t delta = perfectDelta; // Delta for first frame is always 1/30
        while (true) {
            uint32_t now = millis();
            lua_getglobal(L, "_update");
            if (lua_isfunction(L, -1)) {
                // Call _update function, passing delta as argument
                lua_pushnumber(L, delta);
                retCode = lua_pcall(L, 1, 0, 0);
                if (retCode) {
                    // const char* err = lua_tostring(L, -1);
                    // lilka::ui_alert("Lua", String("Помилка в _update: ") + err);
                    longjmp(stopjmp, retCode);
                }
            } else {
                // No _update function - we're done
                longjmp(stopjmp, 32);
            }

            // Calculate time spent in _update
            uint32_t elapsed = millis() - now;
            // If we're too fast, delay to keep 30 FPS
            if (elapsed < perfectDelta) {
                delay(perfectDelta - elapsed);
                delta = perfectDelta;
            } else {
                // If we're too slow, don't delay and set delta to elapsed time
                delta = elapsed;
            }
        }
    }

    int retCode = jmpCode;
    if (retCode == 32) {
        // Normal exit through longjmp
        retCode = 0;
    }

    if (retCode) {
        const char* err = lua_tostring(L, -1);
        lilka::ui_alert("Lua", String("Помилка: ") + err);
    }

    lilka::serial_log("lua: cleanup");

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
#else
    ui_alert("Помилка", "Lua не підтримується");
#endif
}

} // namespace lilka

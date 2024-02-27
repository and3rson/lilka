#include <csetjmp>

#include <lua.hpp>
#include "clib/u8g2.h"
#include <lilka.h>

#include "lualilka_display.h"
#include "lualilka_console.h"
#include "lualilka_controller.h"
#include "lualilka_resources.h"
#include "lualilka_math.h"
#include "lualilka_geometry.h"
#include "lualilka_gpio.h"
#include "lualilka_util.h"
#include "lualilka_state.h"

namespace lilka {

jmp_buf stopjmp;

int execute(lua_State* L) {
    // Calls Lua code that's on top of the stack (previously loaded with luaL_loadfile or luaL_loadstring)

    int jmpCode = setjmp(stopjmp);

    if (jmpCode == 0) {
        // Run script
        int retCode = lua_pcall(L, 0, LUA_MULTRET, 0);
        if (retCode) {
            longjmp(stopjmp, retCode);
        }

        // Check if lilka._update function exists and call it
        const uint32_t perfectDelta = 1000 / 30;
        uint32_t delta = perfectDelta; // Delta for first frame is always 1/30
        while (true) {
            uint32_t now = millis();
            // Check for lilka._update function
            lua_getglobal(L, "lilka");
            // If no lilka table, we're done
            if (!lua_istable(L, -1)) {
                // TODO: pop?
                serial_log("lua: no lilka table");
                longjmp(stopjmp, 32);
            }
            lua_getfield(L, -1, "_update");
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
                // serial_log("lua: no _update function in lilka table");
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
        return 0;
    }
    return retCode;
}

lua_State* lua_setup(const char* dir) {
    lilka::serial_log("lua: script dir: %s", dir);

    lua_State* L = luaL_newstate();

    lilka::serial_log("lua: init libs");
    luaL_openlibs(L);

    // Store dir in registry with "dir" key
    lua_pushstring(L, dir);
    lua_setfield(L, LUA_REGISTRYINDEX, "dir");

    lilka::serial_log("lua: set path");
    // Set package.path to point to the same directory as the script
    lua_getglobal(L, "package");
    lua_pushstring(L, (String(dir) + "/?.lua").c_str());
    lua_setfield(L, -2, "path");
    lua_pop(L, 1);

    // Set path to C modules to the same directory as the script (I wonder if anyone will ever use this)
    lua_getglobal(L, "package");
    lua_pushstring(L, (String(dir) + "/?.so").c_str());
    lua_setfield(L, -2, "cpath");
    lua_pop(L, 1);

    lilka::serial_log("lua: init display");
    lualilka_display_register(L);
    lilka::serial_log("lua: init console");
    lualilka_console_register(L);
    lilka::serial_log("lua: init controller");
    lualilka_controller_register(L);
    lilka::serial_log("lua: init resources");
    lualilka_resources_register(L);
    lilka::serial_log("lua: init math");
    lualilka_math_register(L);
    lilka::serial_log("lua: init geometry");
    lualilka_geometry_register(L);
    lilka::serial_log("lua: init gpio");
    lualilka_gpio_register(L);
    lilka::serial_log("lua: init util");
    lualilka_util_register(L);

    lilka::serial_log("lua: init canvas");
    lilka::Canvas* canvas = new lilka::Canvas();
    lilka::display.setFont(u8g2_font_10x20_t_cyrillic);
    canvas->setFont(u8g2_font_10x20_t_cyrillic);
    canvas->begin();
    // Store canvas in registry with "canvas" key
    lilka::serial_log("lua: store canvas in registry");
    lua_pushlightuserdata(L, canvas);
    lua_setfield(L, LUA_REGISTRYINDEX, "canvas");
    // Initialize table for bitmap pointers
    lilka::serial_log("lua: init memory for bitmaps");
    lua_newtable(L);
    lua_setfield(L, LUA_REGISTRYINDEX, "bitmaps");

    // Set global "lilka" table for user stuff
    lua_newtable(L);
    lua_setglobal(L, "lilka");

    return L;
}

void lua_teardown(lua_State* L) {
    lilka::serial_log("lua: cleanup");

    // Free bitmaps from registry
    lua_getfield(L, LUA_REGISTRYINDEX, "bitmaps");
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lilka::Bitmap* bitmap = (lilka::Bitmap*)lua_touserdata(L, -1);
        delete bitmap;
        lua_pop(L, 1);
    }

    // Free canvas from registry
    lilka::Canvas* canvas = (lilka::Canvas*)lua_touserdata(L, lua_getfield(L, LUA_REGISTRYINDEX, "canvas"));
    delete canvas;

    lua_close(L);
}

int lua_runfile(String path) {
#ifndef LILKA_NO_LUA
    // Get dir name from path (without the trailing slash)
    String dir = path.substring(0, path.lastIndexOf('/'));

    lua_State* L = lua_setup(dir.c_str());

    // Load state from file (file name is "path" with .lua replaced with .state)
    String statePath = path.substring(0, path.lastIndexOf('.')) + ".state";
    // Check if state file exists
    if (access(statePath.c_str(), F_OK) != -1) {
        lilka::serial_log("lua: found state file %s", statePath.c_str());
        // Load state from file
        lualilka_state_load(L, statePath.c_str());
    }

    lilka::serial_log("lua: run file");

    int retCode = luaL_loadfile(L, path.c_str()) || execute(L);

    if (retCode) {
        const char* err = lua_tostring(L, -1);
        lilka::ui_alert("Lua", String("Помилка: ") + err);
    }

    // Check if state table exists and save it to file if so
    lua_getglobal(L, "state");
    bool hasState = lua_istable(L, -1);
    lua_pop(L, 1);
    if (hasState) {
        lilka::serial_log("lua: saving state to file %s", statePath.c_str());
        lualilka_state_save(L, statePath.c_str());
    } else {
        lilka::serial_log("lua: no state to save");
    }

    lua_teardown(L);

    return retCode;
#else
    ui_alert("Помилка", "Lua не підтримується");
    return -1;
#endif
}

int lua_runsource(String source) {
#ifndef LILKA_NO_LUA
    lua_State* L = lua_setup("/sd"); // TODO: hard-coded

    lilka::serial_log("lua: run source");

    int retCode = luaL_loadstring(L, source.c_str()) || execute(L);

    if (retCode) {
        const char* err = lua_tostring(L, -1);
        lilka::ui_alert("Lua", String("Помилка: ") + err);
    }

    lua_teardown(L);

    return retCode;
#else
    ui_alert("Помилка", "Lua не підтримується");
    return -1;
#endif
}

lua_State* Lrepl;

int lua_repl_start() {
#ifndef LILKA_NO_LUA
    Lrepl = lua_setup("/sd"); // TODO: hard-coded

    lilka::serial_log("lua: start REPL");
    return 0;
#else
    ui_alert("Помилка", "Lua не підтримується");
    return -1;
#endif
}

int lua_repl_input(String input) {
#ifndef LILKA_NO_LUA
    // lilka::serial_log("lua: input: %s", input.c_str());

    int retCode = luaL_loadstring(Lrepl, input.c_str()) || execute(Lrepl);

    if (retCode) {
        const char* err = lua_tostring(Lrepl, -1);
        serial_log("lua: error: %s", err);
    }

    return retCode;
#else
    ui_alert("Помилка", "Lua не підтримується");
    return -1;
#endif
}

int lua_repl_stop() {
#ifndef LILKA_NO_LUA
    lilka::serial_log("lua: stop REPL");
    lua_teardown(Lrepl);
    return 0;
#else
    ui_alert("Помилка", "Lua не підтримується");
    return -1;
#endif
}

} // namespace lilka

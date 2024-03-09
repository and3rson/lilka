#include "lualilka_controller.h"

int lualilka_controller_getState(lua_State* L) {
    lilka::State state = lilka::controller.getState();
    lilka::_StateButtons& buttons = *reinterpret_cast<lilka::_StateButtons*>(&state);
    lua_createtable(L, 0, 10);
    // Push up, down, left, right, a, b, c, d, select, start to the table
    const char* keys[] = {"up", "down", "left", "right", "a", "b", "c", "d", "select", "start"};
    for (int i = 0; i < 10; i++) {
        lua_pushstring(L, keys[i]);
        // Push value as table with keys {pressed, just_pressed, just_released}
        lua_createtable(L, 0, 2);
        lua_pushstring(L, "pressed");
        lua_pushboolean(L, buttons[i].pressed);
        lua_settable(L, -3);
        lua_pushstring(L, "just_pressed");
        lua_pushboolean(L, buttons[i].justPressed);
        lua_settable(L, -3);
        lua_pushstring(L, "just_released");
        lua_pushboolean(L, buttons[i].justReleased);
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

int lualilka_controller_register(lua_State* L) {
    // Create global "controller" table that contains all controller functions
    luaL_newlib(L, lualilka_controller);
    lua_setglobal(L, "controller");
    return 0;
}

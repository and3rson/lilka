#include "lualilka_util.h"

namespace lilka {

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

} // namespace lilka

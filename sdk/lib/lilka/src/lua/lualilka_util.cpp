#include "lualilka_util.h"

namespace lilka {

int lualilka_util_delay(lua_State* L) {
    int ms = luaL_checkinteger(L, 1);
    delay(ms);
    return 0;
}

static const luaL_Reg lualilka_util[] = {
    {"delay", lualilka_util_delay},
    {NULL, NULL},
};

// int luaopen_lilka_util(lua_State* L) {
//     luaL_newlib(L, lualilka_util);
//     return 1;
// }

int lualilka_util_register(lua_State* L) {
    // Create global "util" table that contains all util functions
    luaL_newlib(L, lualilka_util);
    lua_setglobal(L, "util");
    return 0;
}

} // namespace lilka

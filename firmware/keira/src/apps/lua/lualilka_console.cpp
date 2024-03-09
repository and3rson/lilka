#include "lualilka_console.h"

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

int lualilka_console_register(lua_State* L) {
    // Create global "console" table that contains all console functions
    luaL_newlib(L, lualilka_console);
    lua_setglobal(L, "console");
    return 0;
}

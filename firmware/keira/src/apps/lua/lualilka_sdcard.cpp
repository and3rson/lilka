#include <csetjmp>

#include "lualilka_sdcard.h"
#include "lilka.h"

int lualilka_sdcard_listDir(lua_State* L) {
    int n = lua_gettop(L);

    if (n != 1) {
        return luaL_error(L, "Очікується 1 аргумент, отримано %d", n);
    }

    if (!lilka::sdcard.available()) {
        return luaL_error(L, "SD card not found");
    }


    lilka::Entry entries[32];

    String path = lua_tostring(L, 1);

    int numEntries = lilka::sdcard.listDir(path, entries);

    if (numEntries == -1) {
        return luaL_error(L, "Error read dir");
    }

    lua_createtable(L, numEntries, 0);

    for (int i = 0; i < numEntries; i++) {
        lua_pushstring(L, entries[i].name.c_str());
        lua_rawseti (L, -2, i+1); /* In lua indices start at 1 */
    }

    return 1;
}

int lualilka_sdcard_readRAW(lua_State* L){
    int n = lua_gettop(L);

    if (n != 1) {
        return luaL_error(L, "Очікується 1 аргумент, отримано %d", n);
    }

    if (!lilka::sdcard.available()) {
        return luaL_error(L, "SD card not found");
    }

}

static const luaL_Reg lualilka_sdcard[] = {
    {"time", lualilka_sdcard_listDir},
    {NULL, NULL},
};

int lualilka_sdcard_register(lua_State* L) {
    // Create global "util" table that contains all util functions
    luaL_newlib(L, lualilka_sdcard);
    lua_setglobal(L, "sdcard");
    return 0;
}

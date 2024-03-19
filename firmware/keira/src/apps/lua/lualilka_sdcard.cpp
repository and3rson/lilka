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

int lualilka_sdcard_readAll(lua_State* L){
    int n = lua_gettop(L);

    if (n != 2) {
        return luaL_error(L, "Очікується 2 аргумент, отримано %d", n);
    }

    if (!lilka::sdcard.available()) {
        return luaL_error(L, "SD card not found");
    }

    String path = lua_tostring(L, 1);
    int line_index = lua_tointeger(L, 2);

    path = "/sd" + path;

    char line[256];
    int current_line = 1;

    FILE * current_file_entrie = fopen(path.c_str(), "ab+");

    if( current_file_entrie == NULL ) { 
        fclose(current_file_entrie);                      
        return 0;
    }

    while (fgets(line, sizeof(line), current_file_entrie)) {
        if (current_line == line_index) {
            lua_pushstring(L, line);
            break;
        }
        current_line++;
    }

    return 1;
}

int lualilka_sdcard_writeAppend(lua_State* L){
    int n = lua_gettop(L);

    if (n != 2) {
        return luaL_error(L, "Очікується 2 аргумент, отримано %d", n);
    }

    if (!lilka::sdcard.available()) {
        return luaL_error(L, "SD card not found");
    }

    String path = lua_tostring(L, 1);
    String text = lua_tostring(L, 2);

    path = "/sd" + path;

    FILE * current_file_entrie = fopen(path.c_str(), "ab+");

    fprintf(current_file_entrie, text.c_str());

    fclose(current_file_entrie);

    return 0;
}

int lualilka_sdcard_removeFile(lua_State* L){
    int n = lua_gettop(L);

    if (n != 1) {
        return luaL_error(L, "Очікується 1 аргумент, отримано %d", n);
    }

    if (!lilka::sdcard.available()) {
        return luaL_error(L, "SD card not found");
    }

    String path = lua_tostring(L, 1);

    path = "/sd" + path;

    remove(path.c_str());

    return 0;
}

int lualilka_sdcard_renameFile(lua_State* L){
    int n = lua_gettop(L);

    if (n != 2) {
        return luaL_error(L, "Очікується 1 аргумент, отримано %d", n);
    }

    if (!lilka::sdcard.available()) {
        return luaL_error(L, "SD card not found");
    }

    String old_name = lua_tostring(L, 1);
    String new_name = lua_tostring(L, 2);

    old_name = "/sd" + old_name;
    new_name = "/sd" + new_name;

    if (rename(old_name.c_str(), new_name.c_str()) != 0){
        return luaL_error(L, "Error renaming file");
    }
    
    return 0;
}


static const luaL_Reg lualilka_sdcard[] = {
    {"list_dir", lualilka_sdcard_listDir},
    {"read_file", lualilka_sdcard_readAll},
    {"write_file", lualilka_sdcard_writeAppend},
    {"remove_file", lualilka_sdcard_removeFile},
    {"rename_file", lualilka_sdcard_renameFile},
    {NULL, NULL},
};

int lualilka_sdcard_register(lua_State* L) {
    // Create global "util" table that contains all util functions
    luaL_newlib(L, lualilka_sdcard);
    lua_setglobal(L, "sdcard");
    return 0;
}

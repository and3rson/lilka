#include <csetjmp>

#include "lualilka_sdcard.h"
#include "lilka.h"

static FILE* current_file_entrie;
static String current_file_path;
static String current_file_mode;

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

int lualilka_sdcard_openfile(lua_State* L){
    int n = lua_gettop(L);

    if (n != 2) {
        return luaL_error(L, "Очікується 2 аргумент, отримано %d", n);
    }

    if (!lilka::sdcard.available()) {
        return luaL_error(L, "SD card not found");
    }

    current_file_path = lua_tostring(L, 1);
    current_file_mode = lua_tostring(L, 2);
    if(current_file_entrie == NULL){
        current_file_entrie = fopen(current_file_path.c_str(), current_file_mode.c_str());
        // current_file_entrie = lilka::sdcard.fs->open(current_file_path.c_str(), current_file_mode.c_str(), true);
    }
    else{
        return luaL_error(L, "Another file is opened \"%s\"", current_file_path);
    }

    return 0;
}

int lualilka_sdcard_closeFile(lua_State* L){
    if(current_file_entrie != NULL){
        fclose(current_file_entrie);
        current_file_path = "";
        current_file_mode = "";
    }
    return 0;
}

int lualilka_sdcard_readAll(lua_State* L){
    // Serial.println("TEST----->");
    // if(current_file_entrie != NULL){
        
    //     String string_ = fread(current_file_entrie,);
    //     lilka::serial_log(string_.c_str());
    //     lua_pushstring(L, string_.c_str());

    // }
    // else{
    //     return luaL_error(L, "File not opened / Unsupported mode(R/RW)");
    // }
    return 0;
}

int lualilka_sdcard_writeAppend(lua_State* L){
    int n = lua_gettop(L);

    if (n != 1) {
        return luaL_error(L, "Очікується 1 аргумент, отримано %d", n);
    }

    String append_text = lua_tostring(L, 1);

    fprintf(current_file_entrie, append_text.c_str());
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

    lilka::sdcard.fs->remove(path);

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

    String path = lua_tostring(L, 1);
    String new_name = lua_tostring(L, 2);

    lilka::sdcard.fs->rename(path, new_name);
    
    return 0;
}


static const luaL_Reg lualilka_sdcard[] = {
    {"listDir", lualilka_sdcard_listDir},
    {"openFile", lualilka_sdcard_openfile},
    {"closeFile", lualilka_sdcard_closeFile},
    {"readFile", lualilka_sdcard_readAll},
    {"writeFile", lualilka_sdcard_writeAppend},
    {"removeFile", lualilka_sdcard_removeFile},
    {"renameFile", lualilka_sdcard_renameFile},
    {NULL, NULL},
};

int lualilka_sdcard_register(lua_State* L) {
    // Create global "util" table that contains all util functions
    luaL_newlib(L, lualilka_sdcard);
    lua_setglobal(L, "sdcard");
    return 0;
}

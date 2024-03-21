#include <csetjmp>

#include "lualilka_sdcard.h"
#include "lilka.h"

int lualilka_sdcard_list_dir(lua_State* L) {
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
        lua_rawseti(L, -2, i + 1); /* In lua indices start at 1 */
    }

    return 1;
}

int lualilka_sdcard_open_file(lua_State* L) {
    int n = lua_gettop(L);

    if (n != 2) {
        return luaL_error(L, "Очікується 2 аргумент, отримано %d", n);
    }

    if (!lilka::sdcard.available()) {
        return luaL_error(L, "SD card not found");
    }

    String path = lua_tostring(L, 1);

    path = "/sd/" + path;

    String mode = lua_tostring(L, 2);

    FILE* current_file_entrie = fopen(path.c_str(), mode.c_str());

    if (!current_file_entrie) return luaL_error(L, "Помилка при відкриванні файла");

    lua_pushlightuserdata(L, current_file_entrie);

    return 1;
}

int lualilka_sdcard_close_file(lua_State* L) {
    int n = lua_gettop(L);

    if (n != 1) {
        return luaL_error(L, "Очікується 1 аргумент, отримано %d", n);
    }

    if (!lilka::sdcard.available()) {
        return luaL_error(L, "SD card not found");
    }

    FILE* current_file_entrie = static_cast<FILE*>(lua_touserdata(L, 1));

    if (current_file_entrie) {
        fclose(current_file_entrie); // Закриття файлу
    }

    return 1;
}

int lualilka_sdcard_read_all(lua_State* L) {
    int n = lua_gettop(L);

    if (n != 2) {
        return luaL_error(L, "Очікується 1 аргумент, отримано %d", n);
    }

    if (!lilka::sdcard.available()) {
        return luaL_error(L, "SD card not found");
    }

    FILE* current_file_entrie = static_cast<FILE*>(lua_touserdata(L, 1));

    size_t maxBytes = luaL_checknumber(L, 2);
    char* buf = (char*)malloc(maxBytes);

    if (buf == NULL) {
        return luaL_error(L, "Помилка виділення пам'яті");
    }

    fseek(current_file_entrie, 0, SEEK_SET);
    size_t bytesRead = fread(buf, 1, maxBytes, current_file_entrie);

    if (bytesRead < maxBytes && ferror(current_file_entrie)) {
        free(buf);
        return luaL_error(L, "помилка читання з файлу");
    }

    lua_pushlstring(L, buf, bytesRead);

    free(buf);

    return 1;
}

int lualilka_sdcard_write_append(lua_State* L) {
    int n = lua_gettop(L);

    if (n != 2) {
        return luaL_error(L, "Очікується 2 аргумент, отримано %d", n);
    }

    if (!lilka::sdcard.available()) {
        return luaL_error(L, "SD card not found");
    }

    FILE* current_file_entrie =  static_cast<FILE*>(lua_touserdata(L, 1));
    String text = lua_tostring(L, 2);

    fprintf(current_file_entrie, text.c_str());

    return 0;
}

int lualilka_sdcard_remove_file(lua_State* L) {
    int n = lua_gettop(L);

    if (n != 1) {
        return luaL_error(L, "Очікується 1 аргумент, отримано %d", n);
    }

    if (!lilka::sdcard.available()) {
        return luaL_error(L, "SD card not found");
    }

    String path = lua_tostring(L, 1);

    path = "/sd/" + path;

    remove(path.c_str());

    return 0;
}

int lualilka_sdcard_rename_file(lua_State* L) {
    int n = lua_gettop(L);

    if (n != 2) {
        return luaL_error(L, "Очікується 1 аргумент, отримано %d", n);
    }

    if (!lilka::sdcard.available()) {
        return luaL_error(L, "SD card not found");
    }

    String old_name = lua_tostring(L, 1);
    String new_name = lua_tostring(L, 2);

    old_name = "/sd/" + old_name;
    new_name = "/sd/" + new_name;

    if (rename(old_name.c_str(), new_name.c_str()) != 0) {
        return luaL_error(L, "Error renaming file");
    }

    return 0;
}

int lualilka_sdcard_file_size(lua_State* L) {
    int n = lua_gettop(L);

    if (n != 1) {
        return luaL_error(L, "Очікується 1 аргумент, отримано %d", n);
    }

    if (!lilka::sdcard.available()) {
        return luaL_error(L, "SD card not found");
    }

    FILE* current_file_entrie = static_cast<FILE*>(lua_touserdata(L, 1));

    fseek(current_file_entrie, 0, SEEK_END); // seek to end of file
    size_t size = ftell(current_file_entrie); // get current file pointer
    fseek(current_file_entrie, 0, SEEK_SET); // seek back to beginning of file

    lua_pushinteger(L, size);

    return 1;
}

static const luaL_Reg lualilka_sdcard[] = {
    {"list_dir", lualilka_sdcard_list_dir},
    {"open_file", lualilka_sdcard_open_file},
    {"close_file", lualilka_sdcard_close_file},
    {"read_file", lualilka_sdcard_read_all},
    {"write_file", lualilka_sdcard_write_append},
    {"file_size", lualilka_sdcard_file_size},
    {"remove_file", lualilka_sdcard_remove_file},
    {"rename_file", lualilka_sdcard_rename_file},
    {NULL, NULL},
};

int lualilka_sdcard_register(lua_State* L) {
    // Create global "util" table that contains all util functions
    luaL_newlib(L, lualilka_sdcard);
    lua_setglobal(L, "sdcard");
    return 0;
}

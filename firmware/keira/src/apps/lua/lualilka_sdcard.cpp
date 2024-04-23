#include "lualilka_sdcard.h"
#include "lilka.h"

static int lualilka_create_object_file(lua_State* L) {
    String path = luaL_checkstring(L, 1);
    String mode = luaL_checkstring(L, 2);
    *reinterpret_cast<FILE**>(lua_newuserdata(L, sizeof(FILE*))) =
        fopen((lilka::fileutils.getSDRoot() + path).c_str(), mode.c_str());
    luaL_setmetatable(L, FILE_OBJECT);
    return 1;
}

static int lualilka_delete_object_file(lua_State* L) {
    FILE* filePointer = *reinterpret_cast<FILE**>(luaL_checkudata(L, 1, FILE_OBJECT));
    if (filePointer) {
        fclose(filePointer);
    }
    return 0;
}

static int lualilka_file_size(lua_State* L) {
    FILE* filePointer = *reinterpret_cast<FILE**>(luaL_checkudata(L, 1, FILE_OBJECT));
    if (filePointer) {
        size_t initial_offset = ftell(filePointer); // get current file pointer
        fseek(filePointer, 0, SEEK_END); // seek to end of file
        size_t size = ftell(filePointer); // get current file pointer
        fseek(filePointer, initial_offset, SEEK_SET); // seek back to beginning of file

        lua_pushinteger(L, size);
        return 1;
    }
    return luaL_error(L, "Size read error");
}

static int lualilka_file_seek(lua_State* L) {
    FILE* filePointer = *reinterpret_cast<FILE**>(luaL_checkudata(L, 1, FILE_OBJECT));
    if (filePointer) {
        size_t seek = luaL_checknumber(L, 2);
        fseek(filePointer, seek, SEEK_SET);
        return 0;
    }
    return luaL_error(L, "seek error");
}

static int lualilka_file_read(lua_State* L) {
    FILE* filePointer = *reinterpret_cast<FILE**>(luaL_checkudata(L, 1, FILE_OBJECT));
    if (filePointer) {
        size_t maxBytes = luaL_checknumber(L, 2);

        std::unique_ptr<char[]> bufPtr(new char[maxBytes]);

        size_t bytesRead = fread(bufPtr.get(), 1, maxBytes, filePointer);

        if (bytesRead < maxBytes && ferror(filePointer)) {
            return luaL_error(L, "помилка читання з файлу");
        }

        lua_pushlstring(L, bufPtr.get(), bytesRead);
        return 1;
    }
    return luaL_error(L, "read error");
}

static int lualilka_file_write(lua_State* L) {
    FILE* filePointer = *reinterpret_cast<FILE**>(luaL_checkudata(L, 1, FILE_OBJECT));
    if (filePointer) {
        String text = luaL_checkstring(L, 2);

        fprintf(filePointer, "%s", text.c_str());

        return 0;
    }
    return luaL_error(L, "write error");
}

int lualilka_sdcard_list_dir(lua_State* L) {
    int n = lua_gettop(L);

    if (n != 1) {
        return luaL_error(L, "Очікується 1 аргумент, отримано %d", n);
    }

    if (!lilka::fileutils.isSDAvailable()) {
        return luaL_error(L, "SD card not found");
    }

    String path = lua_tostring(L, 1);

    size_t _numEntries = lilka::fileutils.getEntryCount(&SD, path);
    if (_numEntries == 0) {
        return luaL_error(L, "Директорія порожня, або сталася помилка читання директорії");
    }

    lilka::Entry* entries = new lilka::Entry[_numEntries];

    int numEntries = lilka::fileutils.listDir(&SD, path, entries);
    std::unique_ptr<lilka::Entry[]> entriesPtr(entries);

    if (_numEntries != numEntries) {
        return luaL_error(L, "Не вдалося прочитати директорію");
    }

    lua_createtable(L, numEntries, 0);

    for (int i = 0; i < numEntries; i++) {
        lua_pushstring(L, entries[i].name.c_str());
        lua_rawseti(L, -2, i + 1); /* In lua indices start at 1 */
    }

    return 1;
}

int lualilka_sdcard_remove(lua_State* L) {
    int n = lua_gettop(L);

    if (n != 1) {
        return luaL_error(L, "Очікується 1 аргумент, отримано %d", n);
    }

    if (!lilka::fileutils.isSDAvailable()) {
        return luaL_error(L, "SD card not found");
    }

    String path = lua_tostring(L, 1);

    int ret = remove((lilka::fileutils.getSDRoot() + path).c_str());

    if (ret != 0) {
        return luaL_error(L, "Error remove file: %d", ret);
    }

    return 0;
}

int lualilka_sdcard_rename(lua_State* L) {
    int n = lua_gettop(L);

    if (n != 2) {
        return luaL_error(L, "Очікується 1 аргумент, отримано %d", n);
    }

    if (!lilka::fileutils.isSDAvailable()) {
        return luaL_error(L, "SD card not found");
    }

    String old_name = lua_tostring(L, 1);
    String new_name = lua_tostring(L, 2);

    int ret =
        rename((lilka::fileutils.getSDRoot() + old_name).c_str(), (lilka::fileutils.getSDRoot() + new_name).c_str());

    if (ret != 0) {
        return luaL_error(L, "Error renaming file: %d", ret);
    }

    return 0;
}

static const luaL_Reg lualilka_sdcard[] = {
    {"ls", lualilka_sdcard_list_dir},
    {"remove", lualilka_sdcard_remove},
    {"rename", lualilka_sdcard_rename},
    {"open", lualilka_create_object_file},
    {NULL, NULL},
};

int lualilka_sdcard_register(lua_State* L) {
    luaL_newlib(L, lualilka_sdcard);
    lua_setglobal(L, "sdcard");

    luaL_newmetatable(L, FILE_OBJECT);
    lua_pushcfunction(L, lualilka_delete_object_file);
    lua_setfield(L, -2, "__gc");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, lualilka_file_size);
    lua_setfield(L, -2, "size");
    lua_pushcfunction(L, lualilka_file_seek);
    lua_setfield(L, -2, "seek");
    lua_pushcfunction(L, lualilka_file_read);
    lua_setfield(L, -2, "read");
    lua_pushcfunction(L, lualilka_file_write);
    lua_setfield(L, -2, "write");

    lua_pop(L, 1);

    return 0;
}

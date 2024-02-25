#include "lualilka_resources.h"

namespace lilka {

int lualilka_resources_loadBitmap(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    // Get dir from registry
    lua_getfield(L, LUA_REGISTRYINDEX, "dir");
    const char* dir = lua_tostring(L, -1);
    lua_pop(L, 1);
    String fullPath = String(dir) + "/" + path;
    // 2nd argument is optional transparency color (16-bit 5-6-5)
    int32_t transparencyColor = -1;
    if (lua_gettop(L) > 1) {
        if (lua_isnumber(L, 2)) {
            transparencyColor = lua_tointeger(L, 2);
        }
    }

    Bitmap* bitmap = resources.loadBitmap(fullPath, transparencyColor);

    if (!bitmap) {
        return luaL_error(L, "Failed to load bitmap %s", fullPath.c_str());
    }

    serial_log("lua: loaded bitmap %s, width: %d, height: %d", path, bitmap->width, bitmap->height);

    // Append bitmap to bitmaps table in registry
    lua_getfield(L, LUA_REGISTRYINDEX, "bitmaps");
    lua_pushlightuserdata(L, bitmap);
    lua_setfield(L, -2, path);
    lua_pop(L, 1);

    // Create and return table that contains bitmap width, height and pointer
    lua_newtable(L);
    lua_pushinteger(L, bitmap->width);
    lua_setfield(L, -2, "width");
    lua_pushinteger(L, bitmap->height);
    lua_setfield(L, -2, "height");
    lua_pushlightuserdata(L, bitmap);
    lua_setfield(L, -2, "pointer");

    return 1;
}

int lualilka_resources_readFile(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    // Get dir from registry
    lua_getfield(L, LUA_REGISTRYINDEX, "dir");
    const char* dir = lua_tostring(L, -1);
    lua_pop(L, 1);
    String fullPath = String(dir) + "/" + path;

    String fileContent;
    int result = resources.readFile(fullPath, fileContent);
    if (result) {
        return luaL_error(L, "Failed to read file %s", fullPath.c_str());
    }

    lua_pushstring(L, fileContent.c_str());
    return 1;
}

int lualilka_resources_writeFile(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    const char* content = luaL_checkstring(L, 2);
    // Get dir from registry
    lua_getfield(L, LUA_REGISTRYINDEX, "dir");
    const char* dir = lua_tostring(L, -1);
    lua_pop(L, 1);
    String fullPath = String(dir) + "/" + path;

    int result = resources.writeFile(fullPath, content);
    if (result) {
        return luaL_error(L, "Failed to write file %s", fullPath.c_str());
    }

    return 0;
}

static const luaL_Reg lualilka_resources[] = {
    {"load_bitmap", lualilka_resources_loadBitmap},
    {"read_file", lualilka_resources_readFile},
    {"write_file", lualilka_resources_writeFile},
    {NULL, NULL},
};

// int luaopen_lilka_resources(lua_State* L) {
//     luaL_newlib(L, lualilka_resources);
//     return 1;
// }

int lualilka_resources_register(lua_State* L) {
    // Create global "resources" table that contains all resources functions
    luaL_newlib(L, lualilka_resources);
    lua_setglobal(L, "resources");
    return 0;
}

} // namespace lilka

#include "lualilka_resources.h"

namespace lilka {

int lualilka_resources_loadBitmap(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    // Get dir from registry
    lua_getfield(L, LUA_REGISTRYINDEX, "dir");
    const char* dir = lua_tostring(L, -1);
    lua_pop(L, 1);
    String fullPath = String(dir) + "/" + path;

    Bitmap* bitmap = resources.loadBitmap(path);

    if (!bitmap) {
        return luaL_error(L, "Failed to load bitmap %s", fullPath.c_str());
    }

    serial_log("Lua: loaded bitmap %s, width: %d, height: %d", path, bitmap->width, bitmap->height);

    // Append bitmap to bitmaps table in registry
    lua_getfield(L, LUA_REGISTRYINDEX, "bitmaps");
    lua_pushlightuserdata(L, bitmap);
    lua_setfield(L, -2, path);
    lua_pop(L, 1);

    // Return pointer to bitmap
    lua_pushlightuserdata(L, bitmap);

    return 1;
}

static const luaL_Reg lualilka_resources[] = {
    {"load_bitmap", lualilka_resources_loadBitmap},
    {NULL, NULL},
};

int luaopen_lilka_resources(lua_State* L) {
    luaL_newlib(L, lualilka_resources);
    return 1;
}

} // namespace lilka

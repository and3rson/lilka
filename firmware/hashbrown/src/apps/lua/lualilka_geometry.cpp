#include "lualilka_geometry.h"

int lualilka_geometry_intersectLines(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 8) {
        return luaL_error(L, "Очікується 8 аргументів, отримано %d", n);
    }

    float ax = luaL_checknumber(L, 1);
    float ay = luaL_checknumber(L, 2);
    float bx = luaL_checknumber(L, 3);
    float by = luaL_checknumber(L, 4);
    float cx = luaL_checknumber(L, 5);
    float cy = luaL_checknumber(L, 6);
    float dx = luaL_checknumber(L, 7);
    float dy = luaL_checknumber(L, 8);

    // I have no idea what I'm doing - Copilot wrote this for me, lol

    float denominator = (by - ay) * (dx - cx) - (bx - ax) * (dy - cy);
    if (denominator == 0) {
        lua_pushboolean(L, false);
        return 1;
    }

    float t = ((cy - ay) * (dx - cx) - (cx - ax) * (dy - cy)) / denominator;
    float u = -((ay - by) * (cx - ax) - (ax - bx) * (cy - ay)) / denominator;

    if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }
    return 1;
}

int lualilka_geometry_intersectAABB(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 8) {
        return luaL_error(L, "Очікується 8 аргументів, отримано %d", n);
    }

    float ax = luaL_checknumber(L, 1);
    float ay = luaL_checknumber(L, 2);
    float aw = luaL_checknumber(L, 3);
    float ah = luaL_checknumber(L, 4);
    float bx = luaL_checknumber(L, 5);
    float by = luaL_checknumber(L, 6);
    float bw = luaL_checknumber(L, 7);
    float bh = luaL_checknumber(L, 8);

    if (ax < bx + bw && ax + aw > bx && ay < by + bh && ay + ah > by) {
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }
    return 1;
}

static const luaL_Reg lualilka_geometry[] = {
    {"intersect_lines", lualilka_geometry_intersectLines},
    {"intersect_aabb", lualilka_geometry_intersectAABB},

    {NULL, NULL},
};

int lualilka_geometry_register(lua_State* L) {
    // Create global "geometry" table that contains all geometry functions
    luaL_newlib(L, lualilka_geometry);
    lua_setglobal(L, "geometry");
    return 0;
}

#include "lualilka_math.h"

namespace lilka {

int lualilka_math_random(lua_State* L) {
    // If no args - return random value from 0 to max int
    // If 1 arg - return random value from 0 to arg
    // If 2 args - return random value from arg1 to arg2

    int n = lua_gettop(L);

    if (n == 0) {
        lua_pushinteger(L, random());
        return 1;
    } else if (n == 1) {
        int max = luaL_checkinteger(L, 1);
        lua_pushinteger(L, random(max));
        return 1;
    } else if (n == 2) {
        int min = luaL_checkinteger(L, 1);
        int max = luaL_checkinteger(L, 2);
        lua_pushinteger(L, random(min, max));
        return 1;
    } else {
        return luaL_error(L, "Invalid number of arguments");
    }

    return 0;
}

int lualilka_math_clamp(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 3) {
        return luaL_error(L, "Invalid number of arguments");
    }

    float value = luaL_checknumber(L, 1);
    float min = luaL_checknumber(L, 2);
    float max = luaL_checknumber(L, 3);

    lua_pushnumber(L, value < min ? min : (value > max ? max : value));
    return 1;
}

int lualilka_math_lerp(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 3) {
        return luaL_error(L, "Invalid number of arguments");
    }

    float a = luaL_checknumber(L, 1);
    float b = luaL_checknumber(L, 2);
    float t = luaL_checknumber(L, 3);

    lua_pushnumber(L, a + (b - a) * t);
    return 1;
}

int lualilka_math_map(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 5) {
        return luaL_error(L, "Invalid number of arguments");
    }

    float value = luaL_checknumber(L, 1);
    float in_min = luaL_checknumber(L, 2);
    float in_max = luaL_checknumber(L, 3);
    float out_min = luaL_checknumber(L, 4);
    float out_max = luaL_checknumber(L, 5);

    lua_pushnumber(L, (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
    return 1;
}

int lualilka_math_dist(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 4) {
        return luaL_error(L, "Invalid number of arguments");
    }

    float x1 = luaL_checknumber(L, 1);
    float y1 = luaL_checknumber(L, 2);
    float x2 = luaL_checknumber(L, 3);
    float y2 = luaL_checknumber(L, 4);

    lua_pushnumber(L, sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)));
    return 1;
}

int lualilka_math_abs(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 1) {
        return luaL_error(L, "Invalid number of arguments");
    }

    float value = luaL_checknumber(L, 1);
    lua_pushnumber(L, value < 0 ? -value : value);
    return 1;
}

int lualilka_math_sign(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 1) {
        return luaL_error(L, "Invalid number of arguments");
    }

    int value = luaL_checknumber(L, 1);
    lua_pushinteger(L, value > 0 ? 1 : (value < 0 ? -1 : 0));
    return 1;
}

int lualilka_math_sqrt(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 1) {
        return luaL_error(L, "Invalid number of arguments");
    }

    float value = luaL_checknumber(L, 1);
    lua_pushnumber(L, sqrt(value));
    return 1;
}

int assert_table_arg(lua_State* L, int index) {
    // Check if there's only one argument, and if it's a table, and if it's not empty
    int n = lua_gettop(L);
    if (n != 1) {
        return luaL_error(L, "Invalid number of arguments");
    }

    if (!lua_istable(L, index)) {
        return luaL_error(L, "Argument is not a table");
    }

    if (lua_rawlen(L, index) == 0) {
        return luaL_error(L, "Table is empty");
    }

    return 0;
}

int lualilka_math_min(lua_State* L) {
    if (assert_table_arg(L, 1)) {
        return 0;
    }

    lua_pushinteger(L, 1);
    lua_gettable(L, 1);
    float min = luaL_checknumber(L, -1);
    lua_pop(L, 1);

    int n = lua_gettop(L);
    for (int i = 2; i <= n; i++) {
        lua_pushinteger(L, i);
        lua_gettable(L, 1);
        float value = luaL_checknumber(L, -1);
        if (value < min) {
            min = value;
        }
        lua_pop(L, 1);
    }

    lua_pushnumber(L, min);
    return 1;
}

int lualilka_math_max(lua_State* L) {
    if (assert_table_arg(L, 1)) {
        return 0;
    }

    lua_pushinteger(L, 1);
    lua_gettable(L, 1);
    float max = luaL_checknumber(L, -1);
    lua_pop(L, 1);

    int n = lua_gettop(L);
    for (int i = 2; i <= n; i++) {
        lua_pushinteger(L, i);
        lua_gettable(L, 1);
        float value = luaL_checknumber(L, -1);
        if (value > max) {
            max = value;
        }
        lua_pop(L, 1);
    }

    lua_pushnumber(L, max);
    return 1;
}

int lualilka_math_sum(lua_State* L) {
    if (assert_table_arg(L, 1)) {
        return 0;
    }

    float sum = 0;

    int n = lua_gettop(L);
    for (int i = 1; i <= n; i++) {
        lua_pushinteger(L, i);
        lua_gettable(L, 1);
        sum += luaL_checknumber(L, -1);
        lua_pop(L, 1);
    }

    lua_pushnumber(L, sum);
    return 1;
}

int lualilka_math_avg(lua_State* L) {
    if (assert_table_arg(L, 1)) {
        return 0;
    }

    float sum = 0;

    int n = lua_gettop(L);
    for (int i = 1; i <= n; i++) {
        lua_pushinteger(L, i);
        lua_gettable(L, 1);
        sum += luaL_checknumber(L, -1);
        lua_pop(L, 1);
    }

    lua_pushnumber(L, sum / n);
    return 1;
}

int lualilka_math_floor(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 1) {
        return luaL_error(L, "Invalid number of arguments");
    }

    float value = luaL_checknumber(L, 1);
    lua_pushinteger(L, floor(value));
    return 1;
}

int lualilka_math_ceil(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 1) {
        return luaL_error(L, "Invalid number of arguments");
    }

    float value = luaL_checknumber(L, 1);
    lua_pushinteger(L, ceil(value));
    return 1;
}

int lualilka_math_round(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 1) {
        return luaL_error(L, "Invalid number of arguments");
    }

    float value = luaL_checknumber(L, 1);
    lua_pushinteger(L, round(value));
    return 1;
}

static const luaL_Reg lualilka_math[] = {
    {"random", lualilka_math_random}, {"clamp", lualilka_math_clamp},
    {"lerp", lualilka_math_lerp},     {"map", lualilka_math_map},
    {"dist", lualilka_math_dist},     {"abs", lualilka_math_abs},
    {"sign", lualilka_math_sign},     {"sqrt", lualilka_math_sqrt},
    {"min", lualilka_math_min},       {"max", lualilka_math_max},
    {"sum", lualilka_math_sum},       {"avg", lualilka_math_avg},
    {"floor", lualilka_math_floor},   {"ceil", lualilka_math_ceil},
    {"round", lualilka_math_round},   {NULL, NULL},
};

// int luaopen_lilka_math(lua_State* L) {
//     luaL_newlib(L, lualilka_math);
//     return 1;
// }

int lualilka_math_register(lua_State* L) {
    // Create global "math" table that contains all math functions
    luaL_newlib(L, lualilka_math);
    lua_setglobal(L, "math");
    return 0;
}

} // namespace lilka

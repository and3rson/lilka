#include "lualilka_math.h"

int lualilka_math_random(lua_State* L) {
    // If no args - return random float from 0 to 1
    // If 1 arg - return random value from 0 to arg
    // If 2 args - return random value from arg1 to arg2

    int n = lua_gettop(L);

    if (n == 0) {
        lua_pushnumber(L, (float)rand() / RAND_MAX);
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
        return luaL_error(L, "Очікується 0, 1 або 2 аргументи, отримано %d", n);
    }

    return 0;
}

int lualilka_math_clamp(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 3) {
        return luaL_error(L, "Очікується 3 аргументи, отримано %d", n);
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
        return luaL_error(L, "Очікується 3 аргументи, отримано %d", n);
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
        return luaL_error(L, "Очікується 5 аргументів, отримано %d", n);
    }

    float value = luaL_checknumber(L, 1);
    float in_min = luaL_checknumber(L, 2);
    float in_max = luaL_checknumber(L, 3);
    float out_min = luaL_checknumber(L, 4);
    float out_max = luaL_checknumber(L, 5);

    lua_pushnumber(L, (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
    return 1;
}

int lualilka_math_abs(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 1) {
        return luaL_error(L, "Очікується 1 аргумент, отримано %d", n);
    }

    float value = luaL_checknumber(L, 1);
    lua_pushnumber(L, value < 0 ? -value : value);
    return 1;
}

int lualilka_math_sign(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 1) {
        return luaL_error(L, "Очікується 1 аргумент, отримано %d", n);
    }

    int value = luaL_checknumber(L, 1);
    lua_pushinteger(L, value > 0 ? 1 : (value < 0 ? -1 : 0));
    return 1;
}

int lualilka_math_sqrt(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 1) {
        return luaL_error(L, "Очікується 1 аргумент, отримано %d", n);
    }

    float value = luaL_checknumber(L, 1);
    lua_pushnumber(L, sqrt(value));
    return 1;
}

int lualilka_math_pow(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 2) {
        return luaL_error(L, "Очікується 2 аргументи, отримано %d", n);
    }

    float base = luaL_checknumber(L, 1);
    float exponent = luaL_checknumber(L, 2);
    lua_pushnumber(L, pow(base, exponent));
    return 1;
}

int assert_table_arg(lua_State* L, int index) {
    // Check if there's only one argument, and if it's a table, and if it's not empty
    int n = lua_gettop(L);
    if (n != 1) {
        return luaL_error(L, "Очікується 1 аргумент, отримано %d", n);
    }

    if (!lua_istable(L, index)) {
        return luaL_error(L, "Агрумент має бути таблицею");
    }

    if (lua_rawlen(L, index) == 0) {
        return luaL_error(L, "Таблиця не може бути пустою");
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
        return luaL_error(L, "Очікується 1 аргумент, отримано %d", n);
    }

    float value = luaL_checknumber(L, 1);
    lua_pushinteger(L, floor(value));
    return 1;
}

int lualilka_math_ceil(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 1) {
        return luaL_error(L, "Очікується 1 аргумент, отримано %d", n);
    }

    float value = luaL_checknumber(L, 1);
    lua_pushinteger(L, ceil(value));
    return 1;
}

int lualilka_math_round(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 1) {
        return luaL_error(L, "Очікується 1 аргумент, отримано %d", n);
    }

    float value = luaL_checknumber(L, 1);
    lua_pushinteger(L, roundf(value));
    return 1;
}

int lualilka_math_sin(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 1) {
        return luaL_error(L, "Очікується 1 аргумент, отримано %d", n);
    }

    float value = luaL_checknumber(L, 1);
    lua_pushnumber(L, sin(value));
    return 1;
}

int lualilka_math_cos(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 1) {
        return luaL_error(L, "Очікується 1 аргумент, отримано %d", n);
    }

    float value = luaL_checknumber(L, 1);
    lua_pushnumber(L, cos(value));
    return 1;
}

int lualilka_math_tan(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 1) {
        return luaL_error(L, "Очікується 1 аргумент, отримано %d", n);
    }

    float value = luaL_checknumber(L, 1);
    lua_pushnumber(L, tan(value));
    return 1;
}

int lualilka_math_asin(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 1) {
        return luaL_error(L, "Очікується 1 аргумент, отримано %d", n);
    }

    float value = luaL_checknumber(L, 1);
    lua_pushnumber(L, asin(value));
    return 1;
}

int lualilka_math_acos(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 1) {
        return luaL_error(L, "Очікується 1 аргумент, отримано %d", n);
    }

    float value = luaL_checknumber(L, 1);
    lua_pushnumber(L, acos(value));
    return 1;
}

int lualilka_math_atan(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 1) {
        return luaL_error(L, "Очікується 1 аргумент, отримано %d", n);
    }

    float value = luaL_checknumber(L, 1);
    lua_pushnumber(L, atan(value));
    return 1;
}

int lualilka_math_atan2(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 2) {
        return luaL_error(L, "Очікується 2 аргументи, отримано %d", n);
    }

    float y = luaL_checknumber(L, 1);
    float x = luaL_checknumber(L, 2);
    lua_pushnumber(L, atan2(y, x));
    return 1;
}

int lualilka_math_log(lua_State* L) {
    int n = lua_gettop(L);
    if (n == 1) {
        float value = luaL_checknumber(L, 1);
        lua_pushnumber(L, log(value));
        return 1;
    } else if (n == 2) {
        float value = luaL_checknumber(L, 1);
        float base = luaL_checknumber(L, 2);
        lua_pushnumber(L, log(value) / log(base));
        return 1;
    } else {
        return luaL_error(L, "Очікується 1 або 2 аргументи, отримано %d", n);
    }
}

int lualilka_math_deg(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 1) {
        return luaL_error(L, "Очікується 1 аргумент, отримано %d", n);
    }
    float value = luaL_checknumber(L, 1);
    lua_pushnumber(L, value * 180 / M_PI);
    return 1;
}

int lualilka_math_rad(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 1) {
        return luaL_error(L, "Очікується 1 аргумент, отримано %d", n);
    }
    float value = luaL_checknumber(L, 1);
    lua_pushnumber(L, value * M_PI / 180);
    return 1;
}

int lualilka_math_norm(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 2) {
        return luaL_error(L, "Очікується 2 аргументи, отримано %d", n);
    }
    float x = luaL_checknumber(L, 1);
    float y = luaL_checknumber(L, 2);
    float length = sqrt(x * x + y * y);
    lua_pushnumber(L, x / length);
    lua_pushnumber(L, y / length);
    return 2;
}

int lualilka_math_len(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 2) {
        return luaL_error(L, "Очікується 2 аргументи, отримано %d", n);
    }
    float x = luaL_checknumber(L, 1);
    float y = luaL_checknumber(L, 2);
    lua_pushnumber(L, sqrt(x * x + y * y));
    return 1;
}

int lualilka_math_dist(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 4) {
        return luaL_error(L, "Очікується 4 аргументи, отримано %d", n);
    }
    float x1 = luaL_checknumber(L, 1);
    float y1 = luaL_checknumber(L, 2);
    float x2 = luaL_checknumber(L, 3);
    float y2 = luaL_checknumber(L, 4);
    lua_pushnumber(L, sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)));
    return 1;
}

int lualilka_math_rotate(lua_State* L) {
    // Rotate vector (x, y) by angle (in degrees) around origin (Y-axis points down, clockwise)
    int n = lua_gettop(L);
    if (n != 3) {
        return luaL_error(L, "Очікується 3 аргументи, отримано %d", n);
    }
    float x = luaL_checknumber(L, 1);
    float y = luaL_checknumber(L, 2);
    float angle = luaL_checknumber(L, 3);
    float c = lilka::fCos360(angle);
    float s = lilka::fSin360(angle);
    lua_pushnumber(L, x * c - y * s);
    lua_pushnumber(L, x * s + y * c);
    return 2;
}

static const luaL_Reg lualilka_math[] = {
    {"random", lualilka_math_random},
    {"clamp", lualilka_math_clamp},
    {"lerp", lualilka_math_lerp},
    {"map", lualilka_math_map},
    {"abs", lualilka_math_abs},
    {"sign", lualilka_math_sign},
    {"sqrt", lualilka_math_sqrt},
    {"pow", lualilka_math_pow},
    {"min", lualilka_math_min},
    {"max", lualilka_math_max},
    {"sum", lualilka_math_sum},
    {"avg", lualilka_math_avg},
    {"floor", lualilka_math_floor},
    {"ceil", lualilka_math_ceil},
    {"round", lualilka_math_round},
    {"sin", lualilka_math_sin},
    {"cos", lualilka_math_cos},
    {"tan", lualilka_math_tan},
    {"asin", lualilka_math_asin},
    {"acos", lualilka_math_acos},
    {"atan", lualilka_math_atan},
    {"atan2", lualilka_math_atan2},
    {"log", lualilka_math_log},
    {"deg", lualilka_math_deg},
    {"rad", lualilka_math_rad},
    {"norm", lualilka_math_norm},
    {"len", lualilka_math_len},
    {"dist", lualilka_math_dist},
    {"rotate", lualilka_math_rotate},

    {NULL, NULL},
};

int lualilka_math_register(lua_State* L) {
    // Create global "math" table that contains all math functions
    luaL_newlib(L, lualilka_math);
    // Add PI constant
    lua_pushnumber(L, M_PI);
    lua_setfield(L, -2, "pi");
    // Add E constant
    lua_pushnumber(L, M_E);
    lua_setfield(L, -2, "e");
    // Add Tau constant
    lua_pushnumber(L, M_PI * 2);
    lua_setfield(L, -2, "tau");
    lua_setglobal(L, "math");
    return 0;
}

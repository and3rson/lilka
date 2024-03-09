#include <csetjmp>

#include "lualilka_util.h"

extern jmp_buf stopjmp;

int lualilka_util_time(lua_State* L) {
    float time = micros() / 1000000.0;
    lua_pushnumber(L, time);
    return 1;
}

int lualilka_util_sleep(lua_State* L) {
    float s = luaL_checknumber(L, 1);
    // delayMicroseconds(s * 1000000);
    vTaskDelay(s * 1000 / portTICK_PERIOD_MS);
    return 0;
}

int lualilka_util_exit(lua_State* L) {
    longjmp(stopjmp, 32); // TODO: 32 marks an "exit" condition
    return 0;
}

static const luaL_Reg lualilka_util[] = {
    {"time", lualilka_util_time},
    {"sleep", lualilka_util_sleep},
    {"exit", lualilka_util_exit},
    {NULL, NULL},
};

int lualilka_util_register(lua_State* L) {
    // Create global "util" table that contains all util functions
    luaL_newlib(L, lualilka_util);
    lua_setglobal(L, "util");
    return 0;
}

#include <csetjmp>

#include "lualilka_buzzer.h"

extern jmp_buf stopjmp;

int lualilka_buzzer_play(lua_State* L) {
    // Takes 1 or 2 args: frequency or frequency and duration
    int n = lua_gettop(L);
    if (n < 1 || n > 2) {
        return luaL_error(L, "Очікується 1 або 2 аргументи, отримано %d", n);
    }
    int freq = luaL_checkinteger(L, 1);
    if (n == 1) {
        lilka::buzzer.play(freq);
    } else {
        int duration = luaL_checkinteger(L, 2);
        lilka::buzzer.play(freq, duration);
    }
    return 0;
}

int lualilka_buzzer_playMelody(lua_State* L) {
    // Takes 2 arg: table of tones and tempo. Each tone is a table of 2 elements: frequency and duration
    // Convert them to array of arrays
    int n = lua_gettop(L);
    if (n != 2) {
        return luaL_error(L, "Очікується 2 аргументи, отримано %d", n);
    }
    luaL_checktype(L, 1, LUA_TTABLE);
    luaL_checktype(L, 2, LUA_TNUMBER);
    int size = luaL_len(L, 1);
    int tempo = luaL_checkinteger(L, 2);
    lilka::Tone melody[size];
    for (int i = 0; i < size; i++) {
        lua_geti(L, 1, i + 1);
        luaL_checktype(L, -1, LUA_TTABLE);
        lua_rawgeti(L, -1, 1);
        lua_rawgeti(L, -2, 2);
        melody[i].frequency = luaL_checkinteger(L, -2);
        melody[i].size = luaL_checkinteger(L, -1);
        lua_pop(L, 3);
    }
    lua_pop(L, 2);
    lilka::buzzer.playMelody(melody, size, tempo);
    return 0;
}

int lualilka_buzzer_stop(lua_State* L) {
    lilka::buzzer.stop();
    return 0;
}

static const luaL_Reg lualilka_buzzer[] = {
    {"play", lualilka_buzzer_play},
    {"play_melody", lualilka_buzzer_playMelody},
    {"stop", lualilka_buzzer_stop},
    {NULL, NULL},
};

int lualilka_buzzer_register(lua_State* L) {
    // Create global "buzzer" table that contains all buzzer functions
    luaL_newlib(L, lualilka_buzzer);
    lua_setglobal(L, "buzzer");
    return 0;
}

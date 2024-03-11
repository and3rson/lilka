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
    using namespace lilka;
    // Create global "buzzer" table that contains all buzzer functions
    luaL_newlib(L, lualilka_buzzer);
    lua_setglobal(L, "buzzer");
    // Create notes table for frequencies from B0 to D#8
    // clang-format off
    const lilka::Note notes[] = {
        NOTE_B0,
        NOTE_C1, NOTE_CS1, NOTE_D1, NOTE_DS1, NOTE_E1, NOTE_F1, NOTE_FS1, NOTE_G1, NOTE_GS1, NOTE_A1, NOTE_AS1, NOTE_B1,
        NOTE_C2, NOTE_CS2, NOTE_D2, NOTE_DS2, NOTE_E2, NOTE_F2, NOTE_FS2, NOTE_G2, NOTE_GS2, NOTE_A2, NOTE_AS2, NOTE_B2,
        NOTE_C3, NOTE_CS3, NOTE_D3, NOTE_DS3, NOTE_E3, NOTE_F3, NOTE_FS3, NOTE_G3, NOTE_GS3, NOTE_A3, NOTE_AS3, NOTE_B3,
        NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4,
        NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5,
        NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6,
        NOTE_C7, NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_F7, NOTE_FS7, NOTE_G7, NOTE_GS7, NOTE_A7, NOTE_AS7, NOTE_B7,
        NOTE_C8, NOTE_CS8, NOTE_D8, NOTE_DS8, REST,
    };
    const char* noteNames[] = {
        "B0",
        "C1", "CS1", "D1", "DS1", "E1", "F1", "FS1", "G1", "GS1", "A1", "AS1", "B1",
        "C2", "CS2", "D2", "DS2", "E2", "F2", "FS2", "G2", "GS2", "A2", "AS2", "B2",
        "C3", "CS3", "D3", "DS3", "E3", "F3", "FS3", "G3", "GS3", "A3", "AS3", "B3",
        "C4", "CS4", "D4", "DS4", "E4", "F4", "FS4", "G4", "GS4", "A4", "AS4", "B4",
        "C5", "CS5", "D5", "DS5", "E5", "F5", "FS5", "G5", "GS5", "A5", "AS5", "B5",
        "C6", "CS6", "D6", "DS6", "E6", "F6", "FS6", "G6", "GS6", "A6", "AS6", "B6",
        "C7", "CS7", "D7", "DS7", "E7", "F7", "FS7", "G7", "GS7", "A7", "AS7", "B7",
        "C8", "CS8", "D8", "DS8", "REST",
    };
    // clang-format on
    lua_newtable(L);
    for (int i = 0; i < sizeof(notes) / sizeof(notes[0]); i++) {
        lua_pushinteger(L, notes[i]);
        lua_setfield(L, -2, noteNames[i]);
    }
    lua_setglobal(L, "notes");
    return 0;
}

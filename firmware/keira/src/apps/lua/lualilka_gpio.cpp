#include "lualilka_gpio.h"

static int lualilka_gpio_setMode(lua_State* L) {
    int pin = luaL_checkinteger(L, 1);
    int mode = luaL_checkinteger(L, 2);
    pinMode(pin, mode);
    return 0;
}

static int lualilka_gpio_write(lua_State* L) {
    int pin = luaL_checkinteger(L, 1);
    int value = luaL_checkinteger(L, 2);
    digitalWrite(pin, value);
    return 0;
}

static int lualilka_gpio_read(lua_State* L) {
    int pin = luaL_checkinteger(L, 1);
    int value = digitalRead(pin);
    lua_pushinteger(L, value);
    return 1;
}

static int lualilka_gpio_analogRead(lua_State* L) {
    int pin = luaL_checkinteger(L, 1);
    int value = analogRead(pin);
    lua_pushinteger(L, value);
    return 1;
}

static const struct luaL_Reg lualilka_gpio[] = {
    {"set_mode", lualilka_gpio_setMode},
    {"write", lualilka_gpio_write},
    {"read", lualilka_gpio_read},
    {"analog_read", lualilka_gpio_analogRead},
    {NULL, NULL},
};

int lualilka_gpio_register(lua_State* L) {
    luaL_newlib(L, lualilka_gpio);
    // Add LOW and HIGH constants
    lua_pushinteger(L, LOW);
    lua_setfield(L, -2, "LOW");
    lua_pushinteger(L, HIGH);
    lua_setfield(L, -2, "HIGH");
    // Add INPUT, OUTPUT, and INPUT_PULLUP constants
    lua_pushinteger(L, INPUT);
    lua_setfield(L, -2, "INPUT");
    lua_pushinteger(L, OUTPUT);
    lua_setfield(L, -2, "OUTPUT");
    lua_pushinteger(L, INPUT_PULLUP);
    lua_setfield(L, -2, "INPUT_PULLUP");
    lua_pushinteger(L, INPUT_PULLDOWN);
    lua_setfield(L, -2, "INPUT_PULLDOWN");
    lua_setglobal(L, "gpio");
    return 0;
}

#include <HardwareSerial.h>
#include "lualilka_serial.h"

HardwareSerial LuaLilkaSerial(1);

static int lualilka_serial_avaliable(lua_State* L) {
    int value = LuaLilkaSerial.available();
    lua_pushinteger(L, value);
    return 1;
}

static int lualilka_serial_begin(lua_State* L) {
    int baud = luaL_checkinteger(L, 1);
    LuaLilkaSerial.begin(baud, SERIAL_8N1, LILKA_P4, LILKA_P3);
    return 0;
}

static int lualilka_serial_end(lua_State* L) {
    LuaLilkaSerial.end();
    return 0;
}

static int lualilka_serial_flush(lua_State* L) {
    LuaLilkaSerial.flush();
    return 0;
}

static int lualilka_serial_peek(lua_State* L) {
    int value = LuaLilkaSerial.peek();
    lua_pushinteger(L, value);
    return 1;
}

static int lualilka_serial_print(lua_State* L) {
    int n = lua_gettop(L);
    for (int i = 1; i <= n; i++) {
        if (lua_isstring(L, i)) {
            LuaLilkaSerial.print(lua_tostring(L, i));
        } else if (lua_isinteger(L, i)) {
            LuaLilkaSerial.print(lua_tointeger(L, i));
        } else if (lua_isnumber(L, i)) {
            LuaLilkaSerial.print(lua_tonumber(L, i));
        } else {
            LuaLilkaSerial.print(lua_typename(L, lua_type(L, i)));
        }
    }
    return 0;
}

static int lualilka_serial_println(lua_State* L) {
    int n = lua_gettop(L);
    for (int i = 1; i <= n; i++) {
        if (lua_isstring(L, i)) {
            LuaLilkaSerial.println(lua_tostring(L, i));
        } else if (lua_isinteger(L, i)) {
            LuaLilkaSerial.println(lua_tointeger(L, i));
        } else if (lua_isnumber(L, i)) {
            LuaLilkaSerial.println(lua_tonumber(L, i));
        } else {
            LuaLilkaSerial.println(lua_typename(L, lua_type(L, i)));
        }
    }
    return 0;
}

static int lualilka_serial_read(lua_State* L) {
    int value = LuaLilkaSerial.read();
    lua_pushinteger(L, value);
    return 1;
}

static int lualilka_serial_setTimeout(lua_State* L) {
    int timeout = luaL_checkinteger(L, 1);
    LuaLilkaSerial.setTimeout(timeout);
    return 0;
}

static int lualilka_serial_write(lua_State* L) {
    int n = lua_gettop(L);
    for (int i = 1; i <= n; i++) {
        if (lua_isstring(L, i)) {
            LuaLilkaSerial.write(lua_tostring(L, i));
        } else if (lua_isinteger(L, i)) {
            LuaLilkaSerial.write(lua_tointeger(L, i));
        // } else if (lua_isnumber(L, i)) {
        //     LuaLilkaSerial.write(lua_tonumber(L, i));
        } else {
            LuaLilkaSerial.write(lua_typename(L, lua_type(L, i)));
        }
    }
    return 0;
}

static const struct luaL_Reg lualilka_serial[] = {
    {"avaliable", lualilka_serial_avaliable},
    {"begin", lualilka_serial_begin},
    {"end", lualilka_serial_end},
    //{"find", lualilka_serial_find},
    //{"findUntil", lualilka_serial_findUntil},
    {"flush", lualilka_serial_flush},
    //{"parseFloat", lualilka_serial_parseFloat},
    //{"parseInt", lualilka_serial_parseInt},
    {"peek", lualilka_serial_peek},
    {"print", lualilka_serial_print},
    {"println", lualilka_serial_println},
    {"read", lualilka_serial_read},
    //{"readBytes", lualilka_serial_readBytes},
    //{"readBytesUntil", lualilka_serial_readBytesUntil},
    {"setTimeout", lualilka_serial_setTimeout},
    {"write", lualilka_serial_write},
    {NULL, NULL},
};

int lualilka_serial_register(lua_State* L) {
    luaL_newlib(L, lualilka_serial);
    lua_setglobal(L, "serial");
    return 0;
}

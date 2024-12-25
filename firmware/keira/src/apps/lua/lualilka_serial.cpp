#include <HardwareSerial.h>
#include "lualilka_serial.h"

#define DEFAULT_BAUD   115200
#define DEFAULT_CONFIG SERIAL_8N1

HardwareSerial LuaLilkaSerial(1);

static int lualilka_serial_available(lua_State* L) {
    int value = LuaLilkaSerial.available();
    lua_pushinteger(L, value);
    return 1;
}

static int lualilka_serial_begin(lua_State* L) {
    int baud = luaL_optinteger(L, 1, DEFAULT_BAUD);
    int config = luaL_optinteger(L, 2, DEFAULT_CONFIG);
    LuaLilkaSerial.begin(baud, config, LILKA_P4, LILKA_P3);
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
            // } else if (lua_islightuserdata(L, i)) {
            //     LuaLilkaSerial.print(lua_topointer(L, i));
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
            // } else if (lua_islightuserdata(L, i)) {
            //     LuaLilkaSerial.println(lua_topointer(L, i));
        } else {
            LuaLilkaSerial.println(lua_typename(L, lua_type(L, i)));
        }
    }
    return 0;
}

static int lualilka_serial_read(lua_State* L) {
    int bytes = luaL_optinteger(L, 1, 0);
    if (bytes <= 0) {
        int value = LuaLilkaSerial.read();
        lua_pushinteger(L, value);
    } else {
        char* buffer = new char[bytes + 1];
        bytes = LuaLilkaSerial.read(buffer, bytes);
        buffer[bytes] = 0;
        lua_pushstring(L, buffer);
        delete[] buffer;
    }
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
    {"available", lualilka_serial_available},
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
    // Add SerialConfig constants
    lua_pushinteger(L, SERIAL_5N1);
    lua_setfield(L, -2, "SERIAL_5N1");
    lua_pushinteger(L, SERIAL_6N1);
    lua_setfield(L, -2, "SERIAL_6N1");
    lua_pushinteger(L, SERIAL_7N1);
    lua_setfield(L, -2, "SERIAL_7N1");
    lua_pushinteger(L, SERIAL_8N1);
    lua_setfield(L, -2, "SERIAL_8N1");
    lua_pushinteger(L, SERIAL_5N2);
    lua_setfield(L, -2, "SERIAL_5N2");
    lua_pushinteger(L, SERIAL_6N2);
    lua_setfield(L, -2, "SERIAL_6N2");
    lua_pushinteger(L, SERIAL_7N2);
    lua_setfield(L, -2, "SERIAL_7N2");
    lua_pushinteger(L, SERIAL_8N2);
    lua_setfield(L, -2, "SERIAL_8N2");
    lua_pushinteger(L, SERIAL_5E1);
    lua_setfield(L, -2, "SERIAL_5E1");
    lua_pushinteger(L, SERIAL_6E1);
    lua_setfield(L, -2, "SERIAL_6E1");
    lua_pushinteger(L, SERIAL_7E1);
    lua_setfield(L, -2, "SERIAL_7E1");
    lua_pushinteger(L, SERIAL_8E1);
    lua_setfield(L, -2, "SERIAL_8E1");
    lua_pushinteger(L, SERIAL_5E2);
    lua_setfield(L, -2, "SERIAL_5E2");
    lua_pushinteger(L, SERIAL_6E2);
    lua_setfield(L, -2, "SERIAL_6E2");
    lua_pushinteger(L, SERIAL_7E2);
    lua_setfield(L, -2, "SERIAL_7E2");
    lua_pushinteger(L, SERIAL_8E2);
    lua_setfield(L, -2, "SERIAL_8E2");
    lua_pushinteger(L, SERIAL_5O1);
    lua_setfield(L, -2, "SERIAL_5O1");
    lua_pushinteger(L, SERIAL_6O1);
    lua_setfield(L, -2, "SERIAL_6O1");
    lua_pushinteger(L, SERIAL_7O1);
    lua_setfield(L, -2, "SERIAL_7O1");
    lua_pushinteger(L, SERIAL_8O1);
    lua_setfield(L, -2, "SERIAL_8O1");
    lua_pushinteger(L, SERIAL_5O2);
    lua_setfield(L, -2, "SERIAL_5O2");
    lua_pushinteger(L, SERIAL_6O2);
    lua_setfield(L, -2, "SERIAL_6O2");
    lua_pushinteger(L, SERIAL_7O2);
    lua_setfield(L, -2, "SERIAL_7O2");
    lua_pushinteger(L, SERIAL_8O2);
    lua_setfield(L, -2, "SERIAL_8O2");
    lua_setglobal(L, "serial");
    return 0;
}
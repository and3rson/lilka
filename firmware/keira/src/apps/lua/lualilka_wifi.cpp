

#include <csetjmp>

#include "WiFi.h"
#include "lualilka_wifi.h"
#include <lilka.h>


int lualilka_wifi_connect(lua_State* L){
    int n = lua_gettop(L);

    if (n != 2) {
        return luaL_error(L, "Очікується 2 аргументи, отримано %d", n);
    }

    const char* ssid = lua_tostring(L, 1);
    const char* password = lua_tostring(L, 2);
    WiFi.begin(ssid, password);

    return 0;
}

int lualilka_wifi_status(lua_State* L){
    lua_pushinteger(L, WiFi.status());
    return 1;
}

int lualilka_wifi_disconnect(lua_State* L){
    WiFi.disconnect();
    return 0;
}

int lualilka_controller_scanNetworks(lua_State* L) {

    int n = WiFi.scanNetworks();

    lua_createtable(L, n, 0);
    for (int i = 0; i < n; ++i) {
        lua_pushstring(L, WiFi.SSID(i).c_str());
        lua_rawseti (L, -2, i+1); /* In lua indices start at 1 */
    }

    return 1;
}

static const luaL_Reg lualilka_wifi[] = {
    {"connect", lualilka_wifi_connect},
    {"status", lualilka_wifi_status},
    {"disconnect", lualilka_wifi_disconnect},
    {"scan", lualilka_controller_scanNetworks},
    {NULL, NULL},
};

int lualilka_wifi_register(lua_State* L) {
    // Create global "wifi" table that contains all wifi functions
    luaL_newlib(L, lualilka_wifi);
    lua_setglobal(L, "wifi");
    return 0;
}
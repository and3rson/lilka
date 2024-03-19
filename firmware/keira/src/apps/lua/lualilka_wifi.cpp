

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

int lualilka_controller_getRSSI(lua_State* L){
    int n = lua_gettop(L);
        if (n != 1) {
        return luaL_error(L, "Очікується 1 аргументи, отримано %d", n);
    }

    int wifiAccessPoint = lua_tointeger(L, 1) - 1; /* In lua indices start at 1 */

    lua_pushinteger(L, WiFi.RSSI(wifiAccessPoint));

    return 1;
}

int lualilka_controller_getEncryptionType(lua_State* L){
    int n = lua_gettop(L);
        if (n != 1) {
        return luaL_error(L, "Очікується 1 аргументи, отримано %d", n);
    }

    int wifiAccessPoint = lua_tointeger(L, 1) - 1; /* In lua indices start at 1 */

    lua_pushinteger(L, WiFi.encryptionType(wifiAccessPoint));

    return 1;
} 

int lualilka_controller_getMAC(lua_State* L){
    byte mac[6];

    WiFi.macAddress(mac);

    String s;
    for (byte i = 0; i < sizeof(mac); i++)
    {
        char buf[3];
        sprintf(buf, "%02X", mac[i]); // J-M-L: slight modification, added the 0 in the format for padding 
        s += buf;
        if (i < 5) s += ':';
    }

    lua_pushstring(L, s.c_str());

    return 1;
} 

int lualilka_controller_getIP(lua_State* L){
    byte mac[4];

    IPAddress ip = WiFi.localIP();

    lua_pushstring(L, ip.toString().c_str());

    return 1;
} 

int lualilka_controller_config(lua_State* L){
    int n = lua_gettop(L);
    if (n != 5 ) {
        return luaL_error(L, "Очікується 5 аргументів, отримано %d", n);
    }
    IPAddress ip, gateway, subnet, dns1, dns2;

    ip.fromString(lua_tostring(L, 1));
    gateway.fromString(lua_tostring(L, 2));
    subnet.fromString(lua_tostring(L, 3));
    dns1.fromString(lua_tostring(L, 4));
    dns2.fromString(lua_tostring(L, 5));

    WiFi.config(ip, gateway, subnet, dns1, dns2);
    
    return 0;
}

static const luaL_Reg lualilka_wifi[] = {
    {"connect", lualilka_wifi_connect},
    {"status", lualilka_wifi_status},
    {"disconnect", lualilka_wifi_disconnect},
    {"scan", lualilka_controller_scanNetworks},
    {"rssi", lualilka_controller_getRSSI},
    {"encryptionType", lualilka_controller_getRSSI}, // TKIP (WPA) = 2 WEP = 5 CCMP (WPA) = 4 NONE = 7 AUTO = 8
    {"getMAC", lualilka_controller_getMAC},
    {"getLocalIP", lualilka_controller_getMAC},
    {"config", lualilka_controller_config},
    {NULL, NULL},
};

int lualilka_wifi_register(lua_State* L) {
    // Create global "wifi" table that contains all wifi functions
    luaL_newlib(L, lualilka_wifi);
    lua_setglobal(L, "wifi");
    return 0;
}
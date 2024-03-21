

#include <csetjmp>

#include "WiFi.h"
#include "lualilka_wifi.h"
#include <lilka.h>

int lualilka_wifi_connect(lua_State* L) {
    int n = lua_gettop(L);

    if (n != 2) {
        return luaL_error(L, "Очікується 2 аргументи, отримано %d", n);
    }

    const char* ssid = lua_tostring(L, 1);
    const char* password = lua_tostring(L, 2);
    WiFi.begin(ssid, password);

    return 0;
}

int lualilka_wifi_get_status(lua_State* L) {
    lua_pushinteger(L, WiFi.status());
    return 1;
}

int lualilka_wifi_disconnect(lua_State* L) {
    WiFi.disconnect();
    return 0;
}

int lualilka_wifi_scan_networks(lua_State* L) {
    int n = WiFi.scanNetworks();

    lua_createtable(L, n, 0);
    for (int i = 0; i < n; ++i) {
        lua_pushstring(L, WiFi.SSID(i).c_str());
        lua_rawseti(L, -2, i + 1); /* In lua indices start at 1 */
    }

    return 1;
}

int lualilka_wifi_get_rssi(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 1) {
        return luaL_error(L, "Очікується 1 аргументи, отримано %d", n);
    }

    int wifiAccessPoint = lua_tointeger(L, 1) - 1; /* In lua indices start at 1 */

    lua_pushinteger(L, WiFi.RSSI(wifiAccessPoint));

    return 1;
}

int lualilka_wifi_get_encryption_type(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 1) {
        return luaL_error(L, "Очікується 1 аргументи, отримано %d", n);
    }

    int wifiAccessPoint = lua_tointeger(L, 1) - 1; /* In lua indices start at 1 */

    lua_pushinteger(L, WiFi.encryptionType(wifiAccessPoint));

    return 1;
}

int lualilka_wifi_get_mac(lua_State* L) {
    byte mac[6];

    WiFi.macAddress(mac);

    String s;
    for (byte i = 0; i < sizeof(mac); i++) {
        char buf[3];
        sprintf(buf, "%02X", mac[i]);
        s += buf;
        if (i < 5) s += ':';
    }

    lua_pushstring(L, s.c_str());

    return 1;
}

int lualilka_wifi_get_ip(lua_State* L) {
    byte mac[4];

    IPAddress ip = WiFi.localIP();

    lua_pushstring(L, ip.toString().c_str());

    return 1;
}

int lualilka_wifi_set_config(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 5) {
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
    {"get_status", lualilka_wifi_get_status},
    {"disconnect", lualilka_wifi_disconnect},
    {"scan", lualilka_wifi_scan_networks},
    {"get_rssi", lualilka_wifi_get_rssi},
    {"get_encryption_type", lualilka_wifi_get_encryption_type
    }, // TKIP (WPA) = 2 WEP = 5 CCMP (WPA) = 4 NONE = 7 AUTO = 8
    {"get_mac", lualilka_wifi_get_mac},
    {"get_local_ip", lualilka_wifi_get_ip},
    {"set_config", lualilka_wifi_set_config},
    {NULL, NULL},
};

int lualilka_wifi_register(lua_State* L) {
    // Create global "wifi" table that contains all wifi functions
    luaL_newlib(L, lualilka_wifi);
    lua_setglobal(L, "wifi");
    return 0;
}
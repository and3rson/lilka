#include <HTTPClient.h>
#include "lualilka_http.h"

#define STR_HELPER(x) #x
#define STR(x)        STR_HELPER(x)
const char* defaultUserAgent = "Lilka/" STR(LILKA_VERSION);

static int lualilka_http_execute(lua_State* L) {
    int n = lua_gettop(L);
    if (n != 1) {
        return luaL_error(L, "Очікується 1 аргумент, отримано %d", n);
    }

    if (!lua_istable(L, 1)) {
        return luaL_error(L, "Агрумент має бути таблицею");
    }

    const char* url = nullptr;
    const char* method = nullptr;
    const char* body = nullptr;
    const char* fileName = nullptr;

    lua_pushnil(L);
    while (lua_next(L, 1) != 0) {
        if (lua_type(L, -2) == LUA_TSTRING) {
            const char* key = lua_tostring(L, -2);
            if (lua_type(L, -1) == LUA_TSTRING) {
                if (strcmp(key, "url") == 0) {
                    url = lua_tostring(L, -1);
                } else if (strcmp(key, "method") == 0) {
                    method = lua_tostring(L, -1);
                } else if (strcmp(key, "body") == 0) {
                    body = lua_tostring(L, -1);
                } else if (strcmp(key, "file") == 0) {
                    fileName = lua_tostring(L, -1);
                }
            }
        }
        lua_pop(L, 1);
    }

    if (method == nullptr) {
        if (body == nullptr) {
            method = "GET";
        } else {
            method = "POST";
        }
    }

    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    http.setUserAgent(defaultUserAgent);
    http.begin(client, url);

    int statusCode;
    if (body == nullptr) {
        statusCode = http.sendRequest(method);
    } else {
        statusCode = http.sendRequest(method, String(body));
    }

    lua_newtable(L);
    lua_pushstring(L, "code");
    lua_pushnumber(L, statusCode);
    lua_settable(L, -3);
    if (statusCode == HTTP_CODE_OK) {
        if (fileName != 0) {
            WiFiClient* stream = http.getStreamPtr();
            File file = SD.open(fileName, FILE_WRITE, true);
            if (!file) {
                http.end();
                return luaL_error(L, "Не вдалося відкрити %s", fileName);
            }

            uint8_t buffer[128];
            while (stream->connected() && stream->available()) {
                size_t bytes = stream->readBytes(buffer, sizeof(buffer));
                if (bytes > 0) {
                    file.write(buffer, bytes);
                }
            }
            file.close();
        } else {
            String response = http.getString();
            lua_pushstring(L, "response");
            lua_pushstring(L, response.c_str());
            lua_settable(L, -3);
        }
    }

    return 1;
}

static const struct luaL_Reg lualilka_http[] = {
    {"execute", lualilka_http_execute},
    {NULL, NULL},
};

int lualilka_http_register(lua_State* L) {
    luaL_newlib(L, lualilka_http);
    lua_pushnumber(L, HTTP_CODE_OK);
    lua_setfield(L, -2, "HTTP_CODE_OK");
    lua_setglobal(L, "http");

    return 0;
}
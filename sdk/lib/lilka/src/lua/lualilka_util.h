#ifndef LUALILKA_UTIL_H
#define LUALILKA_UTIL_H

#include <lua.hpp>
#include <Arduino.h>

namespace lilka {

// int luaopen_lilka_util(lua_State* L);
int lualilka_util_register(lua_State* L);

}

#endif // LUALILKA_UTIL_H

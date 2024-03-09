#pragma once

#include <lua.hpp>
#include <lilka.h>

int lualilka_state_load(lua_State* L, const char* path);
int lualilka_state_save(lua_State* L, const char* path);

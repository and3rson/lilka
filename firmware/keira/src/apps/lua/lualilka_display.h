#pragma once

#include <lua.hpp>
#include <lilka.h>

lilka::Canvas* getDrawable(lua_State* L);
int lualilka_display_register(lua_State* L);

#ifndef LUALILKA_DISPLAY_H
#define LUALILKA_DISPLAY_H

#include <lua.hpp>
#include <lilka.h>

namespace lilka {

// int luaopen_lilka_display(lua_State *L);
int lualilka_display_register(lua_State *L);

}

#endif // LUALILKA_DISPLAY_H

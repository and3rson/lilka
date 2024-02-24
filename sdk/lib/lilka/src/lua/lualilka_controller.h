#ifndef LUALILKA_CONTROLLER_H
#define LUALILKA_CONTROLLER_H

#include <lilka.h>
#include <lua.hpp>

namespace lilka {

// int luaopen_lilka_controller(lua_State* L);
int lualilka_controller_register(lua_State* L);

}

#endif // LUALILKA_CONTROLLER_H

#ifndef LUALILKA_RESOURCES_H
#define LUALILKA_RESOURCES_H

#include <lilka.h>
#include <lua.hpp>

namespace lilka {

// int luaopen_lilka_resources(lua_State* L);
int lualilka_resources_register(lua_State* L);

}

#endif // LUALILKA_RESOURCES_H

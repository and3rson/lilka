#ifndef LUALILKA_CONSOLE_H
#define LUALILKA_CONSOLE_H

#include <lua.hpp>
#include <lilka.h>

namespace lilka {

// int luaopen_lilka_console(lua_State* L);
int lualilka_console_register(lua_State* L);

}

#endif // LUALILKA_CONSOLE_H

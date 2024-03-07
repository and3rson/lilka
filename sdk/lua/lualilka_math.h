#ifndef LUALILKA_MATH_H
#define LUALILKA_MATH_H

#include <lua.hpp>
#include <lilka.h>

namespace lilka {

// int luaopen_lilka_math(lua_State *L);
int lualilka_math_register(lua_State* L);

}

#endif // LUALILKA_MATH_H

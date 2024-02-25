#ifndef LUALILKA_STATE_H
#define LUALILKA_STATE_H

#include <lua.hpp>
#include <lilka.h>

namespace lilka {

int lualilka_state_load(lua_State* L, const char* path);
int lualilka_state_save(lua_State* L, const char* path);

} // namespace lilka

#endif // LUALILKA_STATE_H

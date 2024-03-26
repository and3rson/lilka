#pragma once

#include <lua.hpp>
#include <Arduino.h>

#define OBJECT_DIALOG_ALERT             "alertDialog"
#define OBJECT_DIALOG_PROGRESS_ALERT    "progressDialog"

int lualilka_dialog_register_inputDialog(lua_State* L);
int lualilka_dialog_register_alertDialog(lua_State* L);
int lualilka_dialog_register_progressDialog(lua_State* L);

#pragma once

#include <lua.hpp>
#include <Arduino.h>

#define OBJECT_DIALOG_KEYBOARD "keyboardDialog"
#define OBJECT_DIALOG_ALERT    "alertDialog"
#define OBJECT_DIALOG_PROGRESS "progressDialog"

int lualilka_dialog_register_keyboardDialog(lua_State* L);
int lualilka_dialog_register_alertDialog(lua_State* L);
int lualilka_dialog_register_progressDialog(lua_State* L);

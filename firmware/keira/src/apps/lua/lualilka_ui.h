#pragma once

#include <lua.hpp>
#include <Arduino.h>

#define OBJECT_UI_KEYBOARD "keyboardUI"
#define OBJECT_UI_ALERT    "alertUI"
#define OBJECT_UI_PROGRESS "progressUI"

int lualilka_UI_register_keyboard(lua_State* L);
int lualilka_UI_register_alert(lua_State* L);
int lualilka_UI_register_progress(lua_State* L);

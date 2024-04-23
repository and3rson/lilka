#pragma once

#include <lua.hpp>
#include <Arduino.h>
#include <SD.h>
#define FILE_OBJECT "File"

int lualilka_sdcard_register(lua_State* L);

#ifndef LILKA_LUARUNNER_H
#define LILKA_LUARUNNER_H

#include <Arduino.h>
#include "lilka/display.h"

namespace lilka {

int lua_runfile(Canvas *canvas, String path);
int lua_runsource(Canvas *canvas, String source);
int lua_repl_start();
int lua_repl_input(String input);
int lua_repl_stop();

} // namespace lilka

#endif // LILKA_LUARUNNER_H

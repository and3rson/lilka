#ifndef LILKA_LUARUNNER_H
#define LILKA_LUARUNNER_H

#include <Arduino.h>

namespace lilka {

int lua_runfile(String path);
int lua_runsource(String source);
int lua_repl_start();
int lua_repl_input(String input);
int lua_repl_stop();

}

#endif // LILKA_LUARUNNER_H

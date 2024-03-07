#pragma once

#include <Arduino.h>
#include "../app.h"

class LuaRunnerApp : public App {
    LuaRunnerApp();
    void run();
};

class LuaFileRunnerApp : public LuaRunnerApp {
    LuaFileRunnerApp(String path);
};

class LuaSourceRunnerApp : public LuaRunnerApp {
    LuaSourceRunnerApp(String source);
};

class LuaReplApp : public LuaRunnerApp {
    LuaReplApp();
    // input(String input);
    // stop();
};

// int lua_runfile(lilka::Canvas *canvas, String path);
// int lua_runsource(lilka::Canvas *canvas, String source);
// int lua_repl_start();
// int lua_repl_input(String input);
// int lua_repl_stop();

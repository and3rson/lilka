#pragma once

#include <Arduino.h>
#include <lua.hpp>
#include "app.h"

// Abstract Lua runner app. Sets up Lua VM and provides a method to run Lua code.
// Does not implement the run method.
class AbstractLuaRunnerApp : public App {
public:
    AbstractLuaRunnerApp(const char *name);

protected:
    void luaSetup(const char *dir);
    void luaTeardown();
    int execute();
    lua_State *L;
};

// Lua runner app that runs a file.
class LuaFileRunnerApp : public AbstractLuaRunnerApp {
public:
    LuaFileRunnerApp(String path);

private:
    void run();
    String path;
};

// Lua runner app that runs a string.
class LuaSourceRunnerApp : public AbstractLuaRunnerApp {
public:
    LuaSourceRunnerApp(String source);

private:
    void run();
    String source;
};

// class LuaReplApp : public AbstractLuaRunnerApp {
//     LuaReplApp();
//     void run();
//     // input(String input);
//     // stop();
// };

// int lua_runfile(lilka::Canvas *canvas, String path);
// int lua_runsource(lilka::Canvas *canvas, String source);
// int lua_repl_start();
// int lua_repl_input(String input);
// int lua_repl_stop();

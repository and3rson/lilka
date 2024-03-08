#pragma once

#include <Arduino.h>
#include <lua.hpp>
#include "app.h"

class AbstractLuaRunnerApp : public App {
public:
    AbstractLuaRunnerApp(const char *name);
    virtual void run() = 0;

protected:
    void luaSetup(const char *dir);
    void luaTeardown();
    int execute();
    lua_State *L;
};

class LuaFileRunnerApp : public AbstractLuaRunnerApp {
public:
    LuaFileRunnerApp(String path);
    void run();

private:
    String path;
};

class LuaSourceRunnerApp : public AbstractLuaRunnerApp {
public:
    LuaSourceRunnerApp(String source);
    void run();

private:
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

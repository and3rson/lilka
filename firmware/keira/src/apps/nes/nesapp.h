#pragma once
#include "app.h"

class NesApp : public App {
public:
    explicit NesApp(String path);
    ~NesApp();

private:
    void run();

    char* argv[1];
};

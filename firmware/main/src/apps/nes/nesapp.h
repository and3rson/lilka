#pragma once
#include "app.h"

class NesApp : public App {
public:
    NesApp(String path);

private:
    void run();

    char *argv[1];
};

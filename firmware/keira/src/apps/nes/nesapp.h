#pragma once
#include "app.h"

class NesApp : public App {
public:
    explicit NesApp(String path);
    ~NesApp();

private:
    void run() override;

    char* argv[1];
};

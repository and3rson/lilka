#pragma once
#include "app.h"

class SNesApp : public App {
public:
    explicit SNesApp(String path);

private:
    void run();
    void alert(const char *title, const char *message);

    String romPath;
};

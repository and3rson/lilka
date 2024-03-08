#pragma once

#include "app.h"

class LauncherApp : public App {
public:
    LauncherApp();
    void run();
    void demosMenu();
    void sdBrowserMenu(String path);
    void selectFile(String path);
};

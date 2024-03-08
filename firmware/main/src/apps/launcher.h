#pragma once

#include "app.h"

class LauncherApp : public App {
public:
    LauncherApp();

private:
    void run();
    void demosMenu();
    void sdBrowserMenu(String path);
    void spiffsBrowserMenu();
    void devMenu();
    void systemUtilsMenu();

    void selectFile(String path);
    void alert(String title, String message);
};

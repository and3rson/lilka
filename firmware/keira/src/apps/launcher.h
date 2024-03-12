#pragma once

#include "app.h"

#define APP_CLASS_LIST       std::vector<std::function<App*()>>
#define APP_CLASS(className) []() { return new className(); }

class LauncherApp : public App {
public:
    LauncherApp();

private:
    void run() override;
    void appsMenu();
    void sdBrowserMenu(String path);
    void spiffsBrowserMenu();
    void devMenu();
    void settingsMenu();

    void selectFile(String path);
    void alert(String title, String message);
};

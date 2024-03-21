#pragma once

#include "app.h"

typedef struct {
    const char* name;
    std::function<App*()> construct;
} app_t;

#define APP_ITEM_LIST std::vector<app_t>
#define APP_ITEM(name, className)              \
    {                                          \
        name, []() { return new className(); } \
    }

class LauncherApp : public App {
public:
    LauncherApp();

private:
    void run() override;
    void appsMenu();
    void sdBrowserMenu(String path);
    void spiffsBrowserMenu();
    void fileBrowserMenu(String path);
    void selectFile(String path);
    void devMenu();
    void settingsMenu();

    void alert(String title, String message);
};

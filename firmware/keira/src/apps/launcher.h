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
typedef enum {
    FB_SD,
    FB_SPIFFS,
} FileBrowserType;

class LauncherApp : public App {
public:
    LauncherApp();

private:
    void run() override;
    void appsMenu();
    void fileBrowserMenu(String path, int fbrowserType);
    void devMenu();
    void settingsMenu();

    void selectFile(String path);
    void alert(String title, String message);
};

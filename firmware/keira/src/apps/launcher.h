#pragma once

#include "app.h"

typedef enum {
    APP_ITEM_TYPE_APP,
    APP_ITEM_TYPE_SUBMENU,
} app_item_type_t;

typedef struct item_t {
    app_item_type_t type;
    const char* name;
    std::function<App*()> construct;
    std::vector<item_t> submenu;
} item_t;

#define ITEM_LIST std::vector<item_t>

#define ITEM_APP(itemName, className)                                    \
    {                                                                    \
        APP_ITEM_TYPE_APP, itemName, []() { return new className(); }, { \
        }                                                                \
    }

#define ITEM_SUBMENU(itemName, ...)              \
    {                                            \
        APP_ITEM_TYPE_SUBMENU, itemName, NULL, { \
            __VA_ARGS__                          \
        }                                        \
    }

class LauncherApp : public App {
public:
    LauncherApp();

private:
    void run() override;
    void appsMenu(const char* title, ITEM_LIST& menu);
    void demosMenu();
    void sdBrowserMenu(FS* fSysDriver, const String& path);
    void devMenu();
    void settingsMenu();

    void selectFile(String path);
    void alert(String title, String message);
};

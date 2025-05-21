#pragma once

#include <app.h>
#include "appmanager.h"
// APPS:
#include "../modplayer/modplayer.h"
#include "../liltracker/liltracker.h"
#include "../lua/luarunner.h"
#include "../mjs/mjsrunner.h"
#include "../nes/nesapp.h"
#include "../fmanager/fmanager.h"

// FILE HANDLERS:  ////////////////////////////////////////////////////////////////////////////////////
#define NES_HANDLER(X)        AppManager::getInstance()->runApp(new NesApp(X));
#define LUA_SCRIPT_HANDLER(X) AppManager::getInstance()->runApp(new LuaFileRunnerApp(X));
#define JS_SCRIPT_HANDLER(X)  AppManager::getInstance()->runApp(new MJSApp(X));
#define MOD_HANDLER(X)        AppManager::getInstance()->runApp(new ModPlayerApp(X));
#define LT_HANDLER(X)         AppManager::getInstance()->runApp(new LilTrackerApp(X))

typedef struct {
    String source;
    String target;
    FileType type;
} catalog_entry_file;

typedef struct {
    String name;
    String description;
    String author;
    std::vector<catalog_entry_file> files;
} catalog_entry;

typedef struct {
    String name;
    std::vector<catalog_entry> entries;
} catalog_category;

class LilCatalogApp : public App {
public:
    LilCatalogApp();

private:
    String catalog_url;
    String path_catalog_file;
    String path_catalog_folder;

    lilka::Menu categoriesMenu;
    lilka::Menu entriesMenu;

    std::vector<catalog_category> catalog;

    void showAlert(const String& message);

    void parseCatalog();
    void fetchCatalog();

    void fetchEntry();
    bool validateEntry();
    void removeEntry();
    void executeEntry();

    void fileLoadAsRom(const String& path);

    void drawCatalog();
    void drawCategory();
    void drawEntry();
    void drawEntryDescription();

    void run() override;
};
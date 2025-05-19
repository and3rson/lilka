#pragma once

#include <app.h>

typedef struct {
    String source;
    String target;
    String type;
} catalog_entry_file;

typedef struct {
    String name;
    String description;
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

    void drawCatalog();
    void drawCategory();
    void drawEntry();

    void run() override;
};
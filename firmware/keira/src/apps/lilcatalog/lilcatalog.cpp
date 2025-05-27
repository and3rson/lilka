#include "lilcatalog.h"
#include <HTTPClient.h>
#include <lilka/config.h>

#include "utils/json.h"

LilCatalogApp::LilCatalogApp() : App(LILCATALOG_S_APP) {
    setStackSize(8192);
    path_catalog_folder = "/lilcatalog";
    path_catalog_file = "/lilcatalog/catalog.json";
    catalog_url = "https://raw.githubusercontent.com/and3rson/lilka/refs/heads/main/firmware/keira/src/apps/"
                  "lilcatalog/catalog.json";
}

void LilCatalogApp::run() {
    if (!lilka::fileutils.isSDAvailable()) {
        showAlert(LILCATALOG_S_SD_NOTFOUND);
        lilka::serial.log("SD card not found. Cannot continue.");
        return;
    } else {
        lilka::serial.log("SD card found.");
    }

    parseCatalog();
    doShowCatalog();

    while (1) {
        uiLoop();
    }
}

void LilCatalogApp::doShowCatalog() {
    state = LILCATALOG_CATALOG;
    categoriesMenu.clearItems();
    categoriesMenu.setTitle(LILCATALOG_S_APP);

    for (int i = 0; i < catalog.size(); i++) {
        catalog_category& category = catalog[i];
        categoriesMenu.addItem(
            category.name,
            nullptr,
            0,
            String(category.entries.size()) + LILCATALOG_S_CATEGORY_POSTFIX,
            reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::doShowCategory),
            reinterpret_cast<void*>(this)
        );
    }
    categoriesMenu.addItem(
        LILCATALOG_S_FETCH_CATALOG,
        0,
        0,
        LILCATALOG_S_EMPTY,
        reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::fetchCatalog),
        reinterpret_cast<void*>(this)
    );
    categoriesMenu.addItem(
        LILCATALOG_S_STOP,
        0,
        0,
        LILCATALOG_S_EMPTY,
        reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::stop),
        reinterpret_cast<void*>(this)
    );

    categoriesMenu.setCursor(0);
    categoriesMenu.update();
}

void LilCatalogApp::doShowCategory() {
    state = LILCATALOG_CATEGORY;
    u8_t categoryIndex = categoriesMenu.getCursor();
    catalog_category category = catalog[categoryIndex];

    entriesMenu.clearItems();
    entriesMenu.setTitle(category.name);

    for (int i = 0; i < category.entries.size(); i++) {
        catalog_entry& entry = category.entries[i];
        entriesMenu.addItem(
            entry.name,
            nullptr,
            0,
            entry.author,
            reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::doShowEntry),
            reinterpret_cast<void*>(this)
        );
    }
    entriesMenu.addItem(
        LILCATALOG_S_BACK,
        0,
        0,
        LILCATALOG_S_EMPTY,
        reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::doShowCatalog),
        reinterpret_cast<void*>(this)
    );

    entriesMenu.setCursor(0);
    entriesMenu.update();
}

void LilCatalogApp::doShowEntry() {
    state = LILCATALOG_ENTRY;
    u8_t categoryIndex = categoriesMenu.getCursor();
    u8_t entryIndex = entriesMenu.getCursor();
    catalog_entry entry = catalog[categoryIndex].entries[entryIndex];

    entryMenu.clearItems();
    entryMenu.setTitle(entry.name);

    bool entryInstalled = validateEntry();
    if (entryInstalled) {
        entryMenu.addItem(
            LILCATALOG_S_START,
            0,
            0,
            LILCATALOG_S_EMPTY,
            reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::executeEntry),
            reinterpret_cast<void*>(this)
        );
        entryMenu.addItem(
            LILCATALOG_S_REMOVE,
            0,
            0,
            LILCATALOG_S_EMPTY,
            reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::removeEntry),
            reinterpret_cast<void*>(this)
        );
        entryMenu.addItem(
            LILCATALOG_S_UPDATE,
            0,
            0,
            LILCATALOG_S_EMPTY,
            reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::fetchEntry),
            reinterpret_cast<void*>(this)
        );
    } else {
        entryMenu.addItem(
            LILCATALOG_S_INSTALL,
            0,
            0,
            LILCATALOG_S_EMPTY,
            reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::fetchEntry),
            reinterpret_cast<void*>(this)
        );
    }
    entryMenu.addItem(
        LILCATALOG_S_ENTRY_DESCRIPTION,
        0,
        0,
        LILCATALOG_S_EMPTY,
        reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::doShowEntryDescription),
        reinterpret_cast<void*>(this)
    );
    entryMenu.addItem(
        LILCATALOG_S_BACK,
        0,
        0,
        LILCATALOG_S_EMPTY,
        reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::doShowCategory),
        reinterpret_cast<void*>(this)
    );

    entryMenu.setCursor(0);
    entryMenu.update();
}

void LilCatalogApp::doShowEntryDescription() {
    state = LILCATALOG_ENTRY_DESCRIPTION;
}

void LilCatalogApp::uiLoop() {
    switch (state) {
        case LILCATALOG_CATALOG:
            categoriesMenu.update();
            categoriesMenu.draw(canvas);
            queueDraw();
            break;
        case LILCATALOG_CATEGORY:
            entriesMenu.update();
            entriesMenu.draw(canvas);
            queueDraw();
            break;
        case LILCATALOG_ENTRY:
            entryMenu.update();
            entryMenu.draw(canvas);
            queueDraw();
            break;
        case LILCATALOG_ENTRY_DESCRIPTION:
            u8_t categoryIndex = categoriesMenu.getCursor();
            u8_t entryIndex = entriesMenu.getCursor();
            catalog_entry entry = catalog[categoryIndex].entries[entryIndex];
            String description = LILCATALOG_S_ENTRY_DESCRIPTION_NAME + entry.name + "\n" +
                                 LILCATALOG_S_ENTRY_DESCRIPTION_AUTHOR + entry.author + "\n" +
                                 LILCATALOG_S_ENTRY_DESCRIPTION_DESCRIPTION + entry.description + "\n" +
                                 LILCATALOG_S_ENTRY_DESCRIPTION_FILES;
            for (int i = 0; i < entry.files.size(); i++) {
                catalog_entry_file entry_file = entry.files[i];
                description += entry_file.target + "\n";
            }
            showAlert(description);
            state = LILCATALOG_ENTRY;
            break;
    }
    lilka::serial.log("LilCatalogApp::uiLoop() state: %d", state);
}

void LilCatalogApp::parseCatalog() {
    catalog.clear();

    JsonDocument data(&spiRamAllocator);

    if (!SD.exists(path_catalog_folder)) {
        lilka::serial.log("f_mkdir %d", path_catalog_folder);
        if (!SD.mkdir(path_catalog_folder.c_str())) {
            showAlert(LILCATALOG_S_ERROR_CREATE_FOLDER);
        }
    }

    if (!SD.exists(path_catalog_file)) {
        showAlert(LILCATALOG_S_ERROR_NO_CATALOG);
    } else {
        fs::File file = SD.open(path_catalog_file.c_str(), FILE_READ);
        String fileContent = "";

        while (file.available()) {
            fileContent += (char)file.read();
        }

        lilka::serial.log("Downloaded %d byte", fileContent.length());
        lilka::serial.log(fileContent.c_str());
        file.close();

        DeserializationError error = deserializeJson(data, fileContent);
        if (error) {
            showAlert(LILCATALOG_S_ERROR_LOAD_CATALOG);
        } else {
            lilka::serial.log("Catalog categories: %d", data.size());
            for (int i = 0; i < data.size(); i++) {
                catalog_category category;
                category.name = data[i]["category"].as<String>();

                for (int j = 0; j < data[i]["entries"].size(); j++) {
                    catalog_entry entry;
                    entry.name = data[i]["entries"][j]["name"].as<String>();
                    entry.description = data[i]["entries"][j]["description"].as<String>();
                    entry.author = data[i]["entries"][j]["author"].as<String>();
                    for (int k = 0; k < data[i]["entries"][j]["files"].size(); k++) {
                        catalog_entry_file entry_file;
                        entry_file.source = data[i]["entries"][j]["files"][k]["source"].as<String>();
                        entry_file.target = data[i]["entries"][j]["files"][k]["target"].as<String>();
                        String fileType = data[i]["entries"][j]["files"][k]["type"].as<String>();
                        if (fileType == "nes") {
                            entry_file.type = FT_NES_ROM;
                        } else if (fileType == "bin") {
                            entry_file.type = FT_BIN;
                        } else if (fileType == "lua") {
                            entry_file.type = FT_LUA_SCRIPT;
                        } else if (fileType == "js") {
                            entry_file.type = FT_JS_SCRIPT;
                        } else if (fileType == "mod") {
                            entry_file.type = FT_MOD;
                        } else if (fileType == "lt") {
                            entry_file.type = FT_LT;
                        } else {
                            entry_file.type = FT_OTHER;
                        }
                        entry.files.push_back(entry_file);
                    }
                    category.entries.push_back(entry);
                    lilka::serial.log("Category: %s Content: %s", category.name, entry.name);
                }
                catalog.push_back(category);

                lilka::serial.log("Category: '%s', Elements: '%d'", category.name.c_str(), category.entries.size());
            }
        }
    }
}

void LilCatalogApp::fetchCatalog() {
    lilka::Alert alert(LILCATALOG_S_APP, LILCATALOG_S_FILE_LOADING);
    alert.draw(canvas);
    queueDraw();

    WiFiClientSecure client;
    HTTPClient http;

    client.setInsecure();
    http.begin(client, catalog_url);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK) {
        fs::File file = SD.open(path_catalog_file.c_str(), FILE_WRITE);
        if (!file) {
            showAlert(LILCATALOG_S_ERROR_FILE_OPEN);
            return;
        }
        file.print(http.getString());
        file.close();

        delay(10);

        showAlert(LILCATALOG_S_FILE_LOADING_COMPLETE);
        parseCatalog();
    } else {
        showAlert(LILCATALOG_S_ERROR_CONNECTION + http.errorToString(httpCode));
    }
}

void LilCatalogApp::fetchEntry() {
    lilka::Alert alert(LILCATALOG_S_APP, LILCATALOG_S_FILE_LOADING);
    alert.draw(canvas);
    queueDraw();

    u8_t categoryIndex = categoriesMenu.getCursor();
    u8_t entryIndex = entriesMenu.getCursor();
    catalog_entry entry = catalog[categoryIndex].entries[entryIndex];

    WiFiClientSecure client;
    HTTPClient http;

    client.setInsecure();
    for (int i = 0; i < entry.files.size(); i++) {
        catalog_entry_file entry_file = entry.files[i];
        lilka::serial.log("Source file %s", entry_file.source.c_str());
        lilka::serial.log("Target file %s", entry_file.target.c_str());

        if (!lilka::fileutils.makePath(&SD, lilka::fileutils.getParentDirectory(entry_file.target))) {
            showAlert(LILCATALOG_S_ERROR_DIRETORY_CREATE);
            return;
        }

        http.begin(client, entry_file.source);
        int httpCode = http.GET();

        if (httpCode == HTTP_CODE_OK) {
            String destination = entry_file.target;
            fs::File file = SD.open(destination.c_str(), FILE_WRITE);

            if (!file) {
                showAlert(LILCATALOG_S_ERROR_FILE_OPEN);
                return;
            }
            file.print(http.getString());
            file.close();

            delay(10);
        } else {
            showAlert(LILCATALOG_S_ERROR_CONNECTION + http.errorToString(httpCode));
            return;
        }
    }

    showAlert(LILCATALOG_S_FILE_LOADING_COMPLETE);
}

bool LilCatalogApp::validateEntry() {
    u8_t categoryIndex = categoriesMenu.getCursor();
    u8_t entryIndex = entriesMenu.getCursor();
    catalog_entry entry = catalog[categoryIndex].entries[entryIndex];

    for (int i = 0; i < entry.files.size(); i++) {
        catalog_entry_file entry_file = entry.files[i];
        if (!SD.exists(entry_file.target)) {
            return false;
        }
    }
    return true;
}

void LilCatalogApp::removeEntry() {
    u8_t categoryIndex = categoriesMenu.getCursor();
    u8_t entryIndex = entriesMenu.getCursor();
    catalog_entry entry = catalog[categoryIndex].entries[entryIndex];

    for (int i = 0; i < entry.files.size(); i++) {
        catalog_entry_file entry_file = entry.files[i];
        SD.remove(entry_file.target);

        //remove parent folder if empty
        String parentFolder = lilka::fileutils.getParentDirectory(entry_file.target);
        SD.rmdir(parentFolder.c_str());
    }
}

void LilCatalogApp::executeEntry() {
    u8_t categoryIndex = categoriesMenu.getCursor();
    u8_t entryIndex = entriesMenu.getCursor();
    catalog_entry entry = catalog[categoryIndex].entries[entryIndex];
    catalog_entry_file entry_file = entry.files[0];

    lilka::serial.log("Opening path %s", entry_file.target.c_str());

    FileType fileType = entry_file.type;
    String path = lilka::fileutils.getCannonicalPath(&SD, entry_file.target);
    switch (fileType) {
        case FT_NES_ROM:
            NES_HANDLER(path);
            break;
        case FT_BIN:
            fileLoadAsRom(path);
            break;
        case FT_LUA_SCRIPT:
            LUA_SCRIPT_HANDLER(path);
            break;
        case FT_JS_SCRIPT:
            JS_SCRIPT_HANDLER(path);
            break;
        case FT_MOD:
            MOD_HANDLER(path);
            break;
        case FT_LT:
            LT_HANDLER(path);
            break;

        case FT_OTHER:

            break;
    }
    vTaskDelay(100 / portTICK_RATE_MS);
}

void LilCatalogApp::fileLoadAsRom(const String& path) {
    lilka::ProgressDialog dialog(LILCATALOG_S_LOADING, path + "\n\n" + LILCATALOG_S_STARTING);
    dialog.draw(canvas);
    queueDraw();
    int error;
    error = lilka::multiboot.start(path);
    if (error) {
        showAlert(String(LILCATALOG_S_ERROR_STAGE1) + error);
        return;
    }
    dialog.setMessage(path + "\n\n" + LILCATALOG_S_SIZE + String(lilka::multiboot.getBytesTotal()));
    dialog.draw(canvas);
    queueDraw();
    while ((error = lilka::multiboot.process()) > 0) {
        int progress = lilka::multiboot.getBytesWritten() * 100 / lilka::multiboot.getBytesTotal();
        dialog.setProgress(progress);
        dialog.draw(canvas);
        queueDraw();
        if (lilka::controller.getState().a.justPressed) {
            lilka::multiboot.cancel();
            return;
        }
    }
    if (error < 0) {
        showAlert(String(LILCATALOG_S_ERROR_STAGE2) + error);
        return;
    }
    error = lilka::multiboot.finishAndReboot();
    if (error) {
        showAlert(String(LILCATALOG_S_ERROR_STAGE3) + error);
        return;
    }
}

void LilCatalogApp::showAlert(const String& message) {
    lilka::Alert alert(LILCATALOG_S_APP, message);
    alert.draw(canvas);
    queueDraw();
    while (!alert.isFinished()) {
        alert.update();
    }
    lilka::serial.log(message.c_str());
}

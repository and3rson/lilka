#include "lilcatalog.h"
#include <HTTPClient.h>
#include <lilka/config.h>

#include "utils/json.h"

LilCatalogApp::LilCatalogApp() : App(K_S_LILCATALOG_APP) {
    setStackSize(8192);
    path_catalog_folder = "/lilcatalog";
    path_catalog_file = "/lilcatalog/catalog.json";
    catalog_url = "https://raw.githubusercontent.com/lilka-dev/lilka/refs/heads/main/firmware/keira/src/apps/"
                  "lilcatalog/catalog.json";
}

void LilCatalogApp::run() {
    if (!lilka::fileutils.isSDAvailable()) {
        showAlert(K_S_LILCATALOG_SD_NOTFOUND);
        lilka::serial.log("SD card not found. Cannot continue.");
        return;
    } else {
        lilka::serial.log("SD card found.");
    }

    parseCatalog();
    doShowCatalog();

    while (1) {
        processMenu();
        processBackButton();
        queueDraw();
    }
}

void LilCatalogApp::processMenu() {
    switch (state) {
        case LILCATALOG_CATALOG:
            catalogMenu.update();
            catalogMenu.draw(canvas);
            break;
        case LILCATALOG_CATEGORY:
            categoryMenu.update();
            categoryMenu.draw(canvas);
            break;
        case LILCATALOG_ENTRY:
            entryMenu.update();
            entryMenu.draw(canvas);
            break;
        default:
            break;
    }
}

void LilCatalogApp::processBackButton() {
    if (lilka::controller.peekState().b.justPressed) {
        lilka::serial.log("B button pressed, going back.");
        switch (state) {
            case LILCATALOG_CATALOG:
                stop();
                return;
            case LILCATALOG_CATEGORY:
                doShowCatalog();
                break;
            case LILCATALOG_ENTRY:
                doShowCategory();
                break;
            case LILCATALOG_ENTRY_DESCRIPTION:
                doShowEntry();
                break;
            default:
                break;
        }
    }
}

void LilCatalogApp::doShowCatalog() {
    catalogMenu.clearItems();
    catalogMenu.setTitle(K_S_LILCATALOG_APP);
    state = LILCATALOG_CATALOG;

    for (int i = 0; i < catalog.size(); i++) {
        catalog_category& categoty_tmp = catalog[i];
        catalogMenu.addItem(
            categoty_tmp.name,
            nullptr,
            0,
            String(categoty_tmp.entries.size()) + K_S_LILCATALOG_CATEGORY_POSTFIX,
            reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::doShowCategory),
            reinterpret_cast<void*>(this)
        );
    }
    catalogMenu.addItem(
        K_S_LILCATALOG_FETCH_CATALOG,
        0,
        0,
        K_S_LILCATALOG_EMPTY,
        reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::fetchCatalog),
        reinterpret_cast<void*>(this)
    );
    catalogMenu.addItem(
        K_S_LILCATALOG_STOP,
        0,
        0,
        K_S_LILCATALOG_EMPTY,
        reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::stop),
        reinterpret_cast<void*>(this)
    );

    catalogMenu.setCursor(0);
    catalogMenu.update();
}

void LilCatalogApp::doShowCategory() {
    u8_t categoryIndex = catalogMenu.getCursor();
    category = catalog[categoryIndex];
    state = LILCATALOG_CATEGORY;

    categoryMenu.clearItems();
    categoryMenu.setTitle(category.name);

    for (int i = 0; i < category.entries.size(); i++) {
        catalog_entry& entry_tmp = category.entries[i];
        categoryMenu.addItem(
            entry_tmp.name,
            nullptr,
            0,
            entry_tmp.author,
            reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::doShowEntry),
            reinterpret_cast<void*>(this)
        );
    }
    categoryMenu.addItem(
        K_S_LILCATALOG_BACK,
        0,
        0,
        K_S_LILCATALOG_EMPTY,
        reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::doShowCatalog),
        reinterpret_cast<void*>(this)
    );

    categoryMenu.setCursor(0);
    categoryMenu.update();
}

void LilCatalogApp::doShowEntry() {
    u8_t entryIndex = categoryMenu.getCursor();
    entry = category.entries[entryIndex];
    state = LILCATALOG_ENTRY;

    entryMenu.clearItems();
    entryMenu.setTitle(entry.name);

    bool entryInstalled = validateEntry();
    if (entryInstalled) {
        entryMenu.addItem(
            K_S_LILCATALOG_START,
            0,
            0,
            K_S_LILCATALOG_EMPTY,
            reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::executeEntry),
            reinterpret_cast<void*>(this)
        );
        entryMenu.addItem(
            K_S_LILCATALOG_REMOVE,
            0,
            0,
            K_S_LILCATALOG_EMPTY,
            reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::removeEntry),
            reinterpret_cast<void*>(this)
        );
        entryMenu.addItem(
            K_S_LILCATALOG_UPDATE,
            0,
            0,
            K_S_LILCATALOG_EMPTY,
            reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::fetchEntry),
            reinterpret_cast<void*>(this)
        );
    } else {
        entryMenu.addItem(
            K_S_LILCATALOG_INSTALL,
            0,
            0,
            K_S_LILCATALOG_EMPTY,
            reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::fetchEntry),
            reinterpret_cast<void*>(this)
        );
    }
    entryMenu.addItem(
        K_S_LILCATALOG_ENTRY_DESCRIPTION,
        0,
        0,
        K_S_LILCATALOG_EMPTY,
        reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::doShowEntryDescription),
        reinterpret_cast<void*>(this)
    );
    entryMenu.addItem(
        K_S_LILCATALOG_BACK,
        0,
        0,
        K_S_LILCATALOG_EMPTY,
        reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::doShowCategory),
        reinterpret_cast<void*>(this)
    );

    entryMenu.setCursor(0);
    entryMenu.update();
}

void LilCatalogApp::doShowEntryDescription() {
    state = LILCATALOG_ENTRY_DESCRIPTION;
    String description = K_S_LILCATALOG_ENTRY_DESCRIPTION_NAME + entry.name + "\n" +
                         K_S_LILCATALOG_ENTRY_DESCRIPTION_AUTHOR + entry.author + "\n" +
                         K_S_LILCATALOG_ENTRY_DESCRIPTION_DESCRIPTION + entry.description + "\n" +
                         K_S_LILCATALOG_ENTRY_DESCRIPTION_FILES;
    for (int i = 0; i < entry.files.size(); i++) {
        catalog_entry_file entry_file = entry.files[i];
        description += entry_file.target + "\n";
    }
    showAlert(description);
    state = LILCATALOG_ENTRY;
}

void LilCatalogApp::parseCatalog() {
    catalog.clear();

    JsonDocument data(&spiRamAllocator);

    if (!SD.exists(path_catalog_folder)) {
        lilka::serial.log("f_mkdir %d", path_catalog_folder);
        if (!SD.mkdir(path_catalog_folder.c_str())) {
            showAlert(K_S_LILCATALOG_ERROR_CREATE_FOLDER);
        }
    }

    if (!SD.exists(path_catalog_file)) {
        showAlert(K_S_LILCATALOG_ERROR_NO_CATALOG);
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
            showAlert(K_S_LILCATALOG_ERROR_LOAD_CATALOG);
        } else {
            lilka::serial.log("Catalog categories: %d", data.size());
            for (int i = 0; i < data.size(); i++) {
                catalog_category category_tmp;
                category_tmp.name = data[i]["category"].as<String>();

                for (int j = 0; j < data[i]["entries"].size(); j++) {
                    catalog_entry entry_tmp;
                    entry_tmp.name = data[i]["entries"][j]["name"].as<String>();
                    entry_tmp.description = data[i]["entries"][j]["description"].as<String>();
                    entry_tmp.author = data[i]["entries"][j]["author"].as<String>();
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
                        entry_tmp.files.push_back(entry_file);
                    }
                    category_tmp.entries.push_back(entry_tmp);
                    lilka::serial.log("Category: %s Content: %s", category_tmp.name, entry_tmp.name);
                }
                catalog.push_back(category_tmp);

                lilka::serial.log(
                    "Category: '%s', Elements: '%d'", category_tmp.name.c_str(), category_tmp.entries.size()
                );
            }
        }
    }
}

void LilCatalogApp::fetchCatalog() {
    lilka::Alert alert(K_S_LILCATALOG_APP, K_S_LILCATALOG_FILE_LOADING);
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
            showAlert(K_S_LILCATALOG_ERROR_FILE_OPEN);
            return;
        }
        file.print(http.getString());
        file.close();

        delay(10);

        showAlert(K_S_LILCATALOG_FILE_LOADING_COMPLETE);
        parseCatalog();
    } else {
        showAlert(K_S_LILCATALOG_ERROR_CONNECTION + http.errorToString(httpCode));
    }
}

void LilCatalogApp::fetchEntry() {
    lilka::Alert alert(K_S_LILCATALOG_APP, K_S_LILCATALOG_FILE_LOADING);
    alert.draw(canvas);
    queueDraw();

    WiFiClientSecure client;
    HTTPClient http;

    client.setInsecure();
    for (int i = 0; i < entry.files.size(); i++) {
        catalog_entry_file entry_file = entry.files[i];
        lilka::serial.log("Source file %s", entry_file.source.c_str());
        lilka::serial.log("Target file %s", entry_file.target.c_str());

        if (!lilka::fileutils.makePath(&SD, lilka::fileutils.getParentDirectory(entry_file.target))) {
            showAlert(K_S_LILCATALOG_ERROR_DIRETORY_CREATE);
            return;
        }

        http.begin(client, entry_file.source);
        int httpCode = http.GET();

        if (httpCode == HTTP_CODE_OK) {
            String destination = entry_file.target;
            fs::File file = SD.open(destination.c_str(), FILE_WRITE);

            if (!file) {
                showAlert(K_S_LILCATALOG_ERROR_FILE_OPEN);
                return;
            }
            file.print(http.getString());
            file.close();

            delay(10);
        } else {
            showAlert(K_S_LILCATALOG_ERROR_CONNECTION + http.errorToString(httpCode));
            return;
        }
    }

    showAlert(K_S_LILCATALOG_FILE_LOADING_COMPLETE);
}

bool LilCatalogApp::validateEntry() {
    for (int i = 0; i < entry.files.size(); i++) {
        catalog_entry_file entry_file = entry.files[i];
        if (!SD.exists(entry_file.target)) {
            return false;
        }
    }
    return true;
}

void LilCatalogApp::removeEntry() {
    for (int i = 0; i < entry.files.size(); i++) {
        catalog_entry_file entry_file = entry.files[i];
        SD.remove(entry_file.target);

        //remove parent folder if empty
        String parentFolder = lilka::fileutils.getParentDirectory(entry_file.target);
        SD.rmdir(parentFolder.c_str());
    }
}

void LilCatalogApp::executeEntry() {
    catalog_entry_file entry_file = entry.files[0];

    lilka::serial.log("Opening path %s", entry_file.target.c_str());

    FileType fileType = entry_file.type;
    String path = lilka::fileutils.getCannonicalPath(&SD, entry_file.target);
    switch (fileType) {
        case FT_NES_ROM:
            K_FT_NES_HANDLER(path);
            break;
        case FT_BIN:
            fileLoadAsRom(path);
            break;
        case FT_LUA_SCRIPT:
            K_FT_LUA_SCRIPT_HANDLER(path);
            break;
        case FT_JS_SCRIPT:
            K_FT_JS_SCRIPT_HANDLER(path);
            break;
        case FT_MOD:
            K_FT_MOD_HANDLER(path);
            break;
        case FT_LT:
            K_FT_LT_HANDLER(path);
            break;

        case FT_OTHER:

            break;
    }
    vTaskDelay(100 / portTICK_RATE_MS);
}

void LilCatalogApp::fileLoadAsRom(const String& path) {
    lilka::ProgressDialog dialog(K_S_LILCATALOG_LOADING, path + "\n\n" + K_S_LILCATALOG_STARTING);
    dialog.draw(canvas);
    queueDraw();
    int error;
    error = lilka::multiboot.start(path);
    if (error) {
        showAlert(String(K_S_LILCATALOG_ERROR_STAGE1) + error);
        return;
    }
    dialog.setMessage(path + "\n\n" + K_S_LILCATALOG_SIZE + String(lilka::multiboot.getBytesTotal()));
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
        showAlert(String(K_S_LILCATALOG_ERROR_STAGE2) + error);
        return;
    }
    error = lilka::multiboot.finishAndReboot();
    if (error) {
        showAlert(String(K_S_LILCATALOG_ERROR_STAGE3) + error);
        return;
    }
}

void LilCatalogApp::showAlert(const String& message) {
    lilka::Alert alert(K_S_LILCATALOG_APP, message);
    alert.draw(canvas);
    queueDraw();
    while (!alert.isFinished()) {
        alert.update();
    }
    lilka::serial.log(message.c_str());
}

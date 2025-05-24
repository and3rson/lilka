#include "lilcatalog.h"
#include <HTTPClient.h>
#include <lilka/config.h>

#include "utils/json.h"

LilCatalogApp::LilCatalogApp() : App("ЛілКаталог") {
    setStackSize(8192);
    path_catalog_folder = "/lilcatalog";
    path_catalog_file = "/lilcatalog/catalog.json";
    catalog_url = "https://raw.githubusercontent.com/and3rson/lilka/refs/heads/main/firmware/keira/src/apps/"
                  "lilcatalog/catalog.json";

    catalog.clear();
    categoriesMenu.setTitle("ЛілКаталог");
}

void LilCatalogApp::run() {
    if (!lilka::fileutils.isSDAvailable()) {
        showAlert("SD карта не знайдена. Неможливо продовжити");
        lilka::serial.log("SD карта не знайдена");
        return;
    } else {
        lilka::serial.log("SD карта знайдена");
    }

    parseCatalog();
    while (1) {
        drawCatalog();
    }
}

void LilCatalogApp::parseCatalog() {
    catalog.clear();

    JsonDocument data(&spiRamAllocator);

    if (!SD.exists(path_catalog_folder)) {
        lilka::serial.log("f_mkdir %d", path_catalog_folder);
        if (!SD.mkdir(path_catalog_folder.c_str())) {
            showAlert("Помилка створеня каталогу");
        }
    }

    if (!SD.exists(path_catalog_file)) {
        showAlert("Каталог не знайдено. Завантажте його з інтернету");
    } else {
        fs::File file = SD.open(path_catalog_file.c_str(), FILE_READ);
        String fileContent = "";

        while (file.available()) {
            fileContent += (char)file.read();
        }

        lilka::serial.log("Завантажено %d байт", fileContent.length());
        lilka::serial.log(fileContent.c_str());
        file.close();

        DeserializationError error = deserializeJson(data, fileContent);
        if (error) {
            showAlert("Помилка завантаження каталогу");
        } else {
            lilka::serial.log("Категорії у каталозі: %d", data.size());
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
                    lilka::serial.log("Категорія: %s Контент: %s", category.name, entry.name);
                }
                catalog.push_back(category);

                lilka::serial.log(
                    "Категорія: %s, кількість додатків: %d", category.name.c_str(), category.entries.size()
                );
            }
        }
    }
}

void LilCatalogApp::fetchCatalog() {
    lilka::Alert alert("lilcatalog", "Завантаження файлу...");
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
            showAlert("Помилка відкриття файлу");
            return;
        }
        file.print(http.getString());
        file.close();

        delay(10);

        showAlert("Файл завантажено, та збережено");
        parseCatalog();
    } else {
        showAlert("Помилка. Помилка підключення" + http.errorToString(httpCode));
    }
}

void LilCatalogApp::fetchEntry() {
    lilka::Alert alert("lilcatalog", "Завантаження файлу...");
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
        lilka::serial.log("Завантаження %s", entry_file.source.c_str());
        lilka::serial.log("Збереження %s", entry_file.target.c_str());

        if (!lilka::fileutils.makePath(&SD, lilka::fileutils.getParentDirectory(entry_file.target))) {
            showAlert("Помилка створення каталогу");
            return;
        }

        http.begin(client, entry_file.source);
        int httpCode = http.GET();

        if (httpCode == HTTP_CODE_OK) {
            String destination = entry_file.target;
            fs::File file = SD.open(destination.c_str(), FILE_WRITE);

            if (!file) {
                showAlert("Помилка відкриття файлу");
                return;
            }
            file.print(http.getString());
            file.close();

            delay(10);
        } else {
            showAlert("Помилка. Помилка підключення" + http.errorToString(httpCode));
            return;
        }
    }

    showAlert("Файл завантажено, та збережено");
    drawEntry();
}

void LilCatalogApp::drawCatalog() {
    categoriesMenu.clearItems();

    for (u_int i = 0; i < catalog.size(); i++) {
        String size = String(catalog[i].entries.size());
        categoriesMenu.addItem(
            catalog[i].name.c_str(),
            0,
            0,
            size.c_str(),
            reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::drawCategory),
            reinterpret_cast<void*>(this)
        );
    }
    categoriesMenu.addItem(
        "Завантажити каталог",
        0,
        0,
        "",
        reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::fetchCatalog),
        reinterpret_cast<void*>(this)
    );

    while (!categoriesMenu.isFinished()) {
        categoriesMenu.update();
        categoriesMenu.draw(canvas);
        queueDraw();
    }
}

void LilCatalogApp::drawCategory() {
    u8_t categoryIndex = categoriesMenu.getCursor();
    catalog_category& category = catalog[categoryIndex];
    entriesMenu.setTitle(category.name.c_str());
    entriesMenu.clearItems();
    for (u_int j = 0; j < category.entries.size(); j++) {
        entriesMenu.addItem(
            category.entries[j].name.c_str(),
            0,
            0,
            "",
            reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::drawEntry),
            reinterpret_cast<void*>(this)
        );
    }
    entriesMenu.addItem(
        "Назад",
        0,
        0,
        "",
        reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::drawCatalog),
        reinterpret_cast<void*>(this)
    );

    while (!entriesMenu.isFinished()) {
        entriesMenu.update();
        entriesMenu.draw(canvas);
        queueDraw();
    }
}

void LilCatalogApp::drawEntry() {
    lilka::Menu entryMenu("Вибір дії");

    bool entryInstalled = validateEntry();
    if (entryInstalled) {
        entryMenu.addItem(
            "Запустити",
            0,
            0,
            "",
            reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::executeEntry),
            reinterpret_cast<void*>(this)
        );
        entryMenu.addItem(
            "Видалити",
            0,
            0,
            "",
            reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::removeEntry),
            reinterpret_cast<void*>(this)
        );
        entryMenu.addItem(
            "Оновити",
            0,
            0,
            "",
            reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::fetchEntry),
            reinterpret_cast<void*>(this)
        );
        entryMenu.addItem(
            "Опис",
            0,
            0,
            "",
            reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::drawEntryDescription),
            reinterpret_cast<void*>(this)
        );
    } else {
        entryMenu.addItem(
            "Встановити",
            0,
            0,
            "",
            reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::fetchEntry),
            reinterpret_cast<void*>(this)
        );
    }

    entryMenu.addItem(
        "Назад",
        0,
        0,
        "",
        reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::drawCategory),
        reinterpret_cast<void*>(this)
    );

    while (!entryMenu.isFinished()) {
        entryMenu.update();
        entryMenu.draw(canvas);
        queueDraw();
    }
}

void LilCatalogApp::drawEntryDescription() {
    u8_t categoryIndex = categoriesMenu.getCursor();
    u8_t entryIndex = entriesMenu.getCursor();
    catalog_entry entry = catalog[categoryIndex].entries[entryIndex];

    showAlert(
        String("Назва: ") + entry.name + String("\nАвтор: ") + entry.author + String("\nОпис: ") + entry.description
    );
    drawEntry();
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
    drawCategory();
}

void LilCatalogApp::executeEntry() {
    u8_t categoryIndex = categoriesMenu.getCursor();
    u8_t entryIndex = entriesMenu.getCursor();
    catalog_entry entry = catalog[categoryIndex].entries[entryIndex];
    catalog_entry_file entry_file = entry.files[0];

    lilka::serial_log("Opening path %s", entry_file.target.c_str());

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
    lilka::ProgressDialog dialog("Завантаження", path + "\n\nПочинаємо...");
    dialog.draw(canvas);
    queueDraw();
    int error;
    error = lilka::multiboot.start(path);
    if (error) {
        showAlert(String("Етап: 1\nКод: ") + error);
        return;
    }
    dialog.setMessage(path + "\n\nРозмір: " + String(lilka::multiboot.getBytesTotal()) + " Б");
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
        showAlert(String("Етап: 2\nКод: ") + error);
        return;
    }
    error = lilka::multiboot.finishAndReboot();
    if (error) {
        showAlert(String("Етап: 3\nКод: ") + error);
        return;
    }
}

void LilCatalogApp::showAlert(const String& message) {
    lilka::Alert alert("lilcatalog", message);
    alert.draw(canvas);
    queueDraw();
    while (!alert.isFinished()) {
        alert.update();
    }
    lilka::serial.log(message.c_str());
}

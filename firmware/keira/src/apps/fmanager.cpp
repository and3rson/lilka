#include "fmanager.h"

// DEPS:
#include "appmanager.h"
#include <mbedtls/md5.h>
#include "esp_log.h"
#include "esp_err.h"
#include <errno.h>

// APPS:
#include "modplayer/modplayer.h"
#include "liltracker/liltracker.h"
#include "lua/luarunner.h"
#include "mjs/mjsrunner.h"
#include "nes/nesapp.h"

// ICONS:
#include "lilka/fileutils.h"
#include "icons/normalfile.h"
#include "icons/folder.h"
#include "icons/nes.h"
#include "icons/bin.h"
#include "icons/lua.h"
#include "icons/js.h"
#include "icons/music.h"

void FileManagerApp::alert(const String& title, const String& message) {
    lilka::Alert alert(title, message);
    alert.draw(canvas);
    queueDraw();
    while (!alert.isFinished()) {
        alert.update();
        taskYIELD();
    }
}

void FileManagerApp::alertNotImplemented() {
    alert("Помилка", "Ця функція перебуває в розробці");
}

FileManagerApp::FileManagerApp(FS* fSysDriver, const String& path) : App("FileManager") {
    if (fSysDriver == &SD) {
        currentPath = lilka::fileutils.joinPath(LILKA_SD_ROOT, path);
        menuPrefix = "SD: ";
    } else if (fSysDriver == &SPIFFS) {
        currentPath = lilka::fileutils.joinPath(LILKA_SPIFFS_ROOT, path);
        menuPrefix = "SPIFFS: ";
    } else {
        currentPath = path; // if fSysDriver is zero, we just assume it's a canonical path
        menuPrefix = "R: ";
    }
}

String FileManagerApp::getFileMD5(const String& file_path) {
    FILE* file = fopen(file_path.c_str(), "rb"); // Use c_str() for Arduino String compatibility
    if (!file) {
        lilka::serial_err("MD5: Failed to open file: %s", file_path.c_str());
        return String(); // Return an empty string on failure
    }
    lilka::ProgressDialog dialog("Обчислення МD5", file_path);

    // Get file size
    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    mbedtls_md5_context ctx;
    mbedtls_md5_init(&ctx);
    mbedtls_md5_starts_ret(&ctx);

    unsigned char buffer[MD5_CHUNK_SIZE];

    size_t bytesReadChunk = 0;
    size_t bytesRead = 0;

    while ((bytesReadChunk = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        mbedtls_md5_update_ret(&ctx, buffer, bytesReadChunk);

        bytesRead += bytesReadChunk;
        int progress = bytesRead * 100 / fileSize;
        dialog.setProgress(progress);

        if (lilka::controller.getState().a.justPressed) {
            fclose(file);
            return "Не обчислено";
        }
        //  Draw dialog
        dialog.draw(canvas);
        queueDraw();
    }

    if (ferror(file)) {
        ESP_LOGE("MD5", "Error reading file: %s", file_path.c_str());
        fclose(file);
        mbedtls_md5_free(&ctx);
        return String(); // Return an empty string on error
    }

    fclose(file);

    unsigned char output[16];
    mbedtls_md5_finish_ret(&ctx, output);
    mbedtls_md5_free(&ctx);

    // Convert the MD5 hash to a hexadecimal string
    String md5_hex;
    for (int i = 0; i < 16; i++) {
        md5_hex += String(output[i], HEX);
        if (output[i] < 16) {
            // Add a leading zero for single-digit values
            md5_hex = md5_hex.substring(0, md5_hex.length() - 2) + "0" + md5_hex.substring(md5_hex.length() - 2);
        }
    }
    md5_hex.toUpperCase();
    return md5_hex;
}

FMEntry FileManagerApp::pathToEntry(const String& path) {
    FMEntry newEntry;
    bool statPerformed = false;
    newEntry.path = lilka::fileutils.getParentDirectory(path);
    newEntry.name = basename(path.c_str());

    // Perform stat
    // . dir needs specific way to do that
    if (newEntry.name == "." && stat(newEntry.path.c_str(), &(newEntry.stat)) == 0) {
        statPerformed = true;
    } else {
        if (stat(path.c_str(), &(newEntry.stat)) == 0) statPerformed = true;
    }

    if (statPerformed) {
        if (S_ISDIR(newEntry.stat.st_mode)) {
            newEntry.type = FT_DIR;
            newEntry.icon = FT_DIR_ICON;
            newEntry.color = FT_DIR_COLOR;
            return newEntry;
        }
    } else {
        lilka::serial_err("Can't check stat for %s\n%d: %s", path.c_str(), errno, strerror(errno));
        newEntry.type = FT_NONE;
        return newEntry;
    }
    String lowerCasedPath = path;
    lowerCasedPath.toLowerCase();
    if (lowerCasedPath.endsWith(".rom") || lowerCasedPath.endsWith(".nes")) {
        newEntry.type = FT_NES_ROM;
        newEntry.icon = FT_NES_ICON;
        newEntry.color = FT_NES_ROM_COLOR;
    } else if (lowerCasedPath.endsWith(".bin")) {
        newEntry.type = FT_BIN;
        newEntry.icon = FT_BIN_ICON;
        newEntry.color = FT_BIN_COLOR;
    } else if (lowerCasedPath.endsWith(".lua")) {
        newEntry.type = FT_LUA_SCRIPT;
        newEntry.icon = FT_LUA_SCRIPT_ICON;
        newEntry.color = FT_LUA_SCRIPT_COLOR;
    } else if (lowerCasedPath.endsWith(".js")) {
        newEntry.type = FT_JS_SCRIPT;
        newEntry.icon = FT_JS_SCRIPT_ICON;
        newEntry.color = FT_JS_SCRIPT_COLOR;
    } else if (lowerCasedPath.endsWith(".mod")) {
        newEntry.type = FT_MOD;
        newEntry.icon = FT_MOD_ICON;
        newEntry.color = FT_MOD_COLOR;
    } else if (lowerCasedPath.endsWith(".lt")) {
        newEntry.type = FT_LT;
        newEntry.icon = FT_LT_ICON;
        newEntry.color = FT_LT_COLOR;
    } else {
        newEntry.type = FT_OTHER;
        newEntry.icon = FT_OTHER_ICON;
        newEntry.color = FT_OTHER_COLOR;
    }
    return newEntry;
}

void FileManagerApp::openEntry(const FMEntry& entry) {
    String path = lilka::fileutils.joinPath(entry.path, entry.name);
    lilka::serial_log("Opening path %s", path.c_str());
    switch (entry.type) {
        case FT_NES_ROM:
            AppManager::getInstance()->runApp(new NesApp(path));
            break;
        case FT_BIN:
            loadRom(path);
            break;
        case FT_LUA_SCRIPT:
            AppManager::getInstance()->runApp(new LuaFileRunnerApp(path));
            break;
        case FT_JS_SCRIPT:
            AppManager::getInstance()->runApp(new MJSApp(path));
            break;
        case FT_MOD:
            AppManager::getInstance()->runApp(new ModPlayerApp(path));
            break;
        case FT_LT:
            AppManager::getInstance()->runApp(new LilTrackerApp(path));
            break;
        case FT_DIR:
            readDir(path);
            break;
        case FT_OTHER:
            showEntryInfo(entry);
            break;
    }
    vTaskDelay(SUSPEND_AWAIT_TIME / portTICK_RATE_MS);
}
void FileManagerApp::openEntryWith(const FMEntry& entry) {
    lilka::Menu openWithMenu;
    openWithMenu.setTitle("Оберіть додаток");
    openWithMenu.addActivationButton(lilka::Button::B); // Back Button
    openWithMenu.addItem("Файловий менеджер");
    openWithMenu.addItem("Емулятор NES");
    openWithMenu.addItem("Загрузчик прошивок");
    openWithMenu.addItem("Lua");
    openWithMenu.addItem("mJS");
    openWithMenu.addItem("ЛілТрекер");
    openWithMenu.addItem("Програвач MOD");

    while (1) {
        if (exitChildDialogs) return;
        while (!openWithMenu.isFinished()) {
            openWithMenu.update();
            openWithMenu.draw(canvas);
            queueDraw();
            vTaskDelay(5 / portTICK_PERIOD_MS); // Do not consume all resources
        }
        auto button = openWithMenu.getButton();
        if (button == lilka::Button::B) return; // BACK

        auto index = openWithMenu.getCursor();
        String path = lilka::fileutils.joinPath(entry.path, entry.name);
        if (index == 0) { // FileManager
            readDir(path);
        } else if (index == 1) { // Nes Emulator
            AppManager::getInstance()->runApp(new NesApp(path));
        } else if (index == 2) { // ROM Loader
            loadRom(path);
        } else if (index == 3) { // Lua
            AppManager::getInstance()->runApp(new LuaFileRunnerApp(path));
        } else if (index == 4) { // mJS
            AppManager::getInstance()->runApp(new MJSApp(path));
        } else if (index == 5) { // LilTracker
            AppManager::getInstance()->runApp(new LilTrackerApp(path));
        } else if (index == 6) { // ModPlayer
            AppManager::getInstance()->runApp(new ModPlayerApp(path));
        }
    }
}

void FileManagerApp::showEntryInfo(const FMEntry& entry) {
    String info;
    if (entry.type == FT_DIR) {
        info = "Тип: директорія\n";
    } else {
        info = "Тип: файл\n";
        info += "Розмір: " + lilka::fileutils.getHumanFriendlySize(entry.stat.st_size) + "\n";
        info += "MD5: " + getFileMD5(lilka::fileutils.joinPath(entry.path, entry.name)) + "\n";
    }

    alert(entry.name, info);
}

void FileManagerApp::loadRom(const String& path) {
    lilka::ProgressDialog dialog("Завантаження", path + "\n\nПочинаємо...");
    dialog.draw(canvas);
    queueDraw();
    int error;
    error = lilka::multiboot.start(path);
    if (error) {
        alert("Помилка", String("Етап: 1\nКод: ") + error);
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
        alert("Помилка", String("Етап: 2\nКод: ") + error);
        return;
    }
    error = lilka::multiboot.finishAndReboot();
    if (error) {
        alert("Помилка", String("Етап: 3\nКод: ") + error);
        return;
    }
}

void FileManagerApp::selectPath(const String& filename) {
    selectedPaths.push_back(filename);
    lilka::serial_log("Adding %s to selected paths", filename.c_str());
    mode = FM_MODE_SELECT;
}

void FileManagerApp::deselectPath(const String& filename) {
    for (auto it = selectedPaths.begin(); it != selectedPaths.end();) {
        if (*it == filename) {
            it = selectedPaths.erase(it);
            lilka::serial_log("Removing %s from selected paths", filename.c_str());
            if (selectedPaths.size() == 0) mode = FM_MODE_VIEW; // no selected paths
            return; // Assume we don't have duplicates in vector, cause if we've them [-_-]
        }
    }
}

bool FileManagerApp::isSelectedPath(const String& filename) {
    for (auto selPath : selectedPaths) {
        if (selPath == filename) return true;
    }
    return false;
}

void FileManagerApp::showEntryOptions(const FMEntry& entry) {
    lilka::Menu fileOptionsMenu;
    fileOptionsMenu.setTitle("Опції");
    fileOptionsMenu.addActivationButton(lilka::Button::B); // Back Button
    // TODO : add callback based api to lilka:Menu
    fileOptionsMenu.addItem("Відкрити");
    fileOptionsMenu.addItem("Відкрити з");
    fileOptionsMenu.addItem("Перейменувати");
    fileOptionsMenu.addItem("Копіювати");
    fileOptionsMenu.addItem("Перемістити");
    fileOptionsMenu.addItem("Видалити");
    fileOptionsMenu.addItem("Вибрати");
    fileOptionsMenu.addItem("Створити папку");
    while (1) {
        if (exitChildDialogs) return;
        while (!fileOptionsMenu.isFinished()) {
            fileOptionsMenu.update();
            fileOptionsMenu.draw(canvas);
            queueDraw();
            vTaskDelay(5 / portTICK_PERIOD_MS); // Do not consume all resources
        }
        auto button = fileOptionsMenu.getButton();
        if (button == lilka::Button::B) return; // BACK

        auto index = fileOptionsMenu.getCursor();
        if (index == 0) { // Open
            if (entry.name == ".") alert("Помилка", "Колумбе, Америку вже відкрито!");
            else openEntry(entry);
        } else if (index == 1) { // OpenWith
            if (entry.name == ".") alert("Помилка", "Колумбе, Америку вже відкрито!");
            else openEntryWith(entry);
        } else if (index == 2) { // Rename
            if (entry.name == ".") alert("Помилка", "Неможливо перейменувати <нічого>");
            else renameEntry(entry);
        } else if (index == 3) { // Copy
            if (entry.name == ".") alert("Помилка", "Неможливо копіювати <нічого>");
            else alertNotImplemented();
        } else if (index == 4) { // Move
            if (entry.name == ".") alert("Помилка", "Переміщати <нічого> дуже важка робота");
            else alertNotImplemented();
        } else if (index == 5) { // Delete
            if (entry.name == ".") alert("Помилка", "Неможливо видалити <нічого>");
            else deleteEntry(entry);
        } else if (index == 6) { // Select
            if (entry.name == ".") alert("Помилка", "Неможливо вибрати <нічого>");
            else alertNotImplemented();
        } else if (index == 7) { // mkdir
            makeDir(entry.path);
            return;
        }
    }
}
// ReadDir -> Detect Directories -> Sort names -> Draw
// On change dir ReadDir again
// Save top dir? just open new app
void FileManagerApp::readDir(const String& path) {
    std::vector<FMEntry> dirContents;

    lilka::Menu fileListMenu;
    fileListMenu.setTitle(menuPrefix + lilka::fileutils.getLocalPathInfo(path).path);
    fileListMenu.addActivationButton(lilka::Button::C); // Info Button
    fileListMenu.addActivationButton(lilka::Button::B); // Back Button
    fileListMenu.addActivationButton(lilka::Button::D); // Options Button

    lilka::serial_log("Trying to load dir %s", path.c_str());

    int16_t index = 0;
    while (1) {
        // restore showEntryOptions and openEntryWith avalibility
        exitChildDialogs = false;
        // Readdir
        auto dir = opendir(path.c_str());
        if (dir == NULL) { // Can't open dir
            alert("Помилка читання", String(errno) + ": " + strerror(errno));
            return;
        }
        currentPath = path; // change path
        const struct dirent* dir_entry = NULL;

        while ((dir_entry = readdir(dir)) != NULL) {
            String filename = dir_entry->d_name;
            // Skip current directory and top level entries
            if (filename != "." && filename != "..") {
                FMEntry newEntry = pathToEntry(lilka::fileutils.joinPath(currentPath, filename));
                dirContents.push_back(newEntry);
                lilka::serial_log(
                    "Added new entry with type:%d, name:%s, path:%s, ",
                    newEntry.type,
                    newEntry.name.c_str(),
                    newEntry.path.c_str()
                );
            }
        }
        closedir(dir); // unfortunately we can't reuse same dir

        // Sorting directory entries
        std::sort(dirContents.begin(), dirContents.end(), [](FMEntry a, FMEntry b) {
            if (a.type == FT_DIR && b.type != FT_DIR) return true;
            else if (a.type != FT_DIR && b.type == FT_DIR) return false;
            return a.name.compareTo(b.name) < 0;
        });

        // Adding entries to menu
        for (auto dirEntry : dirContents) {
            // TODO: check mode, select proper icon if FM_MODE_SELECT and dirEntry in selectedEbtries
            if (dirEntry.type == FT_DIR) fileListMenu.addItem(dirEntry.name, dirEntry.icon, dirEntry.color);
            else
                fileListMenu.addItem(
                    dirEntry.name,
                    dirEntry.icon,
                    dirEntry.color,
                    lilka::fileutils.getHumanFriendlySize(dirEntry.stat.st_size)
                );
        }

        // Add Back button
        fileListMenu.addItem("<< Назад", 0, 0);

        // Try to restore old menuCursor:
        // last option should be selected cause something should be deleted
        // if this happens. here we assume that we deleted single item
        if (fileListMenu.getItemCount() >= index) fileListMenu.setCursor(index);
        else fileListMenu.setCursor(fileListMenu.getItemCount() - 1); // Select last

        // Do Draw !
        while (!fileListMenu.isFinished()) {
            fileListMenu.update();
            fileListMenu.draw(canvas);
            queueDraw();
            vTaskDelay(5 / portTICK_PERIOD_MS); // Do not consume all resources
        }

        // Set selected entry to . at first
        FMEntry selectedEntry; // pass . as entry by default
        selectedEntry = pathToEntry(lilka::fileutils.joinPath(currentPath, "."));

        // try to save selected entry
        index = fileListMenu.getCursor();
        auto button = fileListMenu.getButton();

        FMEntry* pselectedEntry = NULL;
        if (index != dirContents.size()) {
            selectedEntry = dirContents[index];
            pselectedEntry = &selectedEntry;
        }

        // clear memory
        dirContents.clear();
        fileListMenu.clearItems();

        bool exiting = false;

        if (button == lilka::Button::A) { // Open
            if (pselectedEntry) openEntry(*pselectedEntry);
            else exiting = true;
        } else if (button == lilka::Button::C) { // Info
            if (pselectedEntry) showEntryInfo(*pselectedEntry);
            else exiting = true;
        } else if (button == lilka::Button::D) { // Options
            // if folder has no entries
            // pass . entry to allow mkdir
            if (pselectedEntry) showEntryOptions(*pselectedEntry);
            else showEntryOptions(selectedEntry);
        } else { // B button
            exiting = true;
        }

        // Do some work on exit
        if (exiting) {
            // restore parent dir before exit
            currentPath = lilka::fileutils.getParentDirectory(currentPath);
            break;
        }
    }
}

void FileManagerApp::renameEntry(const FMEntry& entry) {
    exitChildDialogs = true;
    lilka::InputDialog newNameInput("Введіть нову назву");
    newNameInput.setValue(entry.name); // pass old name

    while (!newNameInput.isFinished()) {
        newNameInput.update();
        newNameInput.draw(canvas);
        queueDraw();
    }
    auto newName = newNameInput.getValue();

    // Check name
    // TODO: check on invalid characters in user's input
    if (newName == "" || newName == entry.name) return;
    auto path = lilka::fileutils.joinPath(entry.path, entry.name);
    auto newPath = lilka::fileutils.joinPath(entry.path, newName);
    // Perform rename
    if (rename(path.c_str(), newPath.c_str()) != 0) {
        // Handle errors:
        lilka::serial_err("Can't rename %s to %s. %d: %s", path.c_str(), newPath.c_str(), errno, strerror(errno));
        alert("Помилка", String("Не можу перейменувати\n") + path);
    }
}

void FileManagerApp::deleteEntry(const FMEntry& entry, bool force) {
    exitChildDialogs = true;
    auto path = lilka::fileutils.joinPath(entry.path, entry.name);
    // Perform check on user sureness
    if (!force) {
        lilka::Alert checkAlert(
            "Ви впевнені?", String("Ця операція видалить файл\n") + path + "\nПродовжити: START\nВихід: A/B"
        );
        checkAlert.addActivationButton(lilka::Button::A);
        checkAlert.addActivationButton(lilka::Button::B);
        checkAlert.addActivationButton(lilka::Button::START);
        while (!checkAlert.isFinished()) {
            checkAlert.update();
            checkAlert.draw(canvas);
            queueDraw();
            vTaskDelay(5 / portTICK_PERIOD_MS); // Do not consume all resources
        }
        if (checkAlert.getButton() != lilka::Button::START) return; //Exit
    }
    // Do job
    if (entry.type == FT_DIR) { // Directory
        auto dir = opendir(path.c_str());
        if (dir == NULL) {
            lilka::serial_err("Can't open dir %s. %d: %s", path.c_str(), errno, strerror(errno));
            alert("Помилка", String("Не можу видалити\n") + path);
            return; // some shit happened. run!
        }
        struct dirent* dirEntry;
        while ((dirEntry = readdir(dir)) != NULL) {
            String filename = dirEntry->d_name;
            FMEntry fEntry = pathToEntry(lilka::fileutils.joinPath(path, filename));
            deleteEntry(fEntry, true); // recursive
        }
        closedir(dir);
        // Delete dir itself
        if (unlink(path.c_str()) != 0) {
            lilka::serial_err("Tried to delete %s. %d: %s", path.c_str(), errno, strerror(errno));
            alert("Помилка", String("Не можу видалити\n") + path);
            return; // some shit happened. run!
        }

    } else { // Regular file
        if (unlink(path.c_str()) != 0) {
            lilka::serial_err("Tried to delete %s. %d: %s", path.c_str(), errno, strerror(errno));
            alert("Помилка", String("Не можу видалити\n") + path);
            return; // some shit happened. run!
        }
    }
}

void FileManagerApp::makeDir(const String& path) {
    exitChildDialogs = true;
    lilka::InputDialog dirNameInput("Введіть назву нової папки");
    while (!dirNameInput.isFinished()) {
        dirNameInput.update();
        dirNameInput.draw(canvas);
        queueDraw();
    }
    auto dirName = dirNameInput.getValue();
    if (dirName != "") {
        if (mkdir(lilka::fileutils.joinPath(path, dirName).c_str(), 0777) != 0) {
            alert("Помилка", String("Не можу створити папку") + dirName);
            lilka::serial_err(
                "Can't make dir in %s with name %s. %d: %s", path.c_str(), dirName.c_str(), errno, strerror(errno)
            );
        }
    }
}
void FileManagerApp::run() {
    lilka::serial_log("Opening path %s", currentPath.c_str());
    readDir(currentPath);
    // uint32_t maxReachedStack = uxTaskGetStackHighWaterMark(NULL);
    // lilka::serial_log("exiting fmanager. reached stacksize %u", maxReachedStack);
}

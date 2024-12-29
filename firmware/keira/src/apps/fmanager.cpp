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

FileManagerApp::FileManagerApp(const String& path) :
    App("FileManager"),
    copyProgress("Копіювання...", ""),
    md5Progress("Обчислення МD5", ""),
    mkdirInput("Введіть назву нової папки"),
    renameInput("Введіть нову назву") {
    // Set stack size
    setStackSize(FM_STACK_SIZE);
    // init once
    fileOptionsMenu.setTitle("Опції");
    fileOptionsMenu.addActivationButton(lilka::Button::B); // Back Button
    fileOptionsMenu.addItem("Відкрити");
    fileOptionsMenu.addItem("Відкрити з");
    fileOptionsMenu.addItem("Перейменувати");
    fileOptionsMenu.addItem("Копіювати");
    fileOptionsMenu.addItem("Перемістити");
    fileOptionsMenu.addItem("Видалити");
    fileOptionsMenu.addItem("Вибрати");
    fileOptionsMenu.addItem("Створити папку");
    // init once
    fileOpenWithMenu.setTitle("Оберіть додаток");
    fileOpenWithMenu.addActivationButton(lilka::Button::B); // Back Button
    fileOpenWithMenu.addItem("Файловий менеджер");
    fileOpenWithMenu.addItem("Емулятор NES");
    fileOpenWithMenu.addItem("Загрузчик прошивок");
    fileOpenWithMenu.addItem("Lua");
    fileOpenWithMenu.addItem("mJS");
    fileOpenWithMenu.addItem("ЛілТрекер");
    fileOpenWithMenu.addItem("Програвач MOD");

    // Info Button
    fileListMenu.addActivationButton(lilka::Button::C);
    // Back Button
    fileListMenu.addActivationButton(lilka::Button::B);
    // Options Button
    fileListMenu.addActivationButton(lilka::Button::D);
    // in FM_MODE_COPY_SINGLE or FM_MODE_MOVE_SINGLE -> Paste
    // in FM_MODE_VIEW(default) dir Reload
    fileListMenu.addActivationButton(lilka::Button::START);

    currentPath = path;
}

String FileManagerApp::getFileMD5(const String& file_path) {
    FILE* file = fopen(file_path.c_str(), "rb"); // Use c_str() for Arduino String compatibility
    if (!file) {
        FM_DBG lilka::serial_err("MD5: Failed to open file: %s", file_path.c_str());
        return String(); // Return an empty string on failure
    }
    md5Progress.setMessage(basename(file_path.c_str()));
    // Get file size
    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    mbedtls_md5_context ctx;
    mbedtls_md5_init(&ctx);
    mbedtls_md5_starts_ret(&ctx);
    progress = 0;
    bytesRead = 0;
    bytesReadChunk = 0;

    while ((bytesReadChunk = fread(buffer, 1, FM_CHUNK_SIZE, file)) > 0) {
        mbedtls_md5_update_ret(&ctx, buffer, bytesReadChunk);

        bytesRead += bytesReadChunk;
        progress = bytesRead * 100 / fileSize;
        md5Progress.setProgress(progress);

        if (lilka::controller.getState().a.justPressed) {
            fclose(file);
            return "Не обчислено";
        }
        //  Draw dialog
        md5Progress.draw(canvas);
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
        FM_DBG lilka::serial_err("Can't check stat for %s\n%d: %s", path.c_str(), errno, strerror(errno));
        newEntry.type = FT_NONE;
        newEntry.icon = FT_NONE_ICON;
        newEntry.color = FT_NONE_COLOR;
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

void FileManagerApp::openFileEntry(const FMEntry& entry) {
    String path = lilka::fileutils.joinPath(entry.path, entry.name);
    FM_DBG lilka::serial_log("Opening path %s", path.c_str());
    switch (entry.type) {
        case FT_NES_ROM:
            AppManager::getInstance()->runApp(new NesApp(path));
            break;
        case FT_BIN:
            fileLoadAsRom(path);
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
            fileListMenuShow(path);
            break;
        case FT_OTHER:
            fileInfoShowAlert(entry);
            break;
    }
    vTaskDelay(SUSPEND_AWAIT_TIME / portTICK_RATE_MS);
}
void FileManagerApp::fileOpenWithMenuShow(const FMEntry& entry) {
    while (1) {
        if (exitChildDialogs) return;
        while (!fileOpenWithMenu.isFinished()) {
            fileOpenWithMenu.update();
            fileOpenWithMenu.draw(canvas);
            queueDraw();
        }
        auto button = fileOpenWithMenu.getButton();
        if (button == lilka::Button::B) return; // BACK

        auto index = fileOpenWithMenu.getCursor();

        String path = lilka::fileutils.joinPath(entry.path, entry.name);
        if (index == 0) { // FileManager
            fileListMenuShow(path);
        } else if (index == 1) { // Nes Emulator
            AppManager::getInstance()->runApp(new NesApp(path));
        } else if (index == 2) { // ROM Loader
            fileLoadAsRom(path);
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

void FileManagerApp::fileInfoShowAlert(const FMEntry& entry) {
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

void FileManagerApp::fileLoadAsRom(const String& path) {
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
        progress = lilka::multiboot.getBytesWritten() * 100 / lilka::multiboot.getBytesTotal();
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

void FileManagerApp::fileOptionsMenuShow(const FMEntry& entry) {
    while (1) {
        if (exitChildDialogs) return;
        while (!fileOptionsMenu.isFinished()) {
            fileOptionsMenu.update();
            fileOptionsMenu.draw(canvas);
            queueDraw();
        }
        auto button = fileOptionsMenu.getButton();
        if (button == lilka::Button::B) return; // BACK

        auto index = fileOptionsMenu.getCursor();
        if (index == 0) { // Open
            if (entry.name == ".") alert("Помилка", "Колумбе, Америку вже відкрито!");
            else openFileEntry(entry);
        } else if (index == 1) { // OpenWith
            if (entry.name == ".") alert("Помилка", "Колумбе, Америку вже відкрито!");
            else fileOpenWithMenuShow(entry);
        } else if (index == 2) { // Rename
            if (entry.name == ".") alert("Помилка", "Неможливо перейменувати <нічого>");
            else renameInputShow(entry);
        } else if (index == 3) { // Copy
            if (entry.name == ".") alert("Помилка", "Неможливо копіювати <нічого>");
            else {
                if (mode == FM_MODE_VIEW || mode == FM_MODE_MOVE_SINGLE || mode == FM_MODE_COPY_SINGLE) {
                    copySingleEntry(entry);
                }
            }
        } else if (index == 4) { // Move
            if (entry.name == ".") alert("Помилка", "Переміщати <нічого> дуже важка робота");
            else {
                if (mode == FM_MODE_VIEW || mode == FM_MODE_MOVE_SINGLE || mode == FM_MODE_COPY_SINGLE) {
                    moveSingleEntry(entry);
                }
            }
        } else if (index == 5) { // Delete
            if (entry.name == ".") alert("Помилка", "Неможливо видалити <нічого>");
            else {
                deleteEntry(entry);
                return;
            }
        } else if (index == 6) { // Select
            if (entry.name == ".") alert("Помилка", "Неможливо вибрати <нічого>");
            else alertNotImplemented();
        } else if (index == 7) { // mkdir
            mkdirInputShow(entry.path);
            return;
        }
    }
}
bool FileManagerApp::fileListMenuLoadDir(const String& path) {
    auto dir = opendir(path.c_str());
    if (dir == NULL) { // Can't open dir
        alert("Помилка читання", String(errno) + ": " + strerror(errno));
        return false;
    }

    currentPath = path; // change path
    fileListMenu.setTitle(path);

    const struct dirent* dir_entry = NULL;

    while ((dir_entry = readdir(dir)) != NULL) {
        String filename = dir_entry->d_name;
        // Skip current directory and top level entries
        if (filename != "." && filename != "..") {
            FMEntry newEntry = pathToEntry(lilka::fileutils.joinPath(currentPath, filename));
            currentDirEntries.push_back(newEntry);
            FM_DBG lilka::serial_log(
                "Added new entry with type:%d, name:%s, path:%s, ",
                newEntry.type,
                newEntry.name.c_str(),
                newEntry.path.c_str()
            );
        }
    }
    closedir(dir); // unfortunately we can't reuse same dir

    // Sorting directory entries
    std::sort(currentDirEntries.begin(), currentDirEntries.end(), [](FMEntry a, FMEntry b) {
        if (a.type == FT_DIR && b.type != FT_DIR) return true;
        else if (a.type != FT_DIR && b.type == FT_DIR) return false;
        return a.name.compareTo(b.name) < 0;
    });

    // Adding entries to menu
    for (auto dirEntry : currentDirEntries) {
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
    return true;
}

void FileManagerApp::fileListMenuShow(const String& path) {
    FM_DBG lilka::serial_log("Trying to load dir %s", path.c_str());
    int16_t index = 0;
    while (1) {
        exitChildDialogs = false;

        if (!stackSizeCheck()) return;

        if (!fileListMenuLoadDir(path)) return;

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
        }
        // Set selected entry to . at first
        FMEntry currentEntry; // pass . as entry by default
        currentEntry = pathToEntry(lilka::fileutils.joinPath(currentPath, "."));

        // try to save current cursor position
        index = fileListMenu.getCursor();
        auto button = fileListMenu.getButton();

        const FMEntry* pCurrentEntry = NULL;
        if (index != currentDirEntries.size()) {
            currentEntry = currentDirEntries[index];
            pCurrentEntry = &currentEntry;
        }

        // clear memory
        currentDirEntries.clear();
        fileListMenu.clearItems();

        bool exiting = false;
        if (button == lilka::Button::START) { // Paste/Reload
            if (mode == FM_MODE_COPY_SINGLE || mode == FM_MODE_MOVE_SINGLE) {
                pasteSingleEntry(singleMoveCopyEntry, currentPath);
                changeMode(FM_MODE_VIEW);
                continue; // Reload anyways
            } else {
                MAKE_SANDWICH("Список файлів оновлено");
                FM_DBG lilka::serial_log("Force dir reload");
                continue; // Reload dir
            }
        }

        if (button == lilka::Button::A) { // Open
            if (pCurrentEntry) openFileEntry(*pCurrentEntry);
            else exiting = true;
        } else if (button == lilka::Button::C) { // Info
            if (pCurrentEntry) fileInfoShowAlert(*pCurrentEntry);
            else exiting = true;
        } else if (button == lilka::Button::D) { // Options
            // if folder has no entries
            // pass . entry to allow mkdir
            if (pCurrentEntry) fileOptionsMenuShow(*pCurrentEntry);
            else fileOptionsMenuShow(currentEntry);
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

void FileManagerApp::renameInputShow(const FMEntry& entry) {
    exitChildDialogs = true;

    renameInput.setValue(entry.name); // pass old name
    while (!renameInput.isFinished()) {
        renameInput.update();
        renameInput.draw(canvas);
        queueDraw();
    }

    auto newName = renameInput.getValue();

    // Skip empty and same names
    if (newName == "" || newName == entry.name) return;

    auto path = lilka::fileutils.joinPath(entry.path, entry.name);
    auto newPath = lilka::fileutils.joinPath(entry.path, newName);

    // Try rename
    if (rename(path.c_str(), newPath.c_str()) != 0) {
        FM_DBG lilka::serial_err(
            "Can't rename %s to %s. %d: %s", path.c_str(), newPath.c_str(), errno, strerror(errno)
        );
        alert("Помилка", String("Не можу перейменувати\n") + path);
    }
}

void FileManagerApp::deleteEntry(const FMEntry& entry, bool force) {
    if (exitChildDialogs) return;
    // could also fall here. exit gracefully
    if (!stackSizeCheck()) return;

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
        }
        if (checkAlert.getButton() != lilka::Button::START) return; //Exit
    }

    // Do job
    if (entry.type == FT_DIR) { // Directory
        auto dir = opendir(path.c_str());
        if (dir == NULL) {
            FM_DBG lilka::serial_err("Can't open dir %s. %d: %s", path.c_str(), errno, strerror(errno));
            alert("Помилка", String("Не можу видалити\n") + path);
            return; // some shit happened. run!
        }
        const struct dirent* dirEntry;
        while ((dirEntry = readdir(dir)) != NULL) {
            String filename = dirEntry->d_name;
            FMEntry fEntry = pathToEntry(lilka::fileutils.joinPath(path, filename));
            deleteEntry(fEntry, true); // recursive
        }
        closedir(dir);
        // Delete dir itself
        if (unlink(path.c_str()) != 0) {
            FM_DBG lilka::serial_err("Tried to delete %s. %d: %s", path.c_str(), errno, strerror(errno));
            alert("Помилка", String("Не можу видалити\n") + path);
            return; // some shit happened. run!
        }

    } else { // Regular file
        if (unlink(path.c_str()) != 0) {
            FM_DBG lilka::serial_err("Tried to delete %s. %d: %s", path.c_str(), errno, strerror(errno));
            alert("Помилка", String("Не можу видалити\n") + path);
            return; // some shit happened. run!
        }
    }
}

bool FileManagerApp::changeMode(FmMode newMode) {
    exitChildDialogs = true;
    FM_DBG lilka::serial_log("Trying to enter mode %d", newMode);
    mode = newMode;
    return true;
}

void FileManagerApp::copySingleEntry(const FMEntry& entry) {
    singleMoveCopyEntry = entry;
    // Change mode
    if ((!changeMode(FM_MODE_COPY_SINGLE)) || entry.name == ".") {
        alert("Помилка", "Не можу скопіювати файл");
        FM_DBG lilka::serial_log("Can't enter mode FM_MODE_COPY_SINGLE");
        return;
    };
    MAKE_SANDWICH("Файл додано в буфер обміну");
}

void FileManagerApp::moveSingleEntry(const FMEntry& entry) {
    singleMoveCopyEntry = entry;
    // Change mode
    if ((!changeMode(FM_MODE_MOVE_SINGLE)) || entry.name == ".") {
        alert("Помилка", "Не можу перемістити файл");
        FM_DBG lilka::serial_log("Can't enter mode FM_MODE_MOVE_SINGLE");
        return;
    };
    MAKE_SANDWICH("Файл додано в буфер обміну");
}

bool FileManagerApp::copyPath(const String& source, const String& destination) {
    // Cause copying of large directories with long list of files made via recursion,
    // we could easily fail here
    if (!stackSizeCheck()) return false;

    if (destination.indexOf(source) == 0) {
        alert("Помилка", "Не можу скопіювати файл");
        exitChildDialogs = true;
        return false;
    }
    // Exit, could happen if no stack left
    if (exitChildDialogs) return false;

    copyProgress.setMessage(basename(source.c_str()));
    copyProgress.setProgress(0);

    FM_DBG lilka::serial_log("Reached stacksize %u", uxTaskGetStackHighWaterMark(NULL));

    struct stat entryStat;
    // Get info about the source
    if (stat(source.c_str(), &entryStat) < 0) {
        FM_DBG lilka::serial_log("Error stating source: %s", source.c_str());
        return false;
    }

    if (S_ISREG(entryStat.st_mode)) {
        // Source is a file, copy it
        auto inFile = fopen(source.c_str(), "r");
        if (!inFile) {
            FM_DBG lilka::serial_log("Error opening source file: %s", source.c_str());
            return false;
        }

        fseek(inFile, 0, SEEK_END);
        auto fileSize = ftell(inFile);
        fseek(inFile, 0, SEEK_SET);

        auto outFile = fopen(destination.c_str(), "w");
        if (!outFile) {
            FM_DBG lilka::serial_log("Error opening destination file: %s", destination.c_str());
            return false;
        }

        progress = 0;
        bytesRead = 0;
        bytesReadChunk = 0;

        while ((bytesReadChunk = fread(buffer, 1, FM_CHUNK_SIZE, inFile)) > 0) {
            if (fwrite(buffer, 1, bytesReadChunk, outFile) != bytesReadChunk) {
                FM_DBG lilka::serial_log("Error writing to file: %s", destination.c_str());
                fclose(inFile);
                fclose(outFile);
                return false;
            }
            bytesRead += bytesReadChunk;

            progress = bytesRead * 100 / fileSize;
            copyProgress.setProgress(progress);

            if (lilka::controller.getState().a.justPressed) {
                fclose(inFile);
                fclose(outFile);
                exitChildDialogs = true;
                return false;
            }
            //  Draw dialog
            copyProgress.draw(canvas);
            queueDraw();
        }

        fclose(inFile);
        fclose(outFile);

        return true;
    } else if (S_ISDIR(entryStat.st_mode)) {
        // Source is a directory, create destination directory
        if (mkdir(destination.c_str(), FM_MKDIR_MODE) < 0) {
            FM_DBG lilka::serial_log("Error creating directory: %s", destination);
            return false;
        }

        // Open the source directory
        auto dir = opendir(source.c_str());
        if (dir == NULL) {
            FM_DBG lilka::serial_log("Error opening directory: %s", source);
            return false;
        }

        const struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            String entryName = entry->d_name;

            // Skip `.` and `..`
            if (entryName == "." || entryName == "..") {
                continue;
            }

            // Recursively copy each entry
            auto srcPath = lilka::fileutils.joinPath(source, entryName);
            auto destPath = lilka::fileutils.joinPath(destination, entryName);
            if (!copyPath(srcPath, destPath)) {
                closedir(dir);
                return false;
            }
        }

        closedir(dir);
        return true;
    }

    FM_DBG lilka::serial_log("Unknown file type: %s", source);
    return false;
}

void FileManagerApp::pasteSingleEntry(const FMEntry& entry, String& where) {
    if (mode == FM_MODE_COPY_SINGLE) { // COPY
        auto oldPath = lilka::fileutils.joinPath(entry.path, entry.name);
        auto newPath = lilka::fileutils.joinPath(where, entry.name);
        if (oldPath == newPath) {
            alert("Помилка", "Цей файл/директорія вже існує!");
            return;
        }
        if (copyPath(oldPath, newPath)) {
            MAKE_SANDWICH("Скопійовано!");
        } else {
            if (!exitChildDialogs) alert("Помилка", String(errno) + ": " + strerror(errno));
        }
    } else if (mode == FM_MODE_MOVE_SINGLE) { // MOVE
        auto oldPath = lilka::fileutils.joinPath(entry.path, entry.name);
        auto newPath = lilka::fileutils.joinPath(where, entry.name);
        if (newPath.indexOf(oldPath) == 0) {
            // TODO : place in a single place
            // checks should be same for both
            // move and copy
            alert("Помилка", "Не можу перемістити файл");
        }
        // function should work if src and dst are located on a single filesystem
        if (rename(oldPath.c_str(), newPath.c_str()) == 0) {
            MAKE_SANDWICH("Переміщено!");
        } else {
            FM_DBG lilka::serial_err(
                "Can't rename %s to %s. %d: %s", oldPath.c_str(), newPath.c_str(), errno, strerror(errno)
            );
        }
    } else { // SHIT!
        FM_DBG lilka::serial_err("This should never happen. FM Mode works wrong way");
        // try to restore
        changeMode(FM_MODE_VIEW);
        exitChildDialogs = true;
    }
}

void FileManagerApp::mkdirInputShow(const String& path) {
    exitChildDialogs = true;
    while (!mkdirInput.isFinished()) {
        mkdirInput.update();
        mkdirInput.draw(canvas);
        queueDraw();
    }
    auto dirName = mkdirInput.getValue();
    if (dirName != "") {
        if (mkdir(lilka::fileutils.joinPath(path, dirName).c_str(), FM_MKDIR_MODE) != 0) {
            alert("Помилка", String("Не можу створити папку") + dirName);
            FM_DBG lilka::serial_err(
                "Can't make dir in %s with name %s. %d: %s", path.c_str(), dirName.c_str(), errno, strerror(errno)
            );
        }
    }
}

bool FileManagerApp::stackSizeCheck() {
    if (uxTaskGetStackHighWaterMark(NULL) < FM_STACK_MIN_FREE_SIZE) {
        alert("Помилка", "Недостатньо пам'яті аби завершити операіцю");
        FM_DBG lilka::serial_log("Stack is too small");
        exitChildDialogs = true;
        return false;
    }
    return true;
}
void FileManagerApp::run() {
    FM_DBG lilka::serial_log("Opening path %s", currentPath.c_str());
    fileListMenuShow(currentPath);
}

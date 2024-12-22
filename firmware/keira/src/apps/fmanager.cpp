#include "fmanager.h"

// DEPS:
#include "appmanager.h"
#include <mbedtls/md5.h>
#include "esp_log.h"
#include "esp_err.h"

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

#define MD5_CHUNK_SIZE 1024
void FileManagerApp::alert(const String& title, const String& message) {
    lilka::Alert alert(title, message);
    alert.draw(canvas);
    queueDraw();
    while (!alert.isFinished()) {
        alert.update();
        taskYIELD();
    }
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

FileType FileManagerApp::detectFileType(const String& filename) {
    String lowerCasedFileName = filename;
    lowerCasedFileName.toLowerCase();
    // Mime-type detection?
    if (lowerCasedFileName.endsWith(".rom") || lowerCasedFileName.endsWith(".nes")) {
        return FT_NES_ROM;
    } else if (lowerCasedFileName.endsWith(".bin")) {
        return FT_BIN;
    } else if (lowerCasedFileName.endsWith(".lua")) {
        return FT_LUA_SCRIPT;
    } else if (lowerCasedFileName.endsWith(".js")) {
        return FT_JS_SCRIPT;
    } else if (lowerCasedFileName.endsWith(".mod")) {
        return FT_MOD;
    } else if (lowerCasedFileName.endsWith(".lt")) {
        return FT_LT;
    } else {
        return FT_OTHER;
    }
}

const menu_icon_t* FileManagerApp::getFileIcon(const String& filename) {
    auto fType = detectFileType(filename);
    switch (fType) {
        case FT_NES_ROM:
            return &nes_img;
        case FT_BIN:
            return &bin_img;
        case FT_LUA_SCRIPT:
            return &lua_img;
        case FT_JS_SCRIPT:
            return &js_img;
        case FT_MOD:
            return &music_img;
        case FT_LT: // TODO: add separate icon for FT_MOD or for FT_LT
            return &music_img;
        default:
            return &normalfile_img;
    }
}

const uint16_t FileManagerApp::getFileColor(const String& filename) {
    auto fType = detectFileType(filename);
    switch (fType) {
        case FT_NES_ROM:
            return lilka::colors::Candy_pink;
        case FT_BIN:
            return lilka::colors::Mint_green;
        case FT_LUA_SCRIPT:
            return lilka::colors::Maya_blue;
        case FT_JS_SCRIPT:
            return lilka::colors::Butterscotch;
        case FT_MOD:
            return lilka::colors::Plum_web;
        case FT_LT:
            return lilka::colors::Pink_lace;
        default:
            return lilka::colors::Light_gray;
    }
}

void FileManagerApp::openFile(const String& path) {
    auto fType = detectFileType(path);
    switch (fType) {
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
        case FT_OTHER:
            showFileInfo(path);
            break;
    }
}
void FileManagerApp::showFileInfo(const String& path) {
    String info;
    struct stat fileStat;
    if (stat(path.c_str(), &fileStat) != 0) {
        lilka::serial_log("showFileInfo failed. No stat data for %s", path.c_str());
        return;
    }
    if (S_ISDIR(fileStat.st_mode)) {
        info = "Тип: директорія\n";
    } else {
        info = "Тип: файл\n";
        info += "Розмір: " + lilka::fileutils.getHumanFriendlySize(fileStat.st_size) + "\n";
        info += "MD5: " + getFileMD5(path) + "\n";
    }

    alert(basename(path.c_str()), info);
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

// ReadDir -> Detect Directories -> Sort names -> Draw
// On change dir ReadDir again
// Save top dir? just open new app

void FileManagerApp::readDir(const String& path) {
    std::vector<FMEntry> dirContents;

    lilka::Menu fileListMenu;
    fileListMenu.setTitle(menuPrefix + lilka::fileutils.getLocalPathInfo(path).path);
    fileListMenu.addActivationButton(lilka::Button::C); // Info Button
    fileListMenu.addActivationButton(lilka::Button::B); // Back Button

    lilka::serial_log("Trying to load dir %s", path.c_str());

    auto dir = opendir(path.c_str());
    if (dir == NULL) return; // Can't open dir
    currentPath = path; // change path
    const struct dirent* dir_entry = NULL;

    // Readdir
    while ((dir_entry = readdir(dir)) != NULL) {
        String filename = dir_entry->d_name;
        // Skip current directory and top level entries
        if (filename != "." || filename == "..") {
            struct stat fileStat;
            String sizeStr;
            // Geting filesize and filetype
            if (stat(lilka::fileutils.joinPath(currentPath, filename).c_str(), &fileStat) != 0) sizeStr = "!!!";
            else sizeStr = fileStat.st_size;
            const menu_icon_t* icon;
            uint16_t color;
            if (S_ISDIR(fileStat.st_mode)) {
                icon = &folder_img;
                color = lilka::colors::Arylide_yellow;
                sizeStr = "";
            } else {
                icon = getFileIcon(filename);
                color = getFileColor(filename);
            }
            dirContents.push_back({detectFileType(filename), icon, color, filename, fileStat});
        }
    }

    // Sorting directory entries
    std::sort(dirContents.begin(), dirContents.end(), [](FMEntry a, FMEntry b) {
        if (S_ISDIR(a.stat.st_mode) && !S_ISDIR(b.stat.st_mode)) return true;
        else if (!S_ISDIR(a.stat.st_mode) && S_ISDIR(b.stat.st_mode)) return false;
        return a.name.compareTo(b.name) < 0;
    });

    // Adding entries to menu
    for (auto dirEntry : dirContents) {
        if (S_ISDIR(dirEntry.stat.st_mode)) fileListMenu.addItem(dirEntry.name, dirEntry.icon, dirEntry.color);
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

    while (1) {
        // Do Draw !
        while (!fileListMenu.isFinished()) {
            fileListMenu.update();
            fileListMenu.draw(canvas);
            queueDraw();
        }

        //Do magic!
        int16_t index = fileListMenu.getCursor();

        if (index == dirContents.size()) break; // Back option selected

        auto button = fileListMenu.getButton();
        if (button == lilka::Button::A) {
            if (S_ISDIR(dirContents[index].stat.st_mode)) {
                readDir(lilka::fileutils.joinPath(currentPath, dirContents[index].name));
                // Restore old path
                currentPath = lilka::fileutils.getParentDirectory(currentPath);
                lilka::serial_log("Restored path %s", currentPath.c_str());
            } else {
                openFile(lilka::fileutils.joinPath(currentPath, dirContents[index].name));
            }
        } else if (button == lilka::Button::C) {
            showFileInfo(lilka::fileutils.joinPath(currentPath, dirContents[index].name));
        } else break; // B button
    }
}
void FileManagerApp::run() {
    lilka::serial_log("Opening path %s", currentPath.c_str());
    readDir(currentPath);
}

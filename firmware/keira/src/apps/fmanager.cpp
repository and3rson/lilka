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
    dirLoadProgress("Загрузка", ""),
    mkdirInput("Введіть назву нової папки"),
    renameInput("Введіть нову назву") {
    // Set stack size
    setStackSize(FM_STACK_SIZE);
    // File Options Menu:
    fileOptionsMenu.setTitle("Опції");
    fileOptionsMenu.addActivationButton(FM_EXIT_BUTTON);
    fileOptionsMenu.addItem("Відкрити");
    fileOptionsMenu.addItem("Відкрити з");
    fileOptionsMenu.addItem("Перейменувати");
    fileOptionsMenu.addItem("Копіювати");
    fileOptionsMenu.addItem("Перемістити");
    fileOptionsMenu.addItem("Видалити");
    fileOptionsMenu.addItem("Вибрати");
    fileOptionsMenu.addItem("Створити папку");
    // Open with Menu:
    fileOpenWithMenu.setTitle("Оберіть додаток");
    fileOpenWithMenu.addActivationButton(FM_EXIT_BUTTON);
    fileOpenWithMenu.addItem("Файловий менеджер");
    fileOpenWithMenu.addItem("Емулятор NES");
    fileOpenWithMenu.addItem("Загрузчик прошивок");
    fileOpenWithMenu.addItem("Lua");
    fileOpenWithMenu.addItem("mJS");
    fileOpenWithMenu.addItem("ЛілТрекер");
    fileOpenWithMenu.addItem("Програвач MOD");
    // File Selection Paste Menu
    fileSelectionPasteMenu.setTitle("Вставка");
    fileSelectionPasteMenu.addActivationButton(FM_EXIT_BUTTON);
    fileSelectionPasteMenu.addItem("Копіювати вибране");
    fileSelectionPasteMenu.addItem("Перемістити вибране");
    fileSelectionPasteMenu.addItem("Видалити вибране");
    fileSelectionPasteMenu.addItem("Очистити вибране");

    // Info Button
    fileListMenu.addActivationButton(lilka::Button::C);
    fileListMenu.addActivationButton(FM_EXIT_BUTTON);
    // Options Button
    fileListMenu.addActivationButton(lilka::Button::D);
    // in FM_MODE_COPY_SINGLE or FM_MODE_MOVE_SINGLE -> Paste
    // in FM_MODE_VIEW(default) dir Reload
    fileListMenu.addActivationButton(FM_PASTE_BUTTON);
    // Toggle selection
    fileListMenu.addActivationButton(lilka::Button::SELECT);

    currentPath = path;
}

String FileManagerApp::getFileMD5(const String& file_path) {
    FILE* file = fopen(file_path.c_str(), "rb");
    if (!file) {
        FM_DBG lilka::serial_err("MD5: Failed to open file: %s", file_path.c_str());
        return "Не обчислено";
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
        auto currentTime = millis();

        if ((lastProgress != progress) && ((currentTime - lastFrameTime) > PROGRESS_FRAME_TIME)) {
            lastProgress = progress;
            canvas->fillScreen(lilka::colors::Black);
            md5Progress.setProgress(progress);
            md5Progress.draw(canvas);
            lastFrameTime = currentTime;
            queueDraw();
        }

        if (lilka::controller.getState().a.justPressed) {
            fclose(file);
            return "Не обчислено";
        }
    }

    if (ferror(file)) {
        ESP_LOGE("MD5", "Error reading file: %s", file_path.c_str());
        fclose(file);
        mbedtls_md5_free(&ctx);
        return "Не обчислено";
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
            FM_DEFAULT_FT_NES_HANDLER(path);
            break;
        case FT_BIN:
            FM_DEFAULT_FT_BIN_HANDLER(path);
            break;
        case FT_LUA_SCRIPT:
            FM_DEFAULT_LUA_SCRIPT_HANDLER(path);
            break;
        case FT_JS_SCRIPT:
            FT_DEFAULT_JS_SCRIPT_HANDLER(path);
            break;
        case FT_MOD:
            FT_DEFAULT_MOD_HANDLER(path);
            break;
        case FT_LT:
            FT_DEFAULT_LT_HANDLER(path);
            break;
        case FT_DIR:
            FT_DEFAULT_DIR_HANDLER(path);
            break;
        case FT_OTHER:
            FT_DEFAULT_OTHER_HANDLER(entry);
            break;
    }
    vTaskDelay(SUSPEND_AWAIT_TIME / portTICK_RATE_MS);
    exitChildDialogs = true;
}
void FileManagerApp::fileOpenWithMenuShow(const FMEntry& entry) {
    while (1) {
        FM_CHILD_DIALOG_CHECKV;
        while (!fileOpenWithMenu.isFinished()) {
            fileOpenWithMenu.update();
            fileOpenWithMenu.draw(canvas);
            queueDraw();
        }
        auto button = fileOpenWithMenu.getButton();
        if (button == FM_EXIT_BUTTON) return; // BACK

        auto index = fileOpenWithMenu.getCursor();

        String path = lilka::fileutils.joinPath(entry.path, entry.name);
        if (index == 0) { // FileManager(zip vfs?)
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

bool FileManagerApp::areDirEntriesEqual(const FMEntry& ent1, const FMEntry& ent2) {
    return (ent1.name == ent2.name) && (ent1.path == ent2.path);
}

bool FileManagerApp::isCopyOrMoveCouldBeDone(const String& src, const String& dst) {
    FM_DBG lilka::serial_log("Check is copy/move possible for %s => %s", src.c_str(), dst.c_str());
    // TODO: remove last check from here, and ask user for confirm
    // Checks on src == dst; src exists; dst not exist;
    bool checks = (dst != src) && (access(src.c_str(), F_OK) == 0) && (access(dst.c_str(), F_OK) != 0);
    if (checks) {
        if (dst.indexOf(src) == 0) {
            // it still doesn't mean something, cause, well
            // it would work with path like /sd and /sdf
            if ((src.length() < dst.length()) && (dst[src.length()] == '/')) return false;
        }
        // path to dst exists;
        auto parentDir = lilka::fileutils.getParentDirectory(dst);
        if (access(parentDir.c_str(), F_OK) == 0) return true;
        // Note, current vfs implementation doesn't support check on existence of vfs root.
        // each vfs root should be added here. Cause lol, it definitely exists
        return (parentDir == LILKA_SD_ROOT || parentDir == LILKA_SPIFFS_ROOT);
    } else return false;
}

uint16_t FileManagerApp::getDirEntryIndex(const std::vector<FMEntry>& vec, const FMEntry& entry) {
    for (size_t it = 0; it < vec.size(); it++) {
        if (areDirEntriesEqual(vec[it], entry)) return it;
    }
    return ENTRY_NOT_FOUND_INDEX;
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
    // Set cursor to beining if not returned by B button
    fileOptionsMenu.setCursor(0);
    while (1) {
        FM_CHILD_DIALOG_CHECKV;
        while (!fileOptionsMenu.isFinished()) {
            fileOptionsMenu.update();
            fileOptionsMenu.draw(canvas);
            queueDraw();
        }
        auto button = fileOptionsMenu.getButton();

        if (button == FM_EXIT_BUTTON) return;

        auto index = fileOptionsMenu.getCursor();
        // Check on "Back selected"
        // Actually should be done in all possible handlers,
        // though, not all of them support it yet
        if ((index != 7) && (entry.name == ".")) {
            FM_UI_CANT_DO_OP;
            FM_MODE_RESET; // Do not perfform reset for mkdir
        }

        // TODO : replace with callbacks
        if (index == 0) openFileEntry(entry);
        else if (index == 1) fileOpenWithMenuShow(entry);
        else if (index == 2) renameInputShow(entry);
        else if (index == 3) { // Copy
            if (mode == FM_MODE_VIEW || mode == FM_MODE_MOVE_SINGLE || mode == FM_MODE_COPY_SINGLE)
                copySingleEntry(entry);
            else if (mode == FM_MODE_SELECT) {
                changeMode(FM_MODE_SELECT_COPY);
                pasteSelectedEntries();
            }
        } else if (index == 4) { // Move
            if (mode == FM_MODE_VIEW || mode == FM_MODE_MOVE_SINGLE || mode == FM_MODE_COPY_SINGLE)
                moveSingleEntry(entry);
            else if (mode == FM_MODE_SELECT) {
                changeMode(FM_MODE_SELECT_MOVE);
                pasteSelectedEntries();
            }
        } else if (index == 5) { // Delete
            deleteEntry(entry);
            return;
        } else if (index == 6) { // Select
            alertNotImplemented();
        } else if (index == 7) { // mkdir
            mkdirInputShow(entry.path);
        }
    }
}
void FileManagerApp::fileSelectionPasteMenuShow() {
    // Do draw
    while (!fileSelectionPasteMenu.isFinished()) {
        fileSelectionPasteMenu.update();
        fileSelectionPasteMenu.draw(canvas);
        queueDraw();
    }
    auto button = fileSelectionPasteMenu.getButton();
    if (button == FM_EXIT_BUTTON) return;
    auto index = fileSelectionPasteMenu.getCursor();
    switch (index) {
        case 0:
            changeMode(FM_MODE_SELECT_COPY);
            pasteSelectedEntries();
            break; // Copy selected
        case 1:
            changeMode(FM_MODE_SELECT_MOVE);
            pasteSelectedEntries();
            break; // Move selected
        case 3:
            alertNotImplemented();
            break;
        case 4:
            selectedDirEntries.clear();
            break; // Clear selection
    }

    FM_MODE_RESET;
}

bool FileManagerApp::fileListMenuLoadDir(const String& path) {
    auto dir = opendir(path.c_str());
    if (dir == NULL) { // Can't open dir
        alert("Помилка читання", String(errno) + ": " + strerror(errno));
        return false;
    }

    dirLoadProgress.setMessage(path);
    dirLoadProgress.setProgress(0);
    auto dirLength = 0;
    auto countLoaded = 0;
    while ((readdir(dir)) != NULL)
        dirLength++;
    rewinddir(dir);

    bool drawProgress = ((dirLength - PROGRESS_FILE_LIST_NO_DRAW_COUNT) >= 0);

    currentPath = path; // change path
    fileListMenu.setTitle(path);

    const struct dirent* dir_entry = NULL;

    while ((dir_entry = readdir(dir)) != NULL) {
        String filename = dir_entry->d_name;
        // Skip current directory and top level entries
        if (filename != "." && filename != "..") {
            FMEntry newEntry = pathToEntry(lilka::fileutils.joinPath(currentPath, filename));
            if (getDirEntryIndex(selectedDirEntries, newEntry) != ENTRY_NOT_FOUND_INDEX) newEntry.selected = true;
            currentDirEntries.push_back(newEntry);
        }
        // More interactive load
        countLoaded++;
        progress = countLoaded * 100 / dirLength;
        auto currentTime = millis();

        if ((drawProgress) && (lastProgress != progress) && ((currentTime - lastFrameTime) > PROGRESS_FRAME_TIME)) {
            lastProgress = progress;
            canvas->fillScreen(lilka::colors::Black);
            dirLoadProgress.setProgress(progress);
            dirLoadProgress.draw(canvas);
            lastFrameTime = currentTime;
            queueDraw();
        }
        if (lilka::controller.getState().a.justPressed) {
            FM_MODE_RESET;
            return false;
        }
    }
    closedir(dir); // unfortunately we can't reuse same dir

    if (drawProgress) {
        canvas->fillScreen(lilka::colors::Black);
        dirLoadProgress.setMessage("Сортування...");
        dirLoadProgress.draw(canvas);
        queueDraw();
    }

    // Sorting directory entries
    std::sort(currentDirEntries.begin(), currentDirEntries.end(), [](FMEntry a, FMEntry b) {
        if (a.type == FT_DIR && b.type != FT_DIR) return true;
        else if (a.type != FT_DIR && b.type == FT_DIR) return false;
        return a.name.compareTo(b.name) < 0;
    });

    if (drawProgress) {
        canvas->fillScreen(lilka::colors::Black);
        dirLoadProgress.setMessage("Майже готово...");
        dirLoadProgress.draw(canvas);
        queueDraw();
    }

    // Adding entries to menu
    for (auto dirEntry : currentDirEntries) {
        fileListMenu.addItem(
            dirEntry.name,
            dirEntry.selected ? FM_SELECTED_ICON : dirEntry.icon,
            dirEntry.color,
            dirEntry.type == FT_DIR ? "" : lilka::fileutils.getHumanFriendlySize(dirEntry.stat.st_size)
        );
    }

    // Add Back button
    fileListMenu.addItem("<< Назад", 0, 0);
    return true;
}

void FileManagerApp::fileListMenuShow(const String& path) {
    FM_DBG lilka::serial_log("Trying to load dir %s", path.c_str());
    int16_t index = 0;
    bool doReload = true; // at first load
    while (1) {
        exitChildDialogs = false;

        if (!stackSizeCheck()) return;

        if (doReload)
            if (!fileListMenuLoadDir(path)) return;

        doReload = false; // do not reload by default

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
        // currentDirEntries have exactly - 1 element inside
        // so we 've no need to change this
        if (index != (currentDirEntries.size())) {
            currentEntry = currentDirEntries[index];
            pCurrentEntry = &currentEntry;
        }

        bool exiting = false;

        // Toggle selection
        if (button == lilka::Button::SELECT) { // NORELOAD
            if (!pCurrentEntry) {
                FM_DBG lilka::serial_log("Selected last item with select do nothing");
                doReload = false;
                continue;
            }
            // Check if selected
            auto selectedIndex = getDirEntryIndex(selectedDirEntries, currentEntry);
            if (selectedIndex != ENTRY_NOT_FOUND_INDEX) {
                selectedDirEntries.erase(selectedDirEntries.begin() + selectedIndex);
                if (selectedDirEntries.size() == 0) FM_MODE_RESET;
                lilka::MenuItem fileListMenuItem;
                fileListMenu.getItem(index, &fileListMenuItem);
                fileListMenuItem.icon = currentDirEntries[index].icon;
                fileListMenu.setItem(
                    index,
                    fileListMenuItem.title,
                    fileListMenuItem.icon,
                    fileListMenuItem.color,
                    fileListMenuItem.postfix
                );
                // TODO :Check if mode reset needed
            }
            // FOUND:
            else {
                // TODO: ENTER other mode
                currentEntry.selected = true;
                selectedDirEntries.push_back(currentEntry);
                lilka::MenuItem fileListMenuItem;
                fileListMenu.getItem(index, &fileListMenuItem);

                fileListMenuItem.icon = FM_SELECTED_ICON;
                fileListMenu.setItem(
                    index,
                    fileListMenuItem.title,
                    fileListMenuItem.icon,
                    fileListMenuItem.color,
                    fileListMenuItem.postfix
                );
                changeMode(FM_MODE_SELECT);
            }
            continue;
        }
        // Info
        else if (button == lilka::Button::C) {
            if (pCurrentEntry) {
                fileInfoShowAlert(*pCurrentEntry);
                continue;
            } else exiting = true;
        }

        // clear memory
        currentDirEntries.clear();
        fileListMenu.clearItems();
        doReload = true; // after clear memory we

        // All options below would perform DIR RELOAD in all scenarios

        // Paste/Reload
        if (button == FM_PASTE_BUTTON) {
            if (mode == FM_MODE_COPY_SINGLE || mode == FM_MODE_MOVE_SINGLE) {
                pasteSingleEntry(singleMoveCopyEntry);
                changeMode(FM_MODE_VIEW);
                continue; // Reload anyways
            } else if (mode == FM_MODE_SELECT) {
                fileSelectionPasteMenuShow();
                continue;
            } else {
                MAKE_SANDWICH("Список файлів оновлено"); // TODO: find better
                // place for this
                FM_DBG lilka::serial_log("Force dir reload");

                continue; // Reload dir
            }
        }
        // Open
        if (button == FM_OKAY_BUTTON) {
            if (pCurrentEntry) openFileEntry(*pCurrentEntry);
            else exiting = true;
        }
        // Options
        else if (button == lilka::Button::D) {
            // if folder has no entries
            // pass . entry to allow mkdir
            if (pCurrentEntry) fileOptionsMenuShow(*pCurrentEntry);
            else fileOptionsMenuShow(currentEntry);
        }
        // Exit via B button
        else {
            exiting = true;
        }

        // Do some work on exit
        if (exiting) {
            // restore parent dir before exit
            exitChildDialogs = true;
            currentPath = lilka::fileutils.getParentDirectory(currentPath);
            break;
        }
    }
}

void FileManagerApp::renameInputShow(const FMEntry& entry) {
    FM_CHILD_DIALOG_CHECKV;
    FM_MODE_RESET;

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
    if (rename(path.c_str(), newPath.c_str()) == 0) FM_UI_SUCCESS_OP;
    else {
        FM_DBG lilka::serial_err(
            "Can't rename %s to %s. %d: %s", path.c_str(), newPath.c_str(), errno, strerror(errno)
        );
        FM_UI_CANT_DO_OP;
        FM_MODE_RESET;
    }
}

void FileManagerApp::deleteEntry(const FMEntry& entry, bool force) {
    FM_CHILD_DIALOG_CHECKV;
    // TODO: Add exit code, or rework without recursion
    // Can't FM_MODE_RESET here cause of recursive nature
    // could also fall here. exit gracefully
    if (!stackSizeCheck()) return;

    auto path = lilka::fileutils.joinPath(entry.path, entry.name);
    // Perform check on user sureness
    if (!force) {
        lilka::Alert checkAlert(
            "Ви впевнені?", String("Ця операція видалить файл\n") + path + "\nПродовжити: START\nВихід: B"
        );
        checkAlert.addActivationButton(FM_EXIT_BUTTON);
        checkAlert.addActivationButton(FM_CONFIRM_BUTTON);
        while (!checkAlert.isFinished()) {
            checkAlert.update();
            checkAlert.draw(canvas);
            queueDraw();
        }
        if (checkAlert.getButton() != FM_CONFIRM_BUTTON) return; //Exit
    }

    // Do job
    if (entry.type == FT_DIR) { // Directory
        auto dir = opendir(path.c_str());
        if (dir == NULL) {
            FM_DBG lilka::serial_err("Can't open dir %s. %d: %s", path.c_str(), errno, strerror(errno));
            FM_UI_CANT_DO_OP;
            FM_MODE_RESET;
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
            FM_UI_CANT_DO_OP;
            FM_MODE_RESET;
            return; // some shit happened. run!
        }
    } else { // Regular file
        if (unlink(path.c_str()) != 0) {
            FM_DBG lilka::serial_err("Tried to delete %s. %d: %s", path.c_str(), errno, strerror(errno));
            FM_UI_CANT_DO_OP;
            FM_MODE_RESET;
            return; // some shit happened. run!
        }
    }
}

bool FileManagerApp::changeMode(FmMode newMode) {
    FM_DBG lilka::serial_log("Trying to enter mode %d", newMode);
    mode = newMode;
    return true;
}

void FileManagerApp::copySingleEntry(const FMEntry& entry) {
    singleMoveCopyEntry = entry;
    // Change mode
    //if ((!changeMode(FM_MODE_COPY_SINGLE)) || entry.name == ".") {
    changeMode(FM_MODE_COPY_SINGLE);
    if (entry.name == ".") {
        FM_UI_CANT_DO_OP;
        FM_MODE_RESET;
        return;
    };
    exitChildDialogs = true;
    FM_UI_ADDED_TO_BUFFER;
}

void FileManagerApp::moveSingleEntry(const FMEntry& entry) {
    singleMoveCopyEntry = entry;
    // Change mode
    //if ((!changeMode(FM_MODE_MOVE_SINGLE)) || entry.name == ".") {
    changeMode(FM_MODE_MOVE_SINGLE);
    if (entry.name == ".") {
        FM_UI_CANT_DO_OP;
        FM_DBG lilka::serial_log("Can't enter mode FM_MODE_MOVE_SINGLE");
        FM_MODE_RESET;
        return;
    };
    exitChildDialogs = true;
    FM_UI_ADDED_TO_BUFFER;
}

bool FileManagerApp::copyPath(const String& source, const String& destination) {
    FM_CHILD_DIALOG_CHECKB;
    // Cause copying of large directories with long list of files made via recursion,
    // we could easily fail here
    if (!stackSizeCheck()) return false;

    if (!isCopyOrMoveCouldBeDone(source, destination)) {
        FM_UI_CANT_DO_OP;
        FM_MODE_RESET;
        return false;
    }
    // Exit, could happen if no stack left

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

            auto currentTime = millis();
            progress = bytesRead * 100 / fileSize;
            // TODO : fix AppManager->toast instead
            if ((lastProgress != progress) && ((currentTime - lastFrameTime) > PROGRESS_FRAME_TIME)) {
                lastProgress = progress;
                canvas->fillScreen(lilka::colors::Black);
                copyProgress.setProgress(progress);
                copyProgress.draw(canvas);
                lastFrameTime = currentTime;
                queueDraw();
            }
            if (lilka::controller.getState().a.justPressed) {
                FM_MODE_RESET;
                return false;
            }
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

void FileManagerApp::pasteSingleEntry(const FMEntry& entry) {
    auto source = lilka::fileutils.joinPath(entry.path, entry.name);
    auto destination = lilka::fileutils.joinPath(currentPath, entry.name);
    // check
    if (!isCopyOrMoveCouldBeDone(source, destination)) {
        FM_UI_CANT_DO_OP; // Allow to paste in other place
        return;
    }
    switch (mode) {
        case FM_MODE_COPY_SINGLE:
            if (copyPath(source, destination)) FM_UI_SUCCESS_OP;
            else {
                FM_UI_CANT_DO_OP;
                FM_MODE_RESET;
            }
            break;
        case FM_MODE_MOVE_SINGLE:
            if (rename(source.c_str(), destination.c_str()) == 0) FM_UI_SUCCESS_OP;
            else {
                FM_DBG lilka::serial_err(
                    "Can't rename %s to %s. %d: %s", source.c_str(), destination.c_str(), errno, strerror(errno)
                );
                FM_UI_CANT_DO_OP;
                FM_MODE_RESET;
            }
            break;
        default: { // SHIT MODE Mb perform all checks here and forget?
            FM_DBG lilka::serial_err("This should never happen. FM Mode works wrong way");
            FM_MODE_RESET;
        } break;
    }
    FM_MODE_RESET;
}
void FileManagerApp::pasteSelectedEntries() {
    auto currentMode = mode;
    for (auto selectedDirEntry : selectedDirEntries) {
        if (currentMode == FM_MODE_SELECT_COPY) {
            changeMode(FM_MODE_COPY_SINGLE);
            pasteSingleEntry(selectedDirEntry);
        } else if (currentMode == FM_MODE_SELECT_MOVE) {
            changeMode(FM_MODE_MOVE_SINGLE);
            pasteSingleEntry(selectedDirEntry);
        }
    }
    selectedDirEntries.clear();
    FM_MODE_RESET;
}

void FileManagerApp::mkdirInputShow(const String& path) {
    FM_MODE_RESET;
    while (!mkdirInput.isFinished()) {
        mkdirInput.update();
        mkdirInput.draw(canvas);
        queueDraw();
    }
    auto dirName = mkdirInput.getValue();
    if (dirName != "") {
        if (mkdir(lilka::fileutils.joinPath(path, dirName).c_str(), FM_MKDIR_MODE) == 0) FM_UI_SUCCESS_OP;
        else {
            FM_UI_CANT_DO_OP;
            FM_DBG lilka::serial_err(
                "Can't make dir in %s with name %s. %d: %s", path.c_str(), dirName.c_str(), errno, strerror(errno)
            );
        }
    }
}

bool FileManagerApp::stackSizeCheck() {
    auto stackLeft = uxTaskGetStackHighWaterMark(NULL);
    if (stackLeft < FM_STACK_MIN_FREE_SIZE) {
        alert("Помилка", "Недостатньо пам'яті аби завершити операіцю");
        FM_DBG lilka::serial_err("Stack left %d less than %d", stackLeft, FM_STACK_MIN_FREE_SIZE);
        FM_MODE_RESET;
        return false;
    }
    return true;
}
void FileManagerApp::run() {
    FM_DBG lilka::serial_log("Opening path %s", currentPath.c_str());
    fileListMenuShow(currentPath);
}
//  Drawing direction ->>>
// Text adjusted to bounds box
//
//
//
// HEIGHT by x
// WIDTH by y
// 0 0 is top let corner

void FileManagerApp::queueDraw() {
    drawStatusBar();
    App::queueDraw();
}

void FileManagerApp::drawStatusBar() {
    // TODO: just read global varialble with text
    // fill Space for status bar
    canvas->fillRect(
        0, canvas->height() - STATUS_BAR_HEIGHT, canvas->width(), STATUS_BAR_HEIGHT, STATUS_BAR_FILL_COLOR
    );
    // Uncomment those to get an idea where is x, and where is y
    // Draw xy
    //canvas->drawLine(0, 0, 50, 0, lilka::colors::Red);
    //canvas->drawLine(0, 0, 0, 50, lilka::colors::Blue);
    // canvas->printf("Height %d, width %d", canvas->height(), canvas->width());

    canvas->setCursor(STATUS_BAR_SAFE_DISTANCE, canvas->height() - 20 / 2); // FONT_Y / 2
    canvas->setFont(FONT_8x13);
    canvas->setTextColor(lilka::colors::White);
    // use left part
    //canvas->setTextBound(
    //   STATUS_BAR_SAFE_DISTANCE, canvas->height() - STATUS_BAR_HEIGHT, STATUS_BAR_WIDTH / 2, STATUS_BAR_HEIGHT
    //);
    canvas->setTextBound(
        STATUS_BAR_SAFE_DISTANCE, canvas->height() - STATUS_BAR_HEIGHT, STATUS_BAR_WIDTH, STATUS_BAR_HEIGHT
    );
    // print mode
    String modeStr;
    switch (mode) {
        case FM_MODE_COPY_SINGLE:
            modeStr = "COPY";
            break;
        case FM_MODE_MOVE_SINGLE:
            modeStr = "MOVE";
            break;
        case FM_MODE_SELECT:
            modeStr = String("SELECTED ") + String(selectedDirEntries.size()) + " entries";
            break;
        case FM_MODE_VIEW:
            modeStr =
                currentDirEntries.size() == 0 ? "" : String("Total: ") + String(currentDirEntries.size()) + " entries";
            break;
        case FM_MODE_SELECT_MOVE:
            modeStr = "MOVING " + String(selectedDirEntries.size()) + " entries";
            break;
        case FM_MODE_SELECT_COPY:
            modeStr = "COPYING" + String(selectedDirEntries.size()) + " entries";
            break;
        default:
            modeStr = "UNKNOWN";
            break;
    }
    canvas->printf("%s", modeStr.c_str());
}
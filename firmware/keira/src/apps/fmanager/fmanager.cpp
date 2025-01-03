#include "fmanager.h"

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

    // FILE OPTIONS MENU SETUP:
    fileOptionsMenu.setTitle("Опції");
    fileOptionsMenu.addItem("Відкрити", 0, 0U, "", FM_CALLBACK_CAST(onFileOptionsMenuOpen), FM_CALLBACK_PTHIS);
    fileOptionsMenu.addItem("Відкрити з", 0, 0U, "", FM_CALLBACK_CAST(onFileOptionsMenuOpenWith), FM_CALLBACK_PTHIS);
    fileOptionsMenu.addItem("Створити папку", 0, 0U, "", FM_CALLBACK_CAST(onFileOptionsMenuMKDir), FM_CALLBACK_PTHIS);
    fileOptionsMenu.addItem("Перейменувати", 0, 0U, "", FM_CALLBACK_CAST(onFileOptionsMenuRename), FM_CALLBACK_PTHIS);
    fileOptionsMenu.addItem("Видалити", 0, 0U, "", FM_CALLBACK_CAST(onFileOptionsMenuDelete), FM_CALLBACK_PTHIS);
    fileOptionsMenu.addItem("Інформація", 0, 0U, "", FM_CALLBACK_CAST(onFileOptionsMenuInfo), FM_CALLBACK_PTHIS);
    fileOptionsMenu.addItem("<< Назад", 0, 0U, "", FM_CALLBACK_CAST(onAnyMenuBack), FM_CALLBACK_PTHIS);

    fileOptionsMenu.addActivationButton(FM_EXIT_BUTTON);

    // FILE OPEN WITH MENU SETUP:
    fileOpenWithMenu.setTitle("Оберіть додаток");
    fileOpenWithMenu.addItem(
        "Файловий менеджер", 0, 0U, "", FM_CALLBACK_CAST(onFileOpenWithFileManager), FM_CALLBACK_PTHIS
    );
    fileOpenWithMenu.addItem("Емулятор NES", 0, 0U, "", FM_CALLBACK_CAST(onFileOpenWithNESEmulator), FM_CALLBACK_PTHIS);
    fileOpenWithMenu.addItem(
        "Загрузчик прошивок", 0, 0U, "", FM_CALLBACK_CAST(onFileOpenWithMultiBootLoader), FM_CALLBACK_PTHIS
    );
    fileOpenWithMenu.addItem("Lua", 0, 0U, "", FM_CALLBACK_CAST(onFileOpenWithLua), FM_CALLBACK_PTHIS);
    fileOpenWithMenu.addItem("mJS", 0, 0U, "", FM_CALLBACK_CAST(onFileOpenWithMJS), FM_CALLBACK_PTHIS);
    fileOpenWithMenu.addItem("ЛілТрекер", 0, 0U, "", FM_CALLBACK_CAST(onFileOpenWithLilTracker), FM_CALLBACK_PTHIS);
    fileOpenWithMenu.addItem("Програвач MOD", 0, 0U, "", FM_CALLBACK_CAST(onFileOpenWithMODPlayer), FM_CALLBACK_PTHIS);
    fileOpenWithMenu.addItem("<< Назад", 0, 0U, "", FM_CALLBACK_CAST(onAnyMenuBack), FM_CALLBACK_PTHIS);

    fileOpenWithMenu.addActivationButton(FM_EXIT_BUTTON);

    // FILE SELECTION PASTE MENU SETUP:
    fileSelectionOptionsMenu.setTitle("Дії над вибраним");
    fileSelectionOptionsMenu.addItem(
        "Копіювати вибране", 0, 0U, "", FM_CALLBACK_CAST(onFileSelectionOptionsMenuCopy), FM_CALLBACK_PTHIS
    );
    fileSelectionOptionsMenu.addItem(
        "Перемістити вибране", 0, 0U, "", FM_CALLBACK_CAST(onFileSelectionOptionsMenuMove), FM_CALLBACK_PTHIS
    );
    fileSelectionOptionsMenu.addItem(
        "Видалити вибране", 0, 0U, "", FM_CALLBACK_CAST(onFileSelectionOptionsMenuDelete), FM_CALLBACK_PTHIS
    );
    fileSelectionOptionsMenu.addItem(
        "Очистити вибране", 0, 0U, "", FM_CALLBACK_CAST(onFileSelectionOptionsMenuClearSelection), FM_CALLBACK_PTHIS
    );

    fileSelectionOptionsMenu.addActivationButton(FM_EXIT_BUTTON);

    // FILE LIST MENU SETUP:
    // CALLBACK_SETUP IN OTHER PLACE
    // Info Button
    fileListMenu.addActivationButton(FM_INFO_BUTTON);
    fileListMenu.addActivationButton(FM_EXIT_BUTTON);
    // Options Button
    fileListMenu.addActivationButton(FM_OPTIONS_MENU_BUTTON);
    // in FM_MODE_COPY_SINGLE or FM_MODE_MOVE_SINGLE -> Paste
    // in FM_MODE_VIEW(default) dir Reload
    // Toggle selection
    fileListMenu.addActivationButton(FM_SELECT_BUTTON);

    currentPath = path;
    initalPath = currentPath;
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
    // mb create on init?
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
    struct stat tmpStat;
    if (newEntry.name == "." && stat(newEntry.path.c_str(), &(tmpStat)) == 0) {
        statPerformed = true;
    } else {
        if (stat(path.c_str(), &tmpStat) == 0) {
            statPerformed = true;
        }
    }
    // Probably we don't need a whole structure, so took only interesting values
    newEntry.st_size = tmpStat.st_size;
    newEntry.st_mode = tmpStat.st_mode;

    if (statPerformed) {
        if (S_ISDIR(newEntry.st_mode)) {
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

void FileManagerApp::openCurrentEntry() {
    String path = lilka::fileutils.joinPath(currentEntry.path, currentEntry.name);
    FM_DBG lilka::serial_log("Opening path %s", path.c_str());
    if (currentEntry.type == FT_DIR) {
        if (currentEntry.name == ".") {
            if (currentPath != initalPath) {
                currentPath = lilka::fileutils.getParentDirectory(currentPath);
                changeMode(FM_MODE_RELOAD);
                return;
            }
        }
    }
    switch (currentEntry.type) {
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
            FT_DEFAULT_DIR_HANDLER;
            break;
        case FT_OTHER:
            FT_DEFAULT_OTHER_HANDLER;
            break;
    }
    vTaskDelay(SUSPEND_AWAIT_TIME / portTICK_RATE_MS);
    changeMode(FM_MODE_RELOAD);
    exitChildDialogs = true;
}

void FileManagerApp::selectCurrentEntry() {
    if (currentEntry.name == ".") return;
    auto index = getDirEntryIndex(selectedDirEntries, currentEntry);
    if (index == ENTRY_NOT_FOUND_INDEX) {
        currentEntry.selected = true;
        selectedDirEntries.push_back(currentEntry);
        auto index2 = getDirEntryIndex(currentDirEntries, currentEntry);
        if (index2 != ENTRY_NOT_FOUND_INDEX) {
            lilka::MenuItem mbuff;
            fileListMenu.getItem(index2, &mbuff);
            fileListMenu.setItem(
                index2, mbuff.title, FM_SELECTED_ICON, mbuff.color, mbuff.postfix

            ); // add callback setup in setItem
            currentDirEntries[index2].selected = true;
            changeMode(FM_MODE_SELECT);
        } else lilka::serial_err("FM:This should never happen!");
    }
}

void FileManagerApp::deselectCurrentEntry() {
    if (currentEntry.name == ".") return;
    auto index = getDirEntryIndex(selectedDirEntries, currentEntry);
    if (index != ENTRY_NOT_FOUND_INDEX) {
        currentEntry.selected = false;
        selectedDirEntries.erase(selectedDirEntries.begin() + index);
        auto index2 = getDirEntryIndex(currentDirEntries, currentEntry);
        if (index2 != ENTRY_NOT_FOUND_INDEX) {
            lilka::MenuItem mbuff;
            fileListMenu.getItem(index2, &mbuff);
            fileListMenu.setItem(
                index2, mbuff.title, currentEntry.icon, mbuff.color, mbuff.postfix

            );
            currentDirEntries[index2].selected = false;
        } else lilka::serial_err("This should never happen!");
    }
    if (selectedDirEntries.size() == 0) changeMode(FM_MODE_VIEW);
}

void FileManagerApp::clearSelectedEntries() {
    for (auto entry : selectedDirEntries) {
        auto index = getDirEntryIndex(currentDirEntries, entry);
        if (index != ENTRY_NOT_FOUND_INDEX) {
            lilka::MenuItem mbuff;
            fileListMenu.getItem(index, &mbuff);
            fileListMenu.setItem(index, mbuff.title, entry.icon, mbuff.color, mbuff.postfix);
        }
    }
    selectedDirEntries.clear();
    changeMode(FM_MODE_VIEW);
}

void FileManagerApp::fileOpenWithMenuShow() {
    FM_CHILD_DIALOG_CHECKV;
    while (!fileOpenWithMenu.isFinished()) {
        fileOpenWithMenu.update();
        fileOpenWithMenu.draw(canvas);
        queueDraw();
    }
}

void FileManagerApp::onFileOpenWithFileManager() {
    FM_DBG lilka::serial_log("Enter onFileOpenWithFileManager");
    auto button = fileOpenWithMenu.getButton();
    if (button == FM_OKAY_BUTTON) {
        if (currentEntry.type == FT_DIR) {
            if (currentEntry.name == ".") {
                if (currentPath != initalPath) {
                    currentPath = lilka::fileutils.getParentDirectory(currentPath);
                    changeMode(FM_MODE_RELOAD);
                    return;
                }
            }
            currentPath = lilka::fileutils.joinPath(currentEntry.path, currentEntry.name);
            changeMode(FM_MODE_RELOAD);
        } else alert("Помилка", "Не підтримується");
    } else if (button != FM_EXIT_BUTTON) fileOpenWithMenu.isFinished(); // do redraw
}

void FileManagerApp::onFileOpenWithNESEmulator() {
    FM_DBG lilka::serial_log("Enter onFileOpenWithNESEmulator");
    auto button = fileOpenWithMenu.getButton();
    if (button == FM_OKAY_BUTTON) {
        AppManager::getInstance()->runApp(new NesApp(lilka::fileutils.joinPath(currentEntry.path, currentEntry.name)));
    } else if (button != FM_EXIT_BUTTON) fileOpenWithMenu.isFinished(); // do redraw
}

void FileManagerApp::onFileOpenWithMultiBootLoader() {
    FM_DBG lilka::serial_log("Enter onFileOpenWithMultiBootLoader");
    auto button = fileOpenWithMenu.getButton();
    if (button == FM_OKAY_BUTTON) {
        fileLoadAsRom(lilka::fileutils.joinPath(currentEntry.path, currentEntry.name));
    } else if (button != FM_EXIT_BUTTON) fileOpenWithMenu.isFinished(); // do redraw
}

void FileManagerApp::onFileOpenWithLua() {
    FM_DBG lilka::serial_log("Enter onFileOpenWithLua");
    auto button = fileOpenWithMenu.getButton();
    if (button == FM_OKAY_BUTTON) {
        AppManager::getInstance()->runApp(
            new LuaFileRunnerApp(lilka::fileutils.joinPath(currentEntry.path, currentEntry.name))
        );
    } else if (button != FM_EXIT_BUTTON) fileOpenWithMenu.isFinished(); // do redraw
}

void FileManagerApp::onFileOpenWithMJS() {
    FM_DBG lilka::serial_log("Enter onFileOpenWithMJS");
    auto button = fileOpenWithMenu.getButton();
    if (button == FM_OKAY_BUTTON) {
        AppManager::getInstance()->runApp(new MJSApp(lilka::fileutils.joinPath(currentEntry.path, currentEntry.name)));
    } else if (button != FM_EXIT_BUTTON) fileOpenWithMenu.isFinished(); // do redraw
}

void FileManagerApp::onFileOpenWithLilTracker() {
    FM_DBG lilka::serial_log("Enter onFileOpenWithLilTracker");
    auto button = fileOpenWithMenu.getButton();
    if (button == FM_OKAY_BUTTON) {
        AppManager::getInstance()->runApp(
            new LilTrackerApp(lilka::fileutils.joinPath(currentEntry.path, currentEntry.name))
        );
    } else if (button != FM_EXIT_BUTTON) fileOpenWithMenu.isFinished(); // do redraw
}

void FileManagerApp::onFileOpenWithMODPlayer() {
    FM_DBG lilka::serial_log("Enter onFileOpenWithMODPlayer");
    auto button = fileOpenWithMenu.getButton();
    if (button == FM_OKAY_BUTTON) {
        AppManager::getInstance()->runApp(
            new ModPlayerApp(lilka::fileutils.joinPath(currentEntry.path, currentEntry.name))
        );
    } else if (button != FM_EXIT_BUTTON) fileOpenWithMenu.isFinished(); // do redraw
}

void FileManagerApp::onFileSelectionOptionsMenuCopy() {
    auto button = fileSelectionOptionsMenu.getButton();
    if (button == FM_OKAY_BUTTON) {
        for (auto entry : selectedDirEntries) {
            auto src = lilka::fileutils.joinPath(entry.path, entry.name);
            auto dst = lilka::fileutils.joinPath(currentPath, entry.name);
            if (!copyPath(src, dst)) {
                clearSelectedEntries();
                return;
            }
        }
        clearSelectedEntries();
        changeMode(FM_MODE_RELOAD);
    } else if (button != FM_EXIT_BUTTON) fileSelectionOptionsMenu.isFinished();
}

void FileManagerApp::onFileSelectionOptionsMenuMove() {
    auto button = fileSelectionOptionsMenu.getButton();
    if (button == FM_OKAY_BUTTON) {
        for (auto entry : selectedDirEntries) {
            auto src = lilka::fileutils.joinPath(entry.path, entry.name);
            auto dst = lilka::fileutils.joinPath(currentPath, entry.name);
            if (!movePath(src, dst)) {
                clearSelectedEntries();
                return;
            }
        }
        clearSelectedEntries();
        changeMode(FM_MODE_RELOAD);
    } else if (button != FM_EXIT_BUTTON) fileSelectionOptionsMenu.isFinished();
}

void FileManagerApp::onFileSelectionOptionsMenuDelete() {
    auto button = fileSelectionOptionsMenu.getButton();
    if (button == FM_OKAY_BUTTON) {
        lilka::Alert checkAlert(
            "Ви впевнені?",
            String("Ця операція видалить ") + String(selectedDirEntries.size()) + " файл(ів)" +
                "\nПродовжити: START\nВихід: B"
        );
        checkAlert.addActivationButton(FM_CONFIRM_BUTTON);
        checkAlert.addActivationButton(FM_EXIT_BUTTON);
        while (!checkAlert.isFinished()) {
            checkAlert.update();
            checkAlert.draw(canvas);
            queueDraw();
        }
        if (checkAlert.getButton() == FM_CONFIRM_BUTTON) {
            for (auto entry : selectedDirEntries)
                deleteEntry(entry, true);
            clearSelectedEntries();
            changeMode(FM_MODE_RELOAD);
        } else {
            clearSelectedEntries(); // clear selection anyways
            return;
        }
    } else if (button != FM_EXIT_BUTTON) fileSelectionOptionsMenu.isFinished();
}

void FileManagerApp::onFileSelectionOptionsMenuClearSelection() {
    auto button = fileSelectionOptionsMenu.getButton();
    if (button == FM_OKAY_BUTTON) {
        clearSelectedEntries();
    } else if (button != FM_EXIT_BUTTON) fileSelectionOptionsMenu.isFinished();
}

// FILE OPTIONS MENU BELOW:
void FileManagerApp::fileOptionsMenuShow() {
    FM_DBG lilka::serial_log("Enter fileOptionsMenuShow");
    fileOptionsMenu.setCursor(0);
    while (!fileOptionsMenu.isFinished()) {
        fileOptionsMenu.update();
        fileOptionsMenu.draw(canvas);
        queueDraw();
    }
}

void FileManagerApp::onFileOptionsMenuOpen() {
    auto button = fileOptionsMenu.getButton();
    FM_DBG lilka::serial_log("Enter onFileOptionsMenuOpen");
    if (button == FM_OKAY_BUTTON) {
        openCurrentEntry();
    } else if (button != FM_EXIT_BUTTON) fileOptionsMenu.isFinished(); // do redraw
}

void FileManagerApp::onFileOptionsMenuOpenWith() {
    auto button = fileOptionsMenu.getButton();
    FM_DBG lilka::serial_log("Enter onFileOptionsMenuOpenWith");
    if (button == FM_OKAY_BUTTON) {
        fileOpenWithMenuShow();

    } else if (button != FM_EXIT_BUTTON) fileOpenWithMenu.isFinished(); // do redraw
}

void FileManagerApp::onFileOptionsMenuMKDir() {
    auto button = fileOptionsMenu.getButton();
    FM_DBG lilka::serial_log("Enter onFileOptionsMenuMKDir");
    if (button == FM_OKAY_BUTTON) {
        mkdirInput.setValue(FM_DEFAULT_NEW_FOLDER_NAME);
        while (!mkdirInput.isFinished()) {
            mkdirInput.update();
            mkdirInput.draw(canvas);
            queueDraw();
        }
        auto dirName = mkdirInput.getValue();
        if (dirName != "") {
            if (mkdir(lilka::fileutils.joinPath(currentEntry.path, dirName).c_str(), FM_MKDIR_MODE) == 0) {
                FM_UI_SUCCESS_OP;
                changeMode(FM_MODE_RELOAD);
            } else {
                FM_UI_CANT_DO_OP;
                FM_DBG lilka::serial_err(
                    "Can't make dir in %s with name %s. %d: %s",
                    currentEntry.path.c_str(),
                    dirName.c_str(),
                    errno,
                    strerror(errno)
                );
            }
            exitChildDialogs = true;
        }
    } else if (button != FM_EXIT_BUTTON) fileOpenWithMenu.isFinished(); // do redraw
}

void FileManagerApp::onFileOptionsMenuDelete() {
    auto button = fileOptionsMenu.getButton();
    FM_DBG lilka::serial_log("Enter onFileOptionsMenuDelete");
    if (button == FM_OKAY_BUTTON) {
        if (mode == FM_MODE_SELECT) {
            lilka::Alert checkAlert(
                "Ви впевнені?",
                String("Ця операція видалить ") + selectedDirEntries.size() + " файл(ів)" +
                    "\nПродовжити: START\nВихід: B"
            );
            checkAlert.addActivationButton(FM_EXIT_BUTTON);
            checkAlert.addActivationButton(FM_CONFIRM_BUTTON);
            while (!checkAlert.isFinished()) {
                checkAlert.update();
                checkAlert.draw(canvas);
                queueDraw();
            }
            if (checkAlert.getButton() == FM_CONFIRM_BUTTON) {
                for (auto entry : selectedDirEntries) {
                    deleteEntry(entry, true);
                }
                clearSelectedEntries();
                changeMode(FM_MODE_RELOAD);
            } else return;
        } else {
            deleteEntry(currentEntry);
            changeMode(FM_MODE_RELOAD);
        }
    } else if (button != FM_EXIT_BUTTON) fileOpenWithMenu.isFinished(); // do redraw
}

void FileManagerApp::onFileOptionsMenuRename() {
    auto button = fileOptionsMenu.getButton();
    FM_DBG lilka::serial_log("Enter onFileOptionsMenuRename");
    if (button == FM_OKAY_BUTTON) {
        renameInput.setValue(currentEntry.name); // pass old name
        while (!renameInput.isFinished()) {
            renameInput.update();
            renameInput.draw(canvas);
            queueDraw();
        }

        auto newName = renameInput.getValue();

        // Skip empty and same names
        if (newName == "" || newName == currentEntry.name) {
            exitChildDialogs = true;
            return;
        }

        auto path = lilka::fileutils.joinPath(currentEntry.path, currentEntry.name);
        auto newPath = lilka::fileutils.joinPath(currentEntry.path, newName);

        // Try rename
        if (rename(path.c_str(), newPath.c_str()) == 0) {
            FM_UI_SUCCESS_OP;
            changeMode(FM_MODE_RELOAD);

        } else {
            FM_DBG lilka::serial_err(
                "Can't rename %s to %s. %d: %s", path.c_str(), newPath.c_str(), errno, strerror(errno)
            );
            FM_UI_CANT_DO_OP;
        }
        exitChildDialogs = true;
    } else if (button != FM_EXIT_BUTTON) fileOpenWithMenu.isFinished(); // do redraw
}

void FileManagerApp::onFileOptionsMenuInfo() {
    auto button = fileOptionsMenu.getButton();
    FM_DBG lilka::serial_log("Enter onFileOptionsMenuRename");
    if (button == FM_OKAY_BUTTON) {
        fileInfoShowAlert();
        exitChildDialogs = true;
    } else if (button != FM_EXIT_BUTTON) fileOpenWithMenu.isFinished(); // do redraw
}

void FileManagerApp::fileInfoShowAlert() {
    String info;
    if (currentEntry.type == FT_DIR) {
        info = "Тип: директорія\n";
    } else {
        info = "Тип: файл\n";
        info += "Розмір: " + lilka::fileutils.getHumanFriendlySize(currentEntry.st_size) + "\n";
        info += "MD5: " + getFileMD5(lilka::fileutils.joinPath(currentEntry.path, currentEntry.name)) + "\n";
    }

    alert(currentEntry.name, info);
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
            if ((src.length() < dst.length()) && (dst[src.length()] == '/')) {
                // clear errno to not be displayed on statusBar
                errno = 0;
                return false;
            }
        }
        // path to dst exists;
        auto parentDir = lilka::fileutils.getParentDirectory(dst);
        if (access(parentDir.c_str(), F_OK) == 0) {
            // clear errno to not be displayed on statusBar
            errno = 0;
            return true;
        }
        // Note, current vfs implementation doesn't support check on existence of vfs root.
        // each vfs root should be added here. Cause lol, it definitely exists
        // clear errno to not be displayed on statusBar
        errno = 0;
        return (parentDir == LILKA_SD_ROOT || parentDir == LILKA_SPIFFS_ROOT);
    } else {
        // clear errno to not be displayed on statusBar
        errno = 0;
        return false;
    }
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

void FileManagerApp::onAnyMenuBack() {
    FM_DBG lilka::serial_log("Enter onAnyMenuBack");
}

void FileManagerApp::fileSelectionOptionsMenuShow() {
    while (!fileSelectionOptionsMenu.isFinished()) {
        fileSelectionOptionsMenu.update();
        fileSelectionOptionsMenu.draw(canvas);
        queueDraw();
    }
}

bool FileManagerApp::fileListMenuLoadDir() {
    auto dir = opendir(currentPath.c_str());
    if (dir == NULL) { // Can't open dir
        alert("Помилка", "Не можу відкрити директорію");
        return false;
    }

    currentDirEntries.clear();
    fileListMenu.clearItems();

    dirLoadProgress.setMessage(currentPath);
    dirLoadProgress.setProgress(0);
    auto dirLength = 0;
    auto countLoaded = 0;
    while ((readdir(dir)) != NULL)
        dirLength++;
    rewinddir(dir);

    bool drawProgress = ((dirLength - PROGRESS_FILE_LIST_NO_DRAW_COUNT) >= 0);

    fileListMenu.setTitle(currentPath);

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
            dirEntry.type == FT_DIR ? "" : lilka::fileutils.getHumanFriendlySize(dirEntry.st_size),
            FM_CALLBACK_CAST(onFileListMenuItem),
            FM_CALLBACK_PTHIS
        );
    }

    // Add Back button
    // We can't reuse onAnyMenuBack here
    fileListMenu.addItem("<< Назад", 0, 0, "", FM_CALLBACK_CAST(onFileListMenuItem), FM_CALLBACK_PTHIS);
    return true;
}

void FileManagerApp::fileListMenuShow() {
    FM_DBG lilka::serial_log("Trying to load dir %s", currentPath.c_str());
    int16_t index = 0;
    exitChildDialogs = false;

    if (!stackSizeCheck()) return;

    // Try to restore old menuCursor:
    // last option should be selected cause something should be deleted
    // if this happens. here we assume that we deleted single item
    if (fileListMenu.getItemCount() >= index) fileListMenu.setCursor(index);
    else fileListMenu.setCursor(fileListMenu.getItemCount() - 1); // Select last

    // Do Draw !
    while (!fileListMenu.isFinished()) {
        if (mode == FM_MODE_RELOAD) {
            if (!fileListMenuLoadDir()) return;
            // if some entries selected, return to FM_MODE_SELECT
            // could happen when pathing to new dir
            if (selectedDirEntries.size() == 0) changeMode(FM_MODE_VIEW);
            else changeMode(FM_MODE_SELECT);
        }
        exitChildDialogs = false;

        fileListMenu.update();
        fileListMenu.draw(canvas);
        queueDraw();
    }

    // use std::queue to store cursor positions
    index = fileListMenu.getCursor();
    auto button = fileListMenu.getButton();

    // currentDirEntries have exactly - 1 element inside
    // so we 've no need to change this
}

void FileManagerApp::onFileListMenuItem() {
    auto button = fileListMenu.getButton();
    auto index = fileListMenu.getCursor();
    FM_DBG lilka::serial_log("Enter onFileListMenuItem");

    if (fileListMenu.getCursor() == currentDirEntries.size()) {
        currentEntry = pathToEntry(lilka::fileutils.joinPath(currentPath, "."));
    } else currentEntry = currentDirEntries[fileListMenu.getCursor()];

    FM_DBG lilka::serial_log("currentEntry path = %s, name = %s", currentEntry.path.c_str(), currentEntry.name.c_str());
    currentPath = currentEntry.path;
    FM_DBG lilka::serial_log("Current path = %s", currentPath.c_str());
    FM_DBG lilka::serial_log("Button = %d", button);
    if (button == FM_CONFIRM_BUTTON) {
        changeMode(FM_MODE_RELOAD);
    }
    if (button == FM_EXIT_BUTTON) {
        if (currentPath != initalPath) {
            currentPath = lilka::fileutils.getParentDirectory(currentPath);
            fileListMenu.isFinished();
            changeMode(FM_MODE_RELOAD);
            return;
        } else return;
    }

    if (button == FM_SELECT_BUTTON) {
        // Check if selected
        if (currentEntry.selected) deselectCurrentEntry();
        else selectCurrentEntry();
        fileListMenu.isFinished();
        return;
    }

    if (button == FM_INFO_BUTTON && currentEntry.name != ".") {
        fileInfoShowAlert();
        fileListMenu.isFinished();
        return;
    }

    // Open
    if (button == FM_OKAY_BUTTON) {
        if (currentEntry.name == ".") {
            if (currentPath != initalPath) {
                currentPath = lilka::fileutils.getParentDirectory(currentPath);
                changeMode(FM_MODE_RELOAD);
                fileListMenu.isFinished();
            }

        } else {
            changeMode(FM_MODE_RELOAD); // reload on open
            openCurrentEntry();
            fileListMenu.isFinished();
        }
        return;
    }
    // Options
    if (button == FM_OPTIONS_MENU_BUTTON) {
        if (mode == FM_MODE_SELECT) {
            fileSelectionOptionsMenuShow();
            fileListMenu.isFinished();
        } else {
            fileOptionsMenuShow();
            fileListMenu.isFinished();
        }
        return;
    }
}

/////
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
    //////
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

bool FileManagerApp::movePath(const String& source, const String destination) {
    if (!isCopyOrMoveCouldBeDone(source, destination)) {
        FM_UI_CANT_DO_OP;
        FM_MODE_RESET;
        return false;
    }
    if (rename(source.c_str(), destination.c_str()) != 0) {
        FM_UI_CANT_DO_OP;
        FM_MODE_RESET;
        return false;
    }

    return true;
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
        ///////////////////////////////////
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
        ///////////////////////////////////
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
    fileListMenuShow();
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

    // use left part
    //canvas->setTextBound(
    //   STATUS_BAR_SAFE_DISTANCE, canvas->height() - STATUS_BAR_HEIGHT, STATUS_BAR_WIDTH / 2, STATUS_BAR_HEIGHT
    //);
    canvas->setTextBound(
        STATUS_BAR_SAFE_DISTANCE, canvas->height() - STATUS_BAR_HEIGHT, STATUS_BAR_WIDTH, STATUS_BAR_HEIGHT
    );
    if (errno != 0) {
        errnoTime = millis();
        errnoStr = String(errno) + ":" + strerror(errno);
        errno = 0;
    }
    if ((millis() - errnoTime) <= FM_ERRNO_TIME) {
        canvas->setTextColor(lilka::colors::Red);
        canvas->printf("%s", errnoStr.c_str());
        return;
    }
    // Other significant data to show
    if (mode == FM_MODE_SELECT) {
        canvas->setTextColor(lilka::colors::White);
        canvas->printf("Вибрано %d файл(ів)", selectedDirEntries.size());
    } else if (mode == FM_MODE_VIEW) {
        canvas->setTextColor(lilka::colors::White);
        auto fileListMenuIndex = fileListMenu.getCursor();
        auto dirLength = currentDirEntries.size();
        if (fileListMenuIndex != dirLength) canvas->printf("[ %d / %d ]", fileListMenuIndex + 1, dirLength);
    }
}
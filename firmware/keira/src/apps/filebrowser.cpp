#include <memory> // unique_ptr

#include "filebrowser.h"
#include "appmanager.h"

// Runners:
#include "lua/luarunner.h"
#include "mjs/mjsrunner.h"
#include "nes/nesapp.h"

// Icons:
#include "icons/normalfile.h"
#include "icons/folder.h"
#include "icons/nes.h"
#include "icons/bin.h"
#include "icons/lua.h"
#include "icons/js.h"

FileBrowserApp::FileBrowserApp() : App(APP_FBROWSER_NAME), currentDir(0) {
}

String FileBrowserApp::getHumanReadableSize(size_t size) {
// Max length of file size
#define H_FILE_SIZE 6
    const char* suffixes[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    const int numSuffixes = sizeof(suffixes) / sizeof(suffixes[0]);
    if (size == 0) return "0B";

    int exp = 0;
    double dsize = (double)(size);

    while (dsize >= 1024 && exp < numSuffixes - 1) {
        dsize /= 1024;
        exp++;
    }

    char buffer[50];
    snprintf(buffer, sizeof(buffer), "%.0f%s", dsize, suffixes[exp]);
    String hFileSize(buffer);
    while (hFileSize.length() != H_FILE_SIZE) {
        hFileSize = hFileSize + " ";
    }
#undef H_FILE_SIZE
    return hFileSize;
}

String FileBrowserApp::getFileInfo(const String& path) {
    struct stat file_stat;
    if (stat(path.c_str(), &file_stat) != 0) {
        return String("Failed to retrieve file information.\n") + strerror(errno);
    }

    String file_info;

    // File path
    file_info += "File Path: " + path + "\n";

    // File type
    file_info += "File Type: ";
    if (S_ISREG(file_stat.st_mode)) file_info += "Regular File\n";
    else if (S_ISDIR(file_stat.st_mode)) file_info += "Directory\n";
    else if (S_ISLNK(file_stat.st_mode)) file_info += "Symbolic Link\n";
    else if (S_ISCHR(file_stat.st_mode)) file_info += "Character Device\n";
    else if (S_ISBLK(file_stat.st_mode)) file_info += "Block Device\n";
    else if (S_ISFIFO(file_stat.st_mode)) file_info += "FIFO (Named Pipe)\n";
    else if (S_ISSOCK(file_stat.st_mode)) file_info += "Socket\n";
    else file_info += "Unknown\n";

    // File size
    file_info = file_info + "File Size: " + String(file_stat.st_size) + " bytes\n";
    /*      FAT32 doesn't support this:
    // File mode (permissions)
    file_info = file_info + "File Mode: " + fileModeToOctal(file_stat.st_mode) + "\n";

    // Owner User ID and Group ID
    file_info = file_info + "Owner User ID: " + String(file_stat.st_uid) + "\n";
    file_info = file_info + "Owner User: " + getUserNameFromUID(file_stat.st_uid) + "\n";
    file_info = file_info + "Owner Group ID: " + String(file_stat.st_gid) + "\n";
    file_info = file_info + "Owner Group: " + getGroupNameFromGID(file_stat.st_gid) + "\n";
    */
    // Access time, Modification time, and Inode change time
    file_info += "Last Access Time: " + String(std::ctime(&file_stat.st_atime));
    file_info += "Last Modification Time: " + String(std::ctime(&file_stat.st_mtime));
    file_info += "Last Inode Change Time: " + String(std::ctime(&file_stat.st_ctime));

    return file_info;
}
void FileBrowserApp::openFile(String path) {
    if (path.endsWith(".rom") || path.endsWith(".nes")) {
        AppManager::getInstance()->runApp(new NesApp(path));
    } else if (path.endsWith(".bin")) {
#if LILKA_VERSION < 2
        alertDraw("Помилка", "Ця операція потребує Лілку 2.0");
        return;
#else
        lilka::ProgressDialog dialog("Завантаження", path + "\n\nПочинаємо...");
        dialog.draw(canvas);
        queueDraw();
        int error;
        error = lilka::multiboot.start(path);
        if (error) {
            alertDraw("Помилка", String("Етап: 1\nКод: ") + error);
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
            alertDraw("Помилка", String("Етап: 2\nКод: ") + error);
            return;
        }
        error = lilka::multiboot.finishAndReboot();
        if (error) {
            alertDraw("Помилка", String("Етап: 3\nКод: ") + error);
            return;
        }
#endif
    } else if (path.endsWith(".lua")) {
        AppManager::getInstance()->runApp(new LuaFileRunnerApp(path));
    } else if (path.endsWith(".js")) {
        AppManager::getInstance()->runApp(new MJSApp(path));
    } else {
        // Probably better to use info?
        // Get file size
        struct stat fileStat;
        if (stat(path.c_str(), &fileStat) == 0) {
            alertDraw(
                path,
                String("Розмір:\n") + getHumanReadableSize(fileStat.st_size) + " або " + fileStat.st_size + " байт"
            );
        } else alertDraw(path, String("Сталася помилка при визначенні розміру файлу"));
    }
}
const menu_icon_t* FileBrowserApp::getFileIcon(const String& filename) {
    // Use
    if (filename.endsWith(".rom") || filename.endsWith(".nes")) {
        return &nes;
    } else if (filename.endsWith(".bin")) {
        return &bin;
    } else if (filename.endsWith(".lua")) {
        return &lua;
    } else if (filename.endsWith(".js")) {
        return &js;
    } else {
        return &normalfile;
    }
}
const uint16_t FileBrowserApp::getFileColor(const String& filename) {
    if (filename.endsWith(".rom") || filename.endsWith(".nes")) {
        return lilka::display.color565(255, 128, 128);
    } else if (filename.endsWith(".bin")) {
        return lilka::display.color565(128, 255, 128);
    } else if (filename.endsWith(".lua")) {
        return lilka::display.color565(128, 128, 255);
    } else if (filename.endsWith(".js")) {
        return lilka::display.color565(255, 200, 128);
    } else {
        return lilka::display.color565(200, 200, 200);
    }
}
void FileBrowserApp::alertDraw(const String title, const String message) {
    lilka::Alert alert(title, message);
    alert.draw(canvas);
    queueDraw();
    while (!alert.isFinished()) {
        alert.update();
        taskYIELD();
    }
}

void FileBrowserApp::menuDraw(lilka::Menu& menu) {
    while (!menu.isFinished()) {
        menu.update();
        menu.draw(canvas);
        queueDraw();
    }
}

void FileBrowserApp::openPath(const String& path) {
    size_t fileCount = 0;
    // openddir accepts only path which ends with /
    const String fPath = path.c_str()[path.length() - 1] == '/' ? path : path + "/";

    currentDir = opendir(fPath.c_str());

    const struct dirent* entry = 0;
    if (!currentDir) {
        alertDraw("Помилка", strerror(errno));
        return;
    }
    // Directory open success
    currentPath = fPath;
    // Counting items in directory
    while ((entry = readdir(currentDir)) != NULL) {
        String fileName = entry->d_name;
        // Skip some specific files
        if (fileName == ".." || fileName == ".") continue;

        fileCount++;
    }
    // Allocating memory for MenuEntries;
    dirent* entries = new dirent[fileCount];
    std::unique_ptr<dirent[]> entriesPTR(entries);
    // TODO: Reuse item instead of infinity creation/deletion of it
    lilka::Menu menu("Path: " + path);
    // Returning to directory begining
    rewinddir(currentDir);
    int i = 0;
    while ((entry = readdir(currentDir)) != NULL) {
        const String fileName = entry->d_name;
        const String fullPath = fPath + fileName;
        // Skip some specific files
        if (fileName == "." || fileName == "..") continue;
        struct stat fileStat;
        // At this point zero means all good
        // copying entry to entries
        memcpy(entries + i, entry, sizeof(dirent));
        if (stat(fullPath.c_str(), &fileStat) == 0) {
            // NOTE: There's also additional interesting data we could retrieve
            size_t fileSize = fileStat.st_size;
            String humanReadableFileSize = getHumanReadableSize(fileSize);

            const menu_icon_t* icon = entries[i].d_type == DT_DIR ? &folder : getFileIcon(fileName);
            const uint16_t color =
                entries[i].d_type == DT_DIR ? lilka::display.color565(255, 255, 200) : getFileColor(fileName);
            if (entries[i].d_type == DT_DIR) {
                menu.addItem(fileName, icon, color);
            } else {
                menu.addItem(fileName, icon, color, humanReadableFileSize);
            }
        }
        i++;
    }
    closedir(currentDir);
    menu.addItem("<< Назад", 0, 0);
    // TODO: There's a reason to create function which could work with
    // multiple buttons definition in a form setActivationButton(lilka::Button::A|lilka::Button::B|lilka::Button::D)
    menu.addActivationButton(lilka::Button::D);
    menu.addActivationButton(lilka::Button::C);
    // Not really needed cuz it's allready added, but just in case
    menu.addActivationButton(lilka::Button::A);
    while (1) {
        menuDraw(menu);
        int16_t index = menu.getCursor();
        if (index == fileCount) break;
        String fullFilePath = fPath + entries[index].d_name;
        if (menu.getButton() == lilka::Button::A) {
            // Open
            if (entries[index].d_type == DT_DIR) {
                openPath(fullFilePath);
            } else {
                openFile(fullFilePath);
            }
        } else if (menu.getButton() == lilka::Button::D) {
            // Back
            break;
        } else if (menu.getButton() == lilka::Button::C) {
            // Info
            // TODO: Find a better ui component for this
            alertDraw("Інформація про файл", getFileInfo(fPath + entries->d_name));
        }
    }
}
void FileBrowserApp::setPath(const String path) {
    currentPath = path;
}
void FileBrowserApp::run() {
    openPath(currentPath);
    // This app immediately exits after
};
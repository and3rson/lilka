#include "filesystem.h"
#include "serial.h"
#include "spi.h"
namespace lilka {
FileSystem::FileSystem() {
    sdfs = &SD;
    spiffs = &SPIFFS;
}

void FileSystem::init() {
    initSD();
    initSPIFFS();
}

void FileSystem::initSPIFFS() {
    serial_log("initializing SPIFFS");
    if (!SPIFFS.begin(false, LILKA_SPIFFS_ROOT)) {
        serial_err("failed to initialize SPIFFS");
    } else {
        serial_log("SPIFFS ok");
    }
}

void FileSystem::initSD() {
    // check if LILKA_SDROOT pathable, if not perform init

    serial_log("initializing SD card");

#if LILKA_SDCARD_CS < 0
    serial_err("SD init failed: no CS pin");
#else
    sdfs->begin(LILKA_SDCARD_CS, SPI1, 20000000, LILKA_SDROOT); // TODO: is 20 MHz OK for all cards?
    sdcard_type_t cardType = sdfs->cardType();

    if (cardType == CARD_NONE) {
        serial_err("no SD card found");
        return;
    }

    if (cardType == CARD_SD || cardType == CARD_SDHC) {
        serial_log(
            "card type: %s, card size: %ld MB", cardType == CARD_SD ? "SD" : "SDHC", sdfs->totalBytes() / (1024 * 1024)
        );
    } else {
        serial_err("unknown SD card type: %d", cardType);
    }
#endif
}

uint32_t FileSystem::getEntryCount(const String& path) {
    FS* fs = getFSClassByPath(path);

    if (fs == NULL) {
        serial_err("Path (%s) reffers to unknown filesystem type", path.c_str());
        return 0;
    }

    File root = fs->open(getRelativePath(stripPath(path)));
    // Below we assume if folder can't be open then it has zero files
    // Btw we will show this error using serial
    if (!root) {
        serial_err("getEntryCount: failed to open directory: %s", path.c_str());
        return 0;
    }
    if (!root.isDirectory()) {
        serial_err("getEntryCount: not a directory: %s", path.c_str());
        return 0;
    }

    size_t countFiles = 0;
    File file = root.openNextFile();

    while (file) {
        file = root.openNextFile();
        countFiles++;
    }

    root.close();

    return countFiles;
}
String FileSystem::stripPath(const String& path) {
    String striped_path = path;
    while (!striped_path.equals("/") && striped_path.endsWith("/")) {
        // Strip trailing slashes, unless it's the root directory
        striped_path.remove(striped_path.length() - 1);
    }
    return striped_path;
}

size_t FileSystem::listDir(const String& path, Entry entries[]) {
    FS* fs = getFSClassByPath(path);

    if (fs == NULL) {
        serial_err("Path (%s) reffers to unknown filesystem type", path.c_str());
        return 0;
    }

    File root = fs->open(getRelativePath(stripPath(path)));
    if (!root) {
        serial_err("listDir: failed to open directory: %s", path.c_str());
        return -1;
    }
    if (!root.isDirectory()) {
        serial_err("listDir: not a directory: %s", path.c_str());
        return -1;
    }

    int i = 0;
    File file = root.openNextFile();
    while (file) {
        entries[i].name = file.name();
        if (file.isDirectory()) {
            entries[i].type = ENT_DIRECTORY;
        } else {
            entries[i].type = ENT_FILE;
        }
        entries[i].size = file.size();
        file.close();
        file = root.openNextFile();
        i++;
    }
    root.close();
    // Sort filenames, but keep directories first
    qsort(entries, i, sizeof(Entry), [](const void* a, const void* b) -> int {
        const Entry* ea = static_cast<const Entry*>(a);
        const Entry* eb = static_cast<const Entry*>(b);
        if (ea->type == ENT_DIRECTORY && eb->type != ENT_DIRECTORY) {
            return -1;
        } else if (ea->type != ENT_DIRECTORY && eb->type == ENT_DIRECTORY) {
            return 1;
        }
        return ea->name.compareTo(eb->name);
    });
    return i;
}

FS* FileSystem::getFSClassByPath(const String& path) {
    FS* fs = NULL;
    if (path.startsWith(LILKA_SDROOT)) {
        fs = static_cast<FS*>(sdfs);
    } else if (path.startsWith(LILKA_SPIFFS_ROOT)) {
        fs = static_cast<FS*>(spiffs);
    }
    return fs;
}
String FileSystem::getRelativePath(const String& path) {
    String relativePath;
    if (path.startsWith(LILKA_SDROOT)) {
        relativePath = "/" + path.substring(LILKA_SDROOT_LEN);
    } else if (path.startsWith(LILKA_SPIFFS_ROOT)) {
        relativePath = "/" + path.substring(LILKA_SPIFFS_ROOT_LEN);
    } else
        // Maybe path is allready relative?
        relativePath = path;
    return relativePath;
}
bool FileSystem::sdAvailable() {
    return (sdfs->cardType() == CARD_NONE || sdfs->cardType() == CARD_UNKNOWN);
}
FileSystem filesystem;
} // namespace lilka
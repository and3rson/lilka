#include "filesystem.h"

#include "serial.h"

#define LILKA_FSROOT "/fs"

namespace lilka {

Filesystem::Filesystem() : _available(false), fs(NULL) {
}

void Filesystem::begin() {
    serial_log("initializing SPIFFS");
    if (!SPIFFS.begin(false, LILKA_FSROOT)) {
        serial_err("failed to initialize SPIFFS");
        _available = false;
    } else {
        serial_log("SPIFFS ok");
        _available = true;
    }
    fs = &SPIFFS;
}

bool Filesystem::available() {
    return _available;
}

int Filesystem::listDir(String path, Entry entries[]) {
    while (!path.equals("/") && path.endsWith("/")) {
        // Strip trailing slashes, unless it's the root directory
        path.remove(path.length() - 1);
    }
    File root = fs->open(path);
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

size_t Filesystem::getEntryCount(String path) {
    size_t countFiles = 0;

    while (!path.equals("/") && path.endsWith("/")) {
        // Strip trailing slashes, unless it's the root directory
        path.remove(path.length() - 1);
    }
    File root = fs->open(path);
    // Below we assume if folder can't be open then it has zero files
    // Btw we will show this error using serial
    if (!root) {
        serial_err("%s:%d:getEntryCount: failed to open directory: %s", __FILE__, __LINE__, path.c_str());
        return 0;
    }
    if (!root.isDirectory()) {
        serial_err("%s:%d:getEntryCount: not a directory: %s", __FILE__, __LINE__, path.c_str());
        return 0;
    }

    File file = root.openNextFile();

    while (file) {
        file = root.openNextFile();
        countFiles++;
    }

    root.close();

    return countFiles;
}

String Filesystem::abspath(String filename) {
    return String(LILKA_FSROOT) + "/" + filename;
}

Filesystem filesystem;

} // namespace lilka

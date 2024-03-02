#include "filesystem.h"

#include "serial.h"

#define LILKA_FSROOT "/fs"

namespace lilka {

Filesystem::Filesystem() {}

void Filesystem::begin() {
    serial_log("initializing SPIFFS");
    if (!SPIFFS.begin(false, LILKA_FSROOT)) {
        serial_err("failed to initialize SPIFFS");
        _available = false;
    } else {
        serial_log("SPIFFS ok");
        _available = true;
    }
    _filesystem = &SPIFFS;
}

bool Filesystem::available() {
    return _available;
}

int Filesystem::readdir(String filenames[]) {
    File _root = _filesystem->open("/");
    int count = 0;
    File file = _root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            file.close();
            file = _root.openNextFile();
            continue;
        }
        filenames[count++] = file.name();
        file.close();
        file = _root.openNextFile();
    }
    _root.close();
    // Sort filenames
    qsort(filenames, count, sizeof(String), [](const void *a, const void *b) -> int {
        const String *ea = (const String *)a;
        const String *eb = (const String *)b;
        return ea->compareTo(*eb);
    });
    return count;
}

int Filesystem::readdir(String filenames[], String extension) {
    File _root = _filesystem->open("/");
    int count = 0;
    File file = _root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            file.close();
            file = _root.openNextFile();
            continue;
        }
        String name = file.name();
        if (name.endsWith(extension)) {
            filenames[count++] = name;
        }
        file.close();
        file = _root.openNextFile();
    }
    _root.close();
    return count;
}

String Filesystem::abspath(String filename) {
    return String(LILKA_FSROOT) + "/" + filename;
}

Filesystem filesystem;

} // namespace lilka

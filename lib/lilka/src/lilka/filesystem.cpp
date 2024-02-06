#include "filesystem.h"

#define LILKA_FSROOT "/fs"

namespace lilka {

Filesystem::Filesystem() {}

void Filesystem::begin() {
    Serial.print("Initializing filesystem... ");
    SPIFFS.begin(false, LILKA_FSROOT);
    _filesystem = &SPIFFS;
    Serial.println("done");
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

#include "filesystem.h"

#define LILKA_FSROOT "/fs"

FS* _filesystem = 0;
File _root;

FS* lilka_filesystem_begin() {
    SPIFFS.begin(false, LILKA_FSROOT);
    _filesystem = &SPIFFS;
    return _filesystem;
}

FS* lilka_filesystem_get() {
    if (_filesystem == 0) {
        lilka_filesystem_begin();
    }
    return _filesystem;
}

int lilka_filesystem_readdir(String filenames[]) {
    _root = lilka_filesystem_get()->open("/");
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

int lilka_filesystem_readdir(String filenames[], String extension) {
    _root = lilka_filesystem_get()->open("/");
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

String lilka_filesystem_abspath(String filename) {
    return String(LILKA_FSROOT) + "/" + filename;
}

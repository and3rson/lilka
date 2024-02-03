#include "filesystem.h"

#define LILKA_FSROOT "/fs"

FS* _filesystem = 0;

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

#ifndef LILKA_FILESYSTEM_H
#define LILKA_FILESYSTEM_H

#include <SPIFFS.h>

extern FS *_filesystem;

FS *lilka_filesystem_begin();
FS *lilka_filesystem_get();
int lilka_filesystem_readdir(String filenames[]);

#endif // LILKA_FILESYSTEM_H

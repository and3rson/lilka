#ifndef LILKA_FILESYSTEM_H
#define LILKA_FILESYSTEM_H

#include <SPIFFS.h>
#include "fsdefs.h"
namespace lilka {
class Filesystem {
public:
    Filesystem();
    void begin();
    bool available();

    int listDir(String path, Entry entries[]);
    size_t getEntryCount(String path);
    String abspath(String filename);

private:
    FS* fs;
    bool _available;
};

extern Filesystem filesystem;

} // namespace lilka

#endif // LILKA_FILESYSTEM_H

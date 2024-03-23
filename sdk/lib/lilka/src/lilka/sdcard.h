#ifndef LILKA_SDCARD_H
#define LILKA_SDCARD_H

#include <SD.h>

#include "config.h"

namespace lilka {

typedef enum {
    ENT_FILE,
    ENT_DIRECTORY,
} EntryType;

typedef struct {
    String name;
    EntryType type;
    size_t size;
} Entry;

class SDCard {
public:
    SDCard();
    void begin();
    bool available();
    SDFS* fs;

    int listDir(String path, Entry entries[]);
    size_t getEntryCount(String path);
    String abspath(String path);
};

extern SDCard sdcard;

} // namespace lilka

#endif // LILKA_SDCARD_H

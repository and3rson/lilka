#ifndef LILKA_SDCARD_H
#define LILKA_SDCARD_H

#include <SD.h>

#include "config.h"

namespace lilka {

typedef enum {
    FILE,
    DIRECTORY,
} EntryType;

typedef struct {
    String name;
    EntryType type;
} Entry;

class SDCard {
public:
    SDCard();
    void begin();
    bool available();
    SDFS *fs;

    int listDir(String path, Entry entries[]);
};

extern SDCard sdcard;

} // namespace lilka

#endif // LILKA_SDCARD_H

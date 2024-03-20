#ifndef LILKA_SDCARD_H
#define LILKA_SDCARD_H

#include <SD.h>

#include "config.h"
#include "fsdefs.h"
namespace lilka {

class SDCard {
public:
    SDCard();
    void begin();
    bool available();

    int listDir(String path, Entry entries[]);
    size_t getEntryCount(String path);
    String abspath(String path);

private:
    SDFS* fs;
};

extern SDCard sdcard;

} // namespace lilka

#endif // LILKA_SDCARD_H

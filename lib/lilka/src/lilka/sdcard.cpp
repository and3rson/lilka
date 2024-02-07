#include "sdcard.h"

#include "serial.h"

namespace lilka {

SDCard::SDCard() {
    fs = &SD;
}

void SDCard::begin() {
    serial_log("initializing SD card");

#if LILKA_SDCARD_CS < 0
    serial_log("SD init failed: no CS pin");
#else
    fs->begin(LILKA_SDCARD_CS, SPI, 1000000);
    sdcard_type_t cardType = fs->cardType();

    if (cardType == CARD_NONE) {
        serial_log("no SD card found");
        return;
    }

    if (cardType == CARD_SD || cardType == CARD_SDHC) {
        serial_log("card type: %s, card size: %d", cardType == CARD_SD ? "SD" : "SDHC", fs->totalBytes());
    } else {
        serial_log("unknown SD card type: %d", cardType);
    }
#endif
}

bool SDCard::available() {
    return fs->cardType() == CARD_SD || fs->cardType() == CARD_SDHC;
}

int SDCard::listDir(String path, Entry entries[]) {
    File root = fs->open(path);
    if (!root) {
        serial_log("listDir: failed to open directory: %s", path.c_str());
        return -1;
    }
    if (!root.isDirectory()) {
        serial_log("listDir: not a directory: %s", path.c_str());
        return -1;
    }

    int i = 0;
    File file = root.openNextFile();
    while (file) {
        entries[i].name = file.name();
        if (file.isDirectory()) {
            entries[i].type = DIRECTORY;
        } else {
            entries[i].type = FILE;
        }
        file.close();
        file = root.openNextFile();
        i++;
    }
    root.close();
    return i;
}

SDCard sdcard;

} // namespace lilka

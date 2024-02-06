#include "sdcard.h"

namespace lilka {

SDCard::SDCard() {
    fs = &SD;
}

void SDCard::begin() {
    Serial.print("Initializing SD card... ");

#if LILKA_SDCARD_CS < 0
    Serial.println("failed: no CS pin");
#else
    fs->begin(LILKA_SDCARD_CS, SPI, 1000000);
    Serial.print("done, SD card type: ");
    switch (fs->cardType()) {
        case CARD_NONE:
            Serial.println("None");
            break;
        case CARD_SD:
            Serial.print("SD, ");
            break;
        case CARD_SDHC:
            Serial.print("SDHC, ");
            break;
        default:
            Serial.println("Unknown");
            break;
    }
    if (fs->cardType() == CARD_SD || fs->cardType() == CARD_SDHC) {
        Serial.print("SD card size: ");
        Serial.print(fs->totalBytes());
        Serial.println(" bytes");
    }
#endif
}

bool SDCard::available() {
    return fs->cardType() == CARD_SD || fs->cardType() == CARD_SDHC;
}

int SDCard::listDir(String path, Entry entries[]) {
    File root = fs->open(path);
    if (!root) {
        Serial.print("listDir: failed to open directory: ");
        Serial.println(path);
        return -1;
    }
    if (!root.isDirectory()) {
        Serial.print("listDir: not a directory: ");
        Serial.println(path);
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
    // for (int i = 0; i < 1024; i++) {
    //     SPI.transfer(0xAA); // http://elm-chan.org/docs/mmc/mmc_e.html#spibus
    //     // SPI.write(0xAA); // http://elm-chan.org/docs/mmc/mmc_e.html#spibus
    // }
    return i;
}

SDCard sdcard;

} // namespace lilka

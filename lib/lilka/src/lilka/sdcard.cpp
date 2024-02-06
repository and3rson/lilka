#include "sdcard.h"

namespace lilka {

SDCard::SDCard() {
    fs = &SD;
}

void SDCard::begin() {
    Serial.print("Initializing SD card... ");
#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
    SPIClass spi(HSPI);
#elif CONFIG_IDF_TARGET_ESP32C3
    SPIClass spi(FSPI);
#endif

#if SDCARD_CS < 0
    Serial.println("failed: no CS pin");
#else
    fs->begin(SDCARD_CS, spi);
    Serial.println("done");
#endif
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
    return i;
}

SDCard sdcard;

} // namespace lilka

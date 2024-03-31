#include "sdcard.h"

#include "serial.h"
#include "spi.h"

#define LILKA_SDROOT "/sd"

namespace lilka {

SDCard::SDCard() {
    fs = &SD;
}

void SDCard::begin() {
    serial_log("initializing SD card");

#if LILKA_SDCARD_CS < 0
    serial_err("SD init failed: no CS pin");
#else
    fs->begin(LILKA_SDCARD_CS, SPI1, 25000000, LILKA_SDROOT); // TODO: is 25 MHz OK for all cards?
    sdcard_type_t cardType = fs->cardType();

    if (cardType == CARD_NONE) {
        serial_err("no SD card found");
        return;
    }

    if (cardType == CARD_SD || cardType == CARD_SDHC) {
        serial_log(
            "card type: %s, card size: %ld MB", cardType == CARD_SD ? "SD" : "SDHC", fs->totalBytes() / (1024 * 1024)
        );
    } else {
        serial_err("unknown SD card type: %d", cardType);
    }
#endif
}

bool SDCard::available() {
    if (fs->cardType() != CARD_SD && fs->cardType() != CARD_SDHC) {
        // Attempt to reinitialize the card
        fs->end();
        begin();
        return fs->cardType() == CARD_SD || fs->cardType() == CARD_SDHC;
    }
    return true;
}

int SDCard::listDir(String path, Entry entries[]) {
    while (!path.equals("/") && path.endsWith("/")) {
        // Strip trailing slashes, unless it's the root directory
        path.remove(path.length() - 1);
    }
    File root = fs->open(path);
    if (!root) {
        serial_err("listDir: failed to open directory: %s", path.c_str());
        return -1;
    }
    if (!root.isDirectory()) {
        serial_err("listDir: not a directory: %s", path.c_str());
        return -1;
    }

    int i = 0;
    File file = root.openNextFile();
    while (file) {
        entries[i].name = file.name();
        if (file.isDirectory()) {
            entries[i].type = ENT_DIRECTORY;
        } else {
            entries[i].type = ENT_FILE;
        }
        entries[i].size = file.size();
        file.close();
        file = root.openNextFile();
        i++;
    }
    root.close();
    // Sort filenames, but keep directories first
    qsort(entries, i, sizeof(Entry), [](const void* a, const void* b) -> int {
        const Entry* ea = static_cast<const Entry*>(a);
        const Entry* eb = static_cast<const Entry*>(b);
        if (ea->type == ENT_DIRECTORY && eb->type != ENT_DIRECTORY) {
            return -1;
        } else if (ea->type != ENT_DIRECTORY && eb->type == ENT_DIRECTORY) {
            return 1;
        }
        return ea->name.compareTo(eb->name);
    });
    return i;
}

size_t SDCard::getEntryCount(String path) {
    size_t countFiles = 0;

    while (!path.equals("/") && path.endsWith("/")) {
        // Strip trailing slashes, unless it's the root directory
        path.remove(path.length() - 1);
    }
    File root = fs->open(path);
    // Below we assume if folder can't be open then it has zero files
    // Btw we will show this error using serial
    if (!root) {
        serial_err("%s:%d:getEntryCount: failed to open directory: %s", __FILE__, __LINE__, path.c_str());
        return 0;
    }
    if (!root.isDirectory()) {
        serial_err("%s:%d:getEntryCount: not a directory: %s", __FILE__, __LINE__, path.c_str());
        return 0;
    }

    File file = root.openNextFile();

    while (file) {
        file = root.openNextFile();
        countFiles++;
    }

    root.close();

    return countFiles;
}

String SDCard::abspath(String filename) {
    while (filename.length() > 0 && filename[0] == '/') {
        filename = filename.substring(1);
    }
    return String(LILKA_SDROOT) + "/" + filename;
}

SDCard sdcard;

} // namespace lilka

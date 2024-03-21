#include "automount.h"
bool AutoMountService::SPIFFSInit() {
    lilka::serial_log("initializing SPIFFS");
    if (!SPIFFS.begin(false, LILKA_SPIFFS_ROOT)) {
        lilka::serial_err("failed to initialize SPIFFS");
        spiffsAvaliable = false;
    } else {
        lilka::serial_log("SPIFFS ok");
        spiffsAvaliable = true;
    }
    // _filesystem = &SPIFFS;
    return spiffsAvaliable;
}
bool AutoMountService::SDInit() {
    lilka::serial_log("initializing SD card");

#if LILKA_SDCARD_CS < 0
    lilka::serial_err("SD init failed: no CS pin");
    sdAvaliable = false;
#else
    SD.begin(LILKA_SDCARD_CS, lilka::SPI1, 20000000, LILKA_SD_ROOT); // TODO: is 20 MHz OK for all cards?
    sdType = SD.cardType();

    if (sdType == CARD_NONE) {
        lilka::serial_err("no SD card found");
        sdAvaliable = false;
    }
    // here we added ability to use CARD_MMC
    if (sdType == CARD_SD || sdType == CARD_SDHC || CARD_MMC) {
        lilka::serial_log(
            "card type: %s, card size: %ld MB", sdType == CARD_SD ? "SD" : "SDHC", SD.totalBytes() / (1024 * 1024)
        );
        sdAvaliable = true;
    } else {
        lilka::serial_err("unknown SD card type: %d", sdType);
    }
#endif
    return sdAvaliable;
}

void AutoMountService::run() {
    while (1) {
        // if sdType == CARD_UNKNOWN
        // probably sd has a wrong filesystem type...
        if (sdType == CARD_NONE) {
            // try to mount if no card present
            SDInit();
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

AutoMountService::AutoMountService() :
    Service("automount"), sdAvaliable(false), spiffsAvaliable(false), sdType(CARD_NONE) {
    SPIFFSInit();
    SDInit();
}

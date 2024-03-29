#include "automount.h"
void AutoMountService::SDInit() {
    lilka::filesystem.initSD();
}

void AutoMountService::run() {
    while (1) {
        // if sdType == CARD_UNKNOWN
        // probably sd has a wrong filesystem type...
        if (lilka::filesystem.sdAvailable()) {
            // try to mount if no card present
            SDInit();
        }
        // 200 msec should be good
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

AutoMountService::AutoMountService() : Service("automount") {
    SDInit();
}

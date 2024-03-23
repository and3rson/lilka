#include "watchdog.h"

WatchdogService::WatchdogService() : Service("watchdog") {
}

void WatchdogService::run() {
    while (1) {
        // Get free RAM
        Serial.println("WatchdogService: Free RAM: " + String(ESP.getFreeHeap()));
        // Get free stack memory
        Serial.println("WatchdogService: Free stack: " + String(uxTaskGetStackHighWaterMark(NULL)));
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

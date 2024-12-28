

#include "KeiraBLEService.h"
#include "lilka/ble_server.h"
KeiraBLEService::KeiraBLEService() : Service("clock") {
    lilka::BLE_server.create_service("1234");
    lilka::BLE_server.create_new_characteristics("1234", "2345", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
}

void KeiraBLEService::run() {
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
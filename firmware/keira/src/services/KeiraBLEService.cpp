

#include "KeiraBLEService.h"
#include "lilka/ble_server.h"
#include "lilka.h"

KeiraBLEService::KeiraBLEService() : Service("clock") {
    lilka::serial_log("Start service and char");
    lilka::BLE_server.create_service("1234");
    lilka::BLE_server.create_new_characteristics("1234", "2345", NIMBLE_PROPERTY::READ);
}

void KeiraBLEService::run() {
    while (1) {
        int battery_level = lilka::battery.readLevel();
        lilka::BLE_server.write_characteristics("1234", "2345", std::to_string(battery_level));
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
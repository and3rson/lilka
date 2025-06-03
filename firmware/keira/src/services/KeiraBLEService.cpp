

#include "KeiraBLEService.h"
#include "lilka/ble_server.h"
#include "lilka.h"

KeiraBLEService::KeiraBLEService() : Service("clock") {
    lilka::serial.log("Start service and char");
    lilka::BLE_server.create_service("1234");
    lilka::BLE_server.create_new_characteristics("1234", "2345", NIMBLE_PROPERTY::READ);
    lilka::BLE_server.create_new_characteristics("1234", "3456", NIMBLE_PROPERTY::WRITE);
}

void KeiraBLEService::run() {
    while (1) {
        lilka::serial.log(lilka::BLE_server.getConnectionStatus() ? "connected" : "not connected");

        int battery_level = lilka::battery.readLevel();
        lilka::BLE_server.write_characteristics("1234", "2345", std::to_string(battery_level));

        std::string value_from_char = lilka::BLE_server.read_characteristics("1234", "3456");
        lilka::serial.log(value_from_char.c_str());

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
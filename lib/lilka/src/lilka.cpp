#include <esp_wifi.h>

#include "lilka.h"

namespace lilka {

void begin() {
    serial_begin();
    display.begin(); // Must be initialized BEFORE SD card
    sdcard.begin();
    controller.begin(); // Must be initialized AFTER SD card (since SD card somehow messes with GPIO)
    filesystem.begin();
    esp_wifi_deinit();
}

} // namespace lilka

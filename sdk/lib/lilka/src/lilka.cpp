#include <esp_wifi.h>

#include "lilka.h"

namespace lilka {

void begin() {
    serial_begin();
    multiboot.begin();
    spi_begin();
    buzzer.begin(); // Play notification sound
    display.begin(); // Must be initialized BEFORE SD card
    sdcard.begin();
    controller.begin(); // Must be initialized AFTER SD card (since SD card somehow messes with GPIO)
    filesystem.begin();
    battery.begin();
    // TODO: I2S
    esp_wifi_deinit();
}

} // namespace lilka

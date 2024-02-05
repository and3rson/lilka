#include <esp_wifi.h>

#include "lilka.h"

namespace lilka {

void begin() {
    serial_begin();
    display.begin();
    controller.begin();
    filesystem.begin();
    esp_wifi_deinit();
}

} // namespace lilka

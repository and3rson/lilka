#include <esp_wifi.h>

#include "lilka.h"

namespace lilka {

void begin() {
    serial_begin();
    display.begin();
    controller.begin();
    // lilka_input_begin();
    // lilka::display.begin();
    // lilka_filesystem_begin();
    esp_wifi_deinit();
}

} // namespace lilka

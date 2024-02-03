#include <esp_wifi.h>

#include "lilka.h"

void lilka_begin() {
    lilka_serial_begin();
    lilka_input_begin();
    lilka_display_begin();
    lilka_filesystem_begin();
    esp_wifi_deinit();
}

#include <esp_ota_ops.h>

#include "loader.h"
#include "sdcard.h"
#include "serial.h"
#include "stdio.h"

namespace lilka {

extern SDCard sdcard;

void Loader::begin() {}

int Loader::execute(String path) {
    // Завантаження прошивки з microSD-картки.
    if (!sdcard.available()) {
        serial_err("SD card not available");
        return -1;
    }

    String abspath = sdcard.abspath(path);

    // TODO: Use sdcard instead of SD
    File file = SD.open(abspath.c_str(), FILE_READ);
    if (!file) {
        serial_err("Failed to open file: %s", abspath.c_str());
        return -2;
    }

    esp_ota_handle_t ota_handle;
    const esp_partition_t *ota_partition = esp_ota_get_next_update_partition(NULL); // get ota1 (we're in ota0 now)
    if (ota_partition == NULL) {
        serial_err("Failed to get next OTA partition");
        return -3;
    }

    esp_err_t err = esp_ota_begin(ota_partition, file.size(), &ota_handle);
    if (err != ESP_OK) {
        serial_err("Failed to begin OTA: %d", err);
        return -4;
    }

    size_t written = 0;
    char buf[1024];
    while (true) {
        int len = file.readBytes(buf, sizeof(buf));
        if (len == 0) {
            break;
        }

        err = esp_ota_write(ota_handle, buf, len);
        if (err != ESP_OK) {
            serial_err("Failed to write OTA: %d", err);
            return -5;
        }

        written += len;

        if (written % (1024 * 65536) == 0) {
            serial_log("Written %d bytes", written);
        }
    }

    err = esp_ota_end(ota_handle);
    if (err != ESP_OK) {
        serial_err("Failed to end OTA: %d", err);
        return -6;
    }

    serial_log("Written %d bytes", written);

    // Перевстановлення активного розділу на OTA-розділ (його буде запущено лише один раз, після чого активним залишиться основний розділ).
    err = esp_ota_set_boot_partition(ota_partition);
    if (err != ESP_OK) {
        serial_err("Failed to set boot partition: %d", err);
        return -7;
    }

    // Запуск нової прошивки.
    esp_restart();
}

Loader loader;

} // namespace lilka

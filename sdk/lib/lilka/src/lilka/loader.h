/*
 * Завантажувач прошивок з microSD-картки.
 *
 * Дозволяє прочитати файл прошивки з microSD-картки в OTA-розділ та запустити його,
 * не замінюючи основну прошивку.
 * Нова прошивка зберігається в OTA-розділі та запускається при наступному включенні,
 * але активною залишається основна прошивка (app rollback).
 *
 * Детальніше про auto-rollback: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/ota.html#rollback-process
 */

#ifndef LILKA_LOADER_H
#define LILKA_LOADER_H

#include <esp_ota_ops.h>
#include <Arduino.h>

#include "sdcard.h"

namespace lilka {

class LoaderHandle {
public:
    LoaderHandle(String path);

    int start();
    int process();
    int getBytesWritten();
    int getBytesTotal();
    int finishAndReboot();

private:
    String path;
    FILE *file;
    esp_ota_handle_t ota_handle;
    const esp_partition_t *ota_partition;
    int bytesWritten;
    int bytesTotal;
};

class Loader {
public:
    void begin();

    LoaderHandle *createHandle(String path);
};

extern Loader loader;

} // namespace lilka

#endif // LILKA_LOADER_H

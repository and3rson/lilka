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

#include <Arduino.h>

namespace lilka {

class Loader {
public:
    void begin();
    int execute(String path);
};

extern Loader loader;

} // namespace lilka

#endif // LILKA_LOADER_H

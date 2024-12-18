#include <esp_wifi.h>
#include <esp_task_wdt.h>

#include "lilka.h"

namespace lilka {

void begin() {
    serial.begin();
    board.begin();
    multiboot.begin();
    spi_begin();
    buzzer.begin(); // Play notification sound
    audio.begin();
    display.begin(); // Must be initialized BEFORE SD card
    fileutils.begin();
    controller.begin(); // Must be initialized AFTER SD card (since SD card somehow messes with GPIO)

    battery.begin();
    // TODO: I2S
    // TODO: Delete Task Watchdog Timer - we'll be running long tasks
    // TODO: Maybe keep it?
    TaskHandle_t idle_0 = xTaskGetIdleTaskHandleForCPU(0);
    if (idle_0 != NULL) {
        esp_task_wdt_delete(idle_0);
    }
#if LILKA_VERSION > 1
    TaskHandle_t idle_1 = xTaskGetIdleTaskHandleForCPU(1);
    if (idle_1 != NULL) {
        esp_task_wdt_delete(idle_1);
    }
#endif
}

} // namespace lilka

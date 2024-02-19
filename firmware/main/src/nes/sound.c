/*
 * Buzzer part start rewrite from: https://github.com/moononournation/esp_8_bit.git
 */
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include <driver/i2s.h>
#include <soc/ledc_struct.h>
#include <esp32-hal-timer.h>

#include <nes/nes.h>

#include "hw_config.h"

int osd_init_sound() {
    return 0;
}

void osd_stopsound() {}

void do_audio_frame() {}

void osd_setsound(void (*playfunc)(void *buffer, int length)) {}

void osd_getsoundinfo(sndinfo_t *info) {
    // dummy value
    info->sample_rate = 22050;
    info->bps = 16;
}

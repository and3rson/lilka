#include "audio.h"
#include "config.h"
#include "serial.h"
#include "ping.h"

namespace lilka {

Audio::Audio() {
}

void ping_task(void* arg);

void Audio::begin() {
#if LILKA_VERSION == 1
    serial_err("Audio not supported in this version of lilka. Try to use Buzzer instead");
#elif LILKA_VERSION == 2
    I2S.setAllPins(LILKA_I2S_BCLK, LILKA_I2S_LRCK, LILKA_I2S_DOUT, LILKA_I2S_DOUT, -1);

    I2S.begin(I2S_PHILIPS_MODE, 22050, 16);

    xTaskCreatePinnedToCore(ping_task, "ping_task", 4096, NULL, 1, NULL, 0);
#endif
}

void ping_task(void* arg) {
#if LILKA_VERSION == 1
    serial_err("This part of code should never be called. Audio not supported for this version of lilka");
#elif LILKA_VERSION == 2
    // Signed 16-bit PCM
    const int16_t* ping = reinterpret_cast<const int16_t*>(ping_raw);

    for (int i = 0; i < ping_raw_size / 2; i++) {
        I2S.write(ping[i] >> 2);
        I2S.write(ping[i] >> 2);
    }

    I2S.end();

    vTaskDelete(NULL);
#endif
}

Audio audio;

} // namespace lilka

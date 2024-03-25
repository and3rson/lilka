#include "audio.h"
#include "config.h"
#include "ping.h"

namespace lilka {

Audio::Audio() {
}

void ping_task(void* arg);

void Audio::begin() {
    I2S.setAllPins(LILKA_I2S_BCLK, LILKA_I2S_LRCK, LILKA_I2S_DOUT, LILKA_I2S_DOUT, -1);

    I2S.begin(I2S_PHILIPS_MODE, 22050, 16);

    xTaskCreatePinnedToCore(ping_task, "ping_task", 4096, NULL, 1, NULL, 0);
}

void ping_task(void* arg) {
    // Signed 16-bit PCM
    const int16_t* ping = reinterpret_cast<const int16_t*>(ping_raw);

    for (int i = 0; i < ping_raw_size / 2; i++) {
        I2S.write(ping[i]);
        I2S.write(ping[i]);
    }

    I2S.end();

    vTaskDelete(NULL);
}

Audio audio;

} // namespace lilka

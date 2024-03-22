#include "audio.h"
#include "hi.h"

namespace lilka {

Audio::Audio() {
}

void hi_task(void* arg);

void Audio::begin() {
    I2S.begin(I2S_PHILIPS_MODE, 22050, 16);

    // Signed 8-bit PCM
    // for (int i = 0; i < hi_raw_size; i++) {
    //     I2S.write(hi_raw[i]);
    //     I2S.write(hi_raw[i]);
    // }

    xTaskCreatePinnedToCore(hi_task, "hi_task", 4096, NULL, 1, NULL, 0);
}

void hi_task(void* arg) {
    // Signed 16-bit PCM
    const int16_t* hi = reinterpret_cast<const int16_t*>(hi_raw);

    for (int i = 0; i < hi_raw_size / 2; i++) {
        I2S.write(hi[i]);
        I2S.write(hi[i]);
    }

    vTaskDelete(NULL);
}

Audio audio;

// I2SClass i2s(0, 0, LILKA_I2S_DOUT, LILKA_I2S_BCLK, LILKA_I2S_LRCK);

} // namespace lilka

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

    initPins();

    I2S.setAllPins(LILKA_I2S_BCLK, LILKA_I2S_LRCK, LILKA_I2S_DOUT, LILKA_I2S_DOUT, -1);

    xTaskCreatePinnedToCore(ping_task, "ping_task", 4096, NULL, 1, NULL, 0);
#endif
}

void Audio::initPins() {
    // Set up I2S pins globally
    constexpr uint8_t pinCount = 3;
    uint8_t pins[pinCount] = {LILKA_I2S_BCLK, LILKA_I2S_LRCK, LILKA_I2S_DOUT};
    uint8_t funcs[pinCount] = {I2S0O_BCK_OUT_IDX, I2S0O_WS_OUT_IDX, I2S0O_SD_OUT_IDX};
    for (int i = 0; i < pinCount; i++) {
        gpio_pad_select_gpio(pins[i]);
        gpio_set_direction((gpio_num_t)pins[i], GPIO_MODE_OUTPUT);
        gpio_matrix_out(pins[i], funcs[i], false, false);
    }
}

void ping_task(void* arg) {
#if LILKA_VERSION == 1
    serial_err("This part of code should never be called. Audio not supported for this version of lilka");
#elif LILKA_VERSION == 2
    // Signed 16-bit PCM
    // const int16_t* ping = reinterpret_cast<const int16_t*>(ping_raw);

    // I2S.begin(I2S_PHILIPS_MODE, 22050, 16);
    // for (int i = 0; i < ping_raw_size / 2; i++) {
    //     // TODO: Should use i2s_write & DMA
    //     I2S.write(ping[i] >> 2);
    //     I2S.write(ping[i] >> 2);
    // }
    // I2S.end();

    vTaskDelete(NULL);
#endif
}

Audio audio;

} // namespace lilka

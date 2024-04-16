#include <lilka.h>
#include "synth.h"

#include "i2s_sink.h"

I2SSink::I2SSink() {
    constexpr uint8_t pinCount = 3;
    uint8_t pins[pinCount] = {LILKA_I2S_BCLK, LILKA_I2S_LRCK, LILKA_I2S_DOUT};
    uint8_t funcs[pinCount] = {I2S0O_BCK_OUT_IDX, I2S0O_WS_OUT_IDX, I2S0O_SD_OUT_IDX};
    for (int i = 0; i < pinCount; i++) {
        gpio_pad_select_gpio(pins[i]);
        gpio_set_direction((gpio_num_t)pins[i], GPIO_MODE_OUTPUT);
        gpio_matrix_out(pins[i], funcs[i], false, false);
    }

    esp_i2s::i2s_config_t cfg = {
        .mode = (esp_i2s::i2s_mode_t)(esp_i2s::I2S_MODE_MASTER | esp_i2s::I2S_MODE_TX),
        .sample_rate = SYNTH_SAMPLE_RATE,
        .bits_per_sample = esp_i2s::I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = esp_i2s::I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format =
            (esp_i2s::i2s_comm_format_t)(esp_i2s::I2S_COMM_FORMAT_STAND_I2S | esp_i2s::I2S_COMM_FORMAT_STAND_MSB),
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = SYNTH_BUFFER_SIZE,
        .use_apll = false,
        .tx_desc_auto_clear = true,
    };
    if (esp_i2s::i2s_driver_install(esp_i2s::I2S_NUM_0, &cfg, 0, NULL) != ESP_OK) {
        lilka::serial_err("Failed to install I2S driver");
        return;
    }
    if (esp_i2s::i2s_zero_dma_buffer(esp_i2s::I2S_NUM_0) != ESP_OK) {
        lilka::serial_err("Failed to zero I2S DMA buffer");
        return;
    }
}

I2SSink::~I2SSink() {
    // Deinit I2S (free DMA buffers & uninstall driver)
    // TODO: Seems like this does not free the DMA buffers and causes a memory leak!
    if (esp_i2s::i2s_driver_uninstall(esp_i2s::I2S_NUM_0) != ESP_OK) {
        lilka::serial_err("Failed to uninstall I2S driver");
    }
}

void I2SSink::start() {
}

size_t I2SSink::write(const int16_t* data, size_t size) {
    size_t bytesWritten = 0;
    esp_i2s::i2s_write(esp_i2s::I2S_NUM_0, data, size * sizeof(int16_t), &bytesWritten, portMAX_DELAY);
    return bytesWritten / sizeof(int16_t);
}

void I2SSink::stop() {
}

#include <lilka.h>
#include "mixer.h"
#include "pattern.h"

class Acquire {
public:
    Acquire(SemaphoreHandle_t xMutex) : xMutex(xMutex) {
        xSemaphoreTake(xMutex, portMAX_DELAY);
    }
    ~Acquire() {
        xSemaphoreGive(xMutex);
    }

private:
    SemaphoreHandle_t xMutex;
};

typedef struct {
    Pattern* pattern;
    int32_t eventIndex;
} play_request_t;

#define SAMPLE_RATE 8000

Mixer::Mixer() : xMutex(xSemaphoreCreateMutex()), xQueue(xQueueCreate(1, sizeof(play_request_t))) {
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
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = esp_i2s::I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = esp_i2s::I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format =
            (esp_i2s::i2s_comm_format_t)(esp_i2s::I2S_COMM_FORMAT_STAND_I2S | esp_i2s::I2S_COMM_FORMAT_STAND_MSB),
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 2,
        .dma_buf_len = MIXER_BUFFER_SIZE,
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

    xTaskCreatePinnedToCore(
        [](void* pvParameters) {
            static_cast<Mixer*>(pvParameters)->mixerTask();
            vTaskDelete(NULL);
        },
        "sequencerTask",
        4096,
        this,
        1,
        nullptr,
        1
    );
}

Mixer::~Mixer() {
    vQueueDelete(xQueue);
    vSemaphoreDelete(xMutex);
}

void Mixer::play(Pattern* pattern, int32_t eventIndex) {
    play_request_t request = {pattern, eventIndex};
    xQueueSend(xQueue, &request, portMAX_DELAY);
}

void Mixer::play(Pattern* pattern) {
    for (int32_t eventIndex = 0; eventIndex < CHANNEL_SIZE; eventIndex++) {
        play(pattern, eventIndex);
        vTaskDelay(1000 / 6 / portTICK_PERIOD_MS);
    }
}

void Mixer::stop() {
    play(NULL, 0);
}

void Mixer::mixerTask() {
    bool playing = false;
    event_t events[CHANNEL_COUNT];
    waveform_t waveforms[CHANNEL_COUNT];
    play_request_t request;
    int32_t mix = 0;
    int64_t time = 0;
    while (1) { // TODO: make this stoppable
        // TODO: Handle envelopes, effects and stuff in this loop
        // Check if queue has a new pattern and event index to play
        if (xQueueReceive(xQueue, &request, 0) == pdTRUE) {
            Pattern* pattern = request.pattern;
            int32_t eventIndex = request.eventIndex;
            if (pattern != NULL) {
                // Play the pattern
                for (int32_t channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
                    events[channelIndex] = pattern->getChannelEvent(channelIndex, eventIndex);
                    waveforms[channelIndex] = pattern->getChannelWaveform(channelIndex);
                }
                playing = true;
            } else {
                // Stop playing
                playing = false;
            }
        }
        // Mix the channels
        if (playing) {
            mix = 0;
            for (int32_t i = 0; i < MIXER_BUFFER_SIZE; i++) {
                float timeSec = ((float)time + i) / SAMPLE_RATE;
                for (int32_t channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
                    event_t event = events[channelIndex];
                    waveform_t waveform = waveforms[channelIndex];
                    waveform_fn_t waveform_fn = waveform_functions[waveform];
                    int16_t value = 0;
                    if (event.pitch != 0) {
                        float fValue = waveform_fn(timeSec, event.pitch, 1.0, 0.0);
                        // TODO
                        fValue *= 0.02; // Reduce volume - I don't want to wake up my family :D /AD
                        value = fValue * 32767;
                    }
                    mix += value;
                }
                mix = mix / CHANNEL_COUNT;
                audioBuffer[i] = mix;
            }
            size_t bytesWritten = 0;
            esp_i2s::i2s_write(esp_i2s::I2S_NUM_0, audioBuffer, MIXER_BUFFER_SIZE * 2, &bytesWritten, portMAX_DELAY);
            time += bytesWritten / 2;
        }
        taskYIELD();
    }

    // TODO: Make this part reachable & deinit I2S
    i2s_driver_uninstall(esp_i2s::I2S_NUM_0);
}

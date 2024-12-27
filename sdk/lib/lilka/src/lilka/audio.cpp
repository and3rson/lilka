#include "audio.h"
#include "config.h"
#include "ping.h"
#include "Preferences.h"

namespace lilka {

Audio::Audio() {
}

void ping_task(void* arg);

void Audio::begin() {
#if LILKA_VERSION == 1
    serial_err("Audio not supported in this version of lilka. Try to use Buzzer instead");
#elif LILKA_VERSION == 2

    initPins();

    Preferences prefs;
    prefs.begin("sound", true);
    volumeLevel = prefs.getUInt("volumeLevel", 100);
    startupSound = prefs.getBool("startupSound", true);
    prefs.end();

    I2S.setAllPins(LILKA_I2S_BCLK, LILKA_I2S_LRCK, LILKA_I2S_DOUT, LILKA_I2S_DOUT, -1);

    if (startupSound) xTaskCreatePinnedToCore(ping_task, "ping_task", 4096, NULL, 1, NULL, 0);
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

void Audio::adjustVolume(void* buffer, size_t size, int bitsPerSample) {
    int gain = (volumeLevel * 1024.0) / 100.0;
    int samples = size / (bitsPerSample / 8);

    if (bitsPerSample == 8) {
        uint8_t* smp = static_cast<uint8_t*>(buffer);

        for (int i = 0; i < samples; i++) {
            *smp = (*smp * gain) >> 10;
            smp++;
        }
    } else if (bitsPerSample == 16) {
        int16_t* smp = static_cast<int16_t*>(buffer);

        for (int i = 0; i < samples; i++) {
            *smp = (*smp * gain) >> 10;
            smp++;
        }
    } else if (bitsPerSample == 32) {
        int32_t* smp = static_cast<int32_t*>(buffer);

        for (int i = 0; i < samples; i++) {
            *smp = (*smp * gain) >> 10;
            smp++;
        }
    }
}

int Audio::getVolume() {
    return volumeLevel;
}

void Audio::setVolume(int level) {
    volumeLevel = level;
    saveSettings();
}

bool Audio::getStartupSoundEnabled() {
    return startupSound;
}

void Audio::setStartupSoundEnabled(bool enable) {
    startupSound = enable;
    saveSettings();
}

void Audio::saveSettings() {
    Preferences prefs;
    prefs.begin("sound", false);
    prefs.putUInt("volumeLevel", lilka::audio.volumeLevel);
    prefs.putBool("startupSound", startupSound);
    prefs.end();
}

void ping_task(void* arg) {
#if LILKA_VERSION == 1
    serial_err("This part of code should never be called. Audio not supported for this version of lilka");
#elif LILKA_VERSION == 2
#    ifndef LILKA_NO_AUDIO_HELLO

    // Signed 16-bit PCM
    const int16_t* ping = reinterpret_cast<const int16_t*>(ping_raw);

    vTaskDelay(400 / portTICK_PERIOD_MS);

    int16_t buf;
    I2S.begin(I2S_PHILIPS_MODE, 22050, 16);
    for (int i = 0; i < ping_raw_size / 2; i++) {
        memcpy(&buf, &ping[i], 2);
        lilka::audio.adjustVolume(&buf, 2, 16);

        I2S.write(buf >> 2);
        I2S.write(buf >> 2);
    }
    I2S.end();

#    endif

    vTaskDelete(NULL);
#endif
}

Audio audio;

} // namespace lilka

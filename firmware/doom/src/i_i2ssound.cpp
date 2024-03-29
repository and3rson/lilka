#include <lilka.h>
#include <I2S.h>
#include "driver/i2s.h"

extern "C" {
#include "config.h"
#include "i_sound.h"
#include "deh_str.h"
#include "m_misc.h"
#include "w_wad.h"
#include "z_zone.h"
}

static boolean use_sfx_prefix;

TaskHandle_t soundTaskHandle = NULL;
SemaphoreHandle_t soundMutexHandle = NULL;

// Звук - це складно! :D /AD
// Ring buffer for mixing sound
int16_t* mixerBuffer;
uint32_t mixerBufferStart = 0;
uint32_t mixerBufferEnd = 0;

extern SemaphoreHandle_t backBufferMutex;

void soundTask(void* param);
void queueSound(const uint8_t* data, uint32_t length, uint32_t sample_rate, uint8_t vol);

static snddevice_t sound_devices[] = {
    SNDDEVICE_SB,
    SNDDEVICE_PAS,
    SNDDEVICE_GUS,
    SNDDEVICE_WAVEBLASTER,
    SNDDEVICE_SOUNDCANVAS,
    SNDDEVICE_AWE32,
};

static boolean I_I2S_InitSound(bool _use_sfx_prefix) {
    lilka::serial_log("I_I2S_InitSound");
    use_sfx_prefix = _use_sfx_prefix;
    soundMutexHandle = xSemaphoreCreateMutex();

    mixerBuffer = static_cast<int16_t*>(ps_malloc(65536 * sizeof(int16_t)));

    xSemaphoreTake(
        backBufferMutex, portMAX_DELAY
    ); // Acquire back buffer mutex to prevent drawing while initializing I2S
    esp_i2s::i2s_config_t cfg = {
        .mode = (esp_i2s::i2s_mode_t)(esp_i2s::I2S_MODE_MASTER | esp_i2s::I2S_MODE_TX),
        .sample_rate = 11025,
        .bits_per_sample = esp_i2s::I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = esp_i2s::I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format =
            (esp_i2s::i2s_comm_format_t)(esp_i2s::I2S_COMM_FORMAT_STAND_I2S | esp_i2s::I2S_COMM_FORMAT_STAND_MSB),
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 2,
        .dma_buf_len = 1024,
        .use_apll = false,
        .tx_desc_auto_clear = true,
    };
    // i2s_driver_install(esp_i2s::I2S_NUM_0, &cfg, 2, &soundQueue);
    i2s_driver_install(esp_i2s::I2S_NUM_0, &cfg, 0, NULL);
    // esp_i2s::i2s_pin_config_t pin_cfg = {
    //     .bck_io_num = LILKA_I2S_BCLK,
    //     .ws_io_num = LILKA_I2S_LRCK,
    //     .data_out_num = LILKA_I2S_DOUT,
    //     .data_in_num = -1,
    // };
    // i2s_set_pin(esp_i2s::I2S_NUM_0, &pin_cfg);
    i2s_zero_dma_buffer(esp_i2s::I2S_NUM_0);
    xSemaphoreGive(backBufferMutex);

    if (xTaskCreatePinnedToCore(soundTask, "soundTask", 2048, NULL, 1, &soundTaskHandle, 0) != pdPASS) {
        lilka::serial_log("I_I2S_StartSound: xTaskCreatePinnedToCore failed");
    }

    return true;
}

static void I_I2S_ShutdownSound(void) {
    lilka::serial_log("I_I2S_ShutdownSound");
    xSemaphoreTake(soundMutexHandle, portMAX_DELAY);
    vTaskDelete(soundTaskHandle);
    i2s_driver_uninstall(esp_i2s::I2S_NUM_0);
    free(mixerBuffer);
    xSemaphoreGive(soundMutexHandle);
    vSemaphoreDelete(soundMutexHandle);
}

static void GetSfxLumpName(sfxinfo_t* sfx, char* buf, size_t buf_len) {
    // Linked sfx lumps? Get the lump number for the sound linked to.

    if (sfx->link != NULL) {
        sfx = sfx->link;
    }

    // Doom adds a DS* prefix to sound lumps; Heretic and Hexen don't
    // do this.

    if (use_sfx_prefix) {
        M_snprintf(buf, buf_len, "ds%s", DEH_String(sfx->name));
    } else {
        M_StringCopy(buf, DEH_String(sfx->name), buf_len);
    }
}

static int I_I2S_GetSfxLumpNum(sfxinfo_t* sfx) {
    char namebuf[9];

    GetSfxLumpName(sfx, namebuf, sizeof(namebuf));

    return W_GetNumForName(namebuf);
}

static void I_I2S_UpdateSound(void) {
    // Nothing to do here.
}

static void I_I2S_UpdateSoundParams(int handle, int vol, int sep) {
    // Nothing to do here.
}

static int I_I2S_StartSound(sfxinfo_t* sfxinfo, int channel, int vol, int sep) {
    // lilka::serial_log("I_I2S_StartSound: %s, channel: %d", DEH_String(sfxinfo->name), channel);
    // Get sound lump
    int lump = I_I2S_GetSfxLumpNum(sfxinfo);
    if (lump == -1) {
        lilka::serial_log("I_I2S_StartSound: I_I2S_GetSfxLumpNum failed");
        return -1;
    }

    uint32_t lumplen = W_LumpLength(lump);
    // lilka::serial_log("I_I2S_StartSound: lump %d, length %d", lump, lumplen);

    // Load sound lump
    byte* data = static_cast<byte*>(W_CacheLumpNum(lump, PU_CACHE)); // TODO - free
    if (data == NULL) {
        lilka::serial_log("I_I2S_StartSound: W_CacheLumpNum failed");
        return -1;
    }

    if (lumplen < 8 || data[0] != 0x03 || data[1] != 0x00) {
        // Invalid sound
        lilka::serial_log("I_I2S_StartSound: Invalid sound");
        return false;
    }

    uint16_t samplerate = (data[3] << 8) | data[2];
    uint32_t length = (data[7] << 24) | (data[6] << 16) | (data[5] << 8) | data[4];

    if (length > lumplen - 8 || length <= 48) {
        lilka::serial_log("I_I2S_StartSound: Invalid sound length");
        return false;
    }

    // The DMX sound library seems to skip the first 16 and last 16
    // bytes of the lump - reason unknown.
    data += 16;
    length -= 32;

    queueSound(data, length, samplerate, vol);

    return 0; // Return channel number?
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))

// Mix sound data into the ring buffer
void queueSound(const uint8_t* data, uint32_t length, uint32_t sample_rate, uint8_t vol) {
    // Here comes the fun part! /AD
    xSemaphoreTake(soundMutexHandle, portMAX_DELAY);

    // TODO: Resample to 11025 if needed? Most sounds are 11025, but not all...

    // Number of samples to mix with buffered samples
    int mixedLength;
    if (mixerBufferStart <= mixerBufferEnd) {
        // Buffer is contiguous
        mixedLength = mixerBufferEnd - mixerBufferStart;
    } else {
        // Buffer is split
        mixedLength = 65536 - mixerBufferStart;
    }
    mixedLength = MIN(mixedLength, length);
    // Number of samples to add unmixed beyond buffered samples
    int unmixedLength = length - mixedLength;

    uint32_t pos = mixerBufferStart;
    for (int i = 0; i < mixedLength; i++) {
        uint16_t rawSample =
            data[i] << 6; // Beef up the sample to 16-bit (minus 2 bits to avoid clipping), range will be [0;16384]
        int16_t sample = rawSample - 8192; // Center the sample around 0, range will be [-8192;8192]
        mixerBuffer[pos] = ((mixerBuffer[pos] * (64 - vol)) >> 6) + ((sample * vol) >> 6); // vol is [0;64]
        pos++;
        if (pos == 65536) {
            pos = 0;
        }
    }
    for (int i = 0; i < unmixedLength; i++) {
        uint16_t rawSample = data[mixedLength + i] << 6; // ditto
        int16_t sample = rawSample - 8192; // ditto
        mixerBuffer[pos] = (sample * vol) >> 6; // ditto
        pos++;
        if (pos == 65536) {
            pos = 0;
        }
    }
    if (unmixedLength > 0) {
        mixerBufferEnd = pos;
    }
    xSemaphoreGive(soundMutexHandle);
}

void soundTask(void* param) {
    while (1) {
        if (mixerBufferStart != mixerBufferEnd) {
            xSemaphoreTake(soundMutexHandle, portMAX_DELAY);
            size_t written = 0;
            TickType_t xLastWakeTime = xTaskGetTickCount();
            uint32_t chunkSize;
            if (mixerBufferStart < mixerBufferEnd) {
                // Buffer is contiguous
                chunkSize = mixerBufferEnd - mixerBufferStart;
            } else {
                // Buffer is split
                chunkSize = 65536 - mixerBufferStart;
            }
            chunkSize = MIN(chunkSize, 512) * 2; // 512 samples. Multiply by 2 because i2s_write expects bytes.
            esp_i2s::i2s_write(esp_i2s::I2S_NUM_0, mixerBuffer + mixerBufferStart, chunkSize, &written, portMAX_DELAY);
            mixerBufferStart += written / 2;
            if (mixerBufferStart >= 65536) {
                mixerBufferStart = 0;
            }
            xSemaphoreGive(soundMutexHandle);
            // Try to avoid starting next chunk too early.
            // We do this to prevent blocking in i2s_write and thus acquiring the mutex for too long.
            // This is done by calculating delay from sample rate & bytes written.
            // TODO: BTW - ring buffer mixing sucks with variable sample rates... /AD
            uint64_t delay = written / 2 * 1000 / 11025 / portTICK_PERIOD_MS;
            // Wait 7/8 of the time to prevent buffer underrun. Not the best solution, but works for now.
            delay = delay * 7 / 8;
            if (delay) {
                // vTaskDelayUntil doesn't like zero delays
                vTaskDelayUntil(&xLastWakeTime, delay);
            }
        }
        taskYIELD();
    }
}

static void I_I2S_StopSound(int handle) {
    // Nothing to do here.
}

static boolean I_I2S_SoundIsPlaying(int handle) {
    return false; // TODO
}

static void I_I2S_PrecacheSounds(sfxinfo_t* sounds, int num_sounds) {
    // Nothing to do here.
}

sound_module_t sound_module_I2S = {
    sound_devices,
    arrlen(sound_devices),
    I_I2S_InitSound,
    I_I2S_ShutdownSound,
    I_I2S_GetSfxLumpNum,
    I_I2S_UpdateSound,
    I_I2S_UpdateSoundParams,
    I_I2S_StartSound,
    I_I2S_StopSound,
    I_I2S_SoundIsPlaying,
    I_I2S_PrecacheSounds,
};

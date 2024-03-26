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
int use_libsamplerate = 0;
float libsamplerate_scale = 0.65f;

typedef struct {
    byte* data;
    uint32_t length;
    uint32_t sample_rate;
} sound_task_data_t;

TaskHandle_t soundTaskHandle = NULL;
SemaphoreHandle_t soundMutexHandle = NULL;
sound_task_data_t* sound_task_data = NULL;
// QueueHandle_t soundQueue = NULL;

extern SemaphoreHandle_t backBufferMutex;

void soundTask(void* param);

static snddevice_t sound_sdl_devices[] = {
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

    xSemaphoreTake(
        backBufferMutex, portMAX_DELAY
    ); // Acquire back buffer mutex to prevent drawing while initializing I2S
    esp_i2s::i2s_config_t cfg = {
        .mode = (esp_i2s::i2s_mode_t)(esp_i2s::I2S_MODE_MASTER | esp_i2s::I2S_MODE_TX),
        .sample_rate = 11025,
        .bits_per_sample = esp_i2s::I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = esp_i2s::I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format =
            (esp_i2s::i2s_comm_format_t)(esp_i2s::I2S_COMM_FORMAT_STAND_I2S | esp_i2s::I2S_COMM_FORMAT_STAND_MSB),
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 2,
        .dma_buf_len = 512,
        .use_apll = false,
        .tx_desc_auto_clear = true,
    };
    // i2s_driver_install(esp_i2s::I2S_NUM_0, &cfg, 2, &soundQueue);
    i2s_driver_install(esp_i2s::I2S_NUM_0, &cfg, 0, NULL);
    esp_i2s::i2s_pin_config_t pin_cfg = {
        .bck_io_num = LILKA_I2S_BCLK,
        .ws_io_num = LILKA_I2S_LRCK,
        .data_out_num = LILKA_I2S_DOUT,
        .data_in_num = -1,
    };
    i2s_set_pin(esp_i2s::I2S_NUM_0, &pin_cfg);
    i2s_zero_dma_buffer(esp_i2s::I2S_NUM_0);
    xSemaphoreGive(backBufferMutex);
    return true;
}

static void I_I2S_ShutdownSound(void) {
    lilka::serial_log("I_I2S_ShutdownSound");
    // I2S.end();
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
    lilka::serial_log("I_I2S_StartSound: %s, channel: %d", DEH_String(sfxinfo->name), channel);
    // Get sound lump
    int lump = I_I2S_GetSfxLumpNum(sfxinfo);
    if (lump == -1) {
        lilka::serial_log("I_I2S_StartSound: I_I2S_GetSfxLumpNum failed");
        return -1;
    }

    uint32_t lumplen = W_LumpLength(lump);
    lilka::serial_log("I_I2S_StartSound: lump %d, length %d", lump, lumplen);

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

    // printf("%x %x %x %x", data[0], data[1], data[2], data[3]);

    // for (int i = 0; i < length; i++) {
    //     I2S.write(data[i] << 6);
    //     I2S.write(data[i] << 6);
    // }
    // I2S.write_blocking(data, length);

    xSemaphoreTake(soundMutexHandle, portMAX_DELAY);
    if (soundTaskHandle != NULL) {
        // Stop current sound
        vTaskDelete(soundTaskHandle);
        soundTaskHandle = NULL;
        // I2S.end();

        // Delete sound task data
        delete sound_task_data;
    }

    // Create sound task data
    sound_task_data = new sound_task_data_t{data, length, samplerate};

    // Start sound
    xTaskCreatePinnedToCore(soundTask, "soundTask", 8192, sound_task_data, 1, &soundTaskHandle, 0);
    xSemaphoreGive(soundMutexHandle);

    //
    // // Start sound
    // xTaskCreatePinnedToCore(
    //     [](void* param) {
    //         sound_task_data_t* sound_task_data = static_cast<sound_task_data_t*>(param);
    //         I2S.write_blocking(sound_task_data->data, sound_task_data->length);
    //         vTaskDelete(NULL);
    //     },
    //     "soundTask",
    //     4096,
    //     sound_task_data,
    //     1,
    //     &soundTaskHandle,
    //     0
    // );

    // uint8_t* data = static_cast<uint8_t*>(sfxinfo->driver_data);
    // Serial.printf("%d %d %d %d", data[0], data[1], data[2], data[3]);
    // I2S.write(sfxinfo->driver_data);
    return 0; // Return channel number?
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))

void soundTask(void* param) {
    sound_task_data_t* sound_task_data = static_cast<sound_task_data_t*>(param);
    xSemaphoreTake(soundMutexHandle, portMAX_DELAY);
    // I2S.begin(I2S_PHILIPS_MODE, sound_task_data->sample_rate, 16);
    xSemaphoreGive(soundMutexHandle);
    // while (!I2S.availableForWrite()) {
    //     taskYIELD();
    // }
    // for (int i = 0; i < sound_task_data->length; i++) {
    //     xSemaphoreTake(soundMutexHandle, portMAX_DELAY);
    //     I2S.write(sound_task_data->data[i] << 6);
    //     I2S.write(sound_task_data->data[i] << 6);
    //     xSemaphoreGive(soundMutexHandle);
    // }

    esp_i2s::i2s_set_sample_rates(esp_i2s::I2S_NUM_0, sound_task_data->sample_rate);

    constexpr size_t samplesPerBuffer = 128;
    constexpr size_t bytesPerSample = 4;

    // size_t writtenTotal = 0;
    size_t samplesWritten = 0;
    size_t samplesTotal = sound_task_data->length;
    // size_t bytesTotal = sound_task_data->length * 4;
    while (samplesWritten < samplesTotal) {
        size_t written = 0;
        uint16_t buffer[samplesPerBuffer * bytesPerSample / 2]; // 1 KiB buffer
        xSemaphoreTake(soundMutexHandle, portMAX_DELAY);
        // Convert 8-bit mono samples to 16-bit stereo samples (x4 size)
        for (int i = 0; i < MIN(samplesPerBuffer, samplesTotal - samplesWritten); i++) {
            uint16_t word = sound_task_data->data[samplesWritten + i] << 3;
            buffer[i * 2] = word;
            buffer[i * 2 + 1] = word;
        }
        // Serial.print("Writing ");
        // Serial.print(MIN((samplesTotal - samplesWritten) * bytesPerSample, samplesPerBuffer * bytesPerSample));
        // Serial.println(" bytes");
        esp_i2s::i2s_write(
            esp_i2s::I2S_NUM_0,
            buffer,
            MIN((samplesTotal - samplesWritten) * bytesPerSample, samplesPerBuffer * bytesPerSample),
            &written,
            portMAX_DELAY
        );
        // Serial.print("Wrote");
        // Serial.print(written);
        // Serial.println(" bytes");
        xSemaphoreGive(soundMutexHandle);
        samplesWritten += (written + 3) / 4; // Round up
    }

    // size_t writtenTotal = 0;
    // while (writtenTotal < sound_task_data->length) {
    //     size_t written = 0;
    //     esp_i2s::i2s_write(
    //         esp_i2s::I2S_NUM_0, sound_task_data, sound_task_data->length - written, &written, portMAX_DELAY
    //     );
    //     writtenTotal += written;
    // }

    // I2S.write(const_cast<uint8_t*>(sound_task_data->data), sound_task_data->length);
    // I2S.write(sound_task_data->data, sound_task_data->length);
    xSemaphoreTake(soundMutexHandle, portMAX_DELAY);
    soundTaskHandle = NULL;
    delete sound_task_data;
    // I2S.end();
    xSemaphoreGive(soundMutexHandle);
    vTaskDelete(NULL);
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

sound_module_t DG_sound_module = {
    sound_sdl_devices,
    arrlen(sound_sdl_devices),
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

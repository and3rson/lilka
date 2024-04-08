#pragma once

#include "FreeRTOS.h"
#include <semphr.h>
#include <stdint.h>
#include "config.h"
#include "waveforms.h"
#include "effects.h"

#define MIXER_BUFFER_SIZE        256
#define SAMPLE_RATE              16000
#define MIXER_BUFFER_DURATION_MS (MIXER_BUFFER_SIZE * 1000 / SAMPLE_RATE)
#define SECONDS_PER_SAMPLE       (1.0f / SAMPLE_RATE)

typedef enum {
    MIXER_COMMAND_SET_WAVEFORM,
    MIXER_COMMAND_SET_FREQUENCY,
    MIXER_COMMAND_SET_VOLUME,
    MIXER_COMMAND_SET_EFFECT,
    MIXER_COMMAND_CLEAR,
    MIXER_COMMAND_COUNT,
} mixer_command_type_t;

typedef struct {
    int32_t channelIndex;
    mixer_command_type_t type;
    union {
        waveform_t waveform;
        float frequency;
        float volume;
        effect_t effect;
    };
} mixer_command_t;

class Mixer {
public:
    Mixer();
    ~Mixer();
    void sendCommand(const mixer_command_t command);
    void start(
        int32_t channelIndex, waveform_t waveforms, float pitch, float volume, effect_t effect = {EFFECT_TYPE_NONE, 0}
    );
    void stop();
    int16_t readBuffer(int16_t* targetBuffer);
    int16_t readBuffer(int16_t* targetBuffer, int32_t channelIndex);
    float getMasterVolume();

private:
    SemaphoreHandle_t xMutex;
    QueueHandle_t xQueue;
    void mixerTask();
    int16_t audioBufferCopy[MIXER_BUFFER_SIZE];
    int16_t channelAudioBuffersCopy[CHANNEL_COUNT][MIXER_BUFFER_SIZE];
    float masterVolume;
};

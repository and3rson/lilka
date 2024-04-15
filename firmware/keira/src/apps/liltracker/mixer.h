#pragma once

#include "FreeRTOS.h"
#include <semphr.h>
#include <stdint.h>
#include "config.h"
#include "waveforms.h"
#include "effects.h"

#define MIXER_BUFFER_SIZE        512
#define SAMPLE_RATE              44100
#define MIXER_BUFFER_DURATION_MS (MIXER_BUFFER_SIZE * 1000 / SAMPLE_RATE)
#define SECONDS_PER_SAMPLE       (1.0f / SAMPLE_RATE)

typedef enum : uint8_t {
    MIXER_COMMAND_SET_WAVEFORM,
    MIXER_COMMAND_SET_FREQUENCY,
    MIXER_COMMAND_SET_VOLUME,
    MIXER_COMMAND_SET_EFFECT,
    MIXER_COMMAND_SET_OFF,
    MIXER_COMMAND_COUNT,
} mixer_command_type_t;

typedef struct {
    uint8_t channelIndex;
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
        uint8_t channelIndex, waveform_t waveforms, float frequency, float volume,
        effect_t effect = {EFFECT_TYPE_NONE, 0}
    );
    void stop();
    void reset();
    int16_t readBuffer(int16_t* targetBuffer);
    int16_t readBuffer(int16_t* targetBuffer, uint8_t channelIndex);
    void setMasterVolume(float volume);
    float getMasterVolume();

private:
    SemaphoreHandle_t xMutex;
    QueueHandle_t xQueue;
    void mixerTask();
    int16_t audioBufferCopy[MIXER_BUFFER_SIZE];
    int16_t channelAudioBuffersCopy[CHANNEL_COUNT][MIXER_BUFFER_SIZE];
    float masterVolume;
};

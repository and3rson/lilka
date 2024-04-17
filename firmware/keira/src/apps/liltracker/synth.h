#pragma once

#include <FreeRTOS.h>
#include <semphr.h>

#include "config.h"
#include "waveforms.h"
#include "effects.h"

#define SYNTH_BUFFER_SIZE 512
#define SYNTH_SAMPLE_RATE 44100
// #define SYNTH_BUFFER_DURATION_MS (SYNTH_BUFFER_SIZE * 1000 / SYNTH_SAMPLE_RATE)
#define SYNTH_SECONDS_PER_SAMPLE (1.0f / SYNTH_SAMPLE_RATE)

typedef struct {
    waveform_t waveform;
    float frequency;
    float volume;
    // TODO: Research how effects should generally be handled in NES & 6581, since things seem weird:
    // some effects are cancelled by others, some are reset by OFF, etc... /AD
    effect_t effect;
    float effectStartTime;
    // float time; // TODO
} channel_state_t;

class Synth {
public:
    Synth();
    ~Synth();
    void reset();
    void setWaveform(uint8_t channelIndex, waveform_t waveform);
    void setFrequency(uint8_t channelIndex, float frequency);
    void setVolume(uint8_t channelIndex, float volume);
    void setEffect(uint8_t channelIndex, effect_t effect);
    void setOff(uint8_t channelIndex);
    void advanceTime(uint64_t sampleCount);
    void render(
        int16_t combinedBuffer[SYNTH_BUFFER_SIZE], int16_t channelBuffers[CHANNEL_COUNT][SYNTH_BUFFER_SIZE],
        uint32_t sampleCount, float masterVolume
    );

private:
    SemaphoreHandle_t xMutex;
    int64_t currentSample;
    channel_state_t channelStates[CHANNEL_COUNT];
};

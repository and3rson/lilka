#pragma once

#include "FreeRTOS.h"
#include <semphr.h>
#include <stdint.h>
#include "functions.h"

constexpr int32_t bpm = 400;
constexpr int32_t channelCount = 3;
constexpr int32_t channelSize = 32;

typedef struct {
    int32_t pitch;
    // float velocity;
} event_t;

typedef struct {
    waveform_t waveform;
    float volume;
    float panning;
    float pitch;
    event_t events[channelSize];
} channel_t;

class Sequencer {
public:
    Sequencer();
    void start();
    waveform_t getChannelWaveform(int32_t channelIndex);
    void setChannelWaveform(int32_t channelIndex, waveform_t waveform);
    event_t getChannelEvent(int32_t channelIndex, int32_t eventIndex);
    void setChannelEvent(int32_t channelIndex, int32_t eventIndex, event_t event);
    void setChannelEvents(int32_t channelIndex, const event_t* events);

    int32_t getCurrentEventIndex();

private:
    SemaphoreHandle_t xMutex;
    channel_t channels[channelCount] = {
        {
            .waveform = WAVEFORM_SQUARE,
            .volume = 1.0f,
            .panning = 0.5f,
            .pitch = 1.0f,
            .events = {},
        },
        // {
        //     .waveform = WAVEFORM_SAWTOOTH,
        //     .volume = 1.0f,
        //     .panning = 0.5f,
        //     .pitch = 1.0f,
        //     .events = {},
        // },
        {
            .waveform = WAVEFORM_SAWTOOTH,
            .volume = 1.0f,
            .panning = 0.5f,
            .pitch = 1.0f,
            .events = {},
        },
        {
            .waveform = WAVEFORM_TRIANGLE,
            .volume = 1.0f,
            .panning = 0.5f,
            .pitch = 1.0f,
            .events = {},
        },
    };
    void sequencerTask();
    int32_t currentEventIndex = 0;
};

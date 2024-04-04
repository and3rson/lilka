#pragma once

#include <FreeRTOS.h>
#include <semphr.h>

#include "waveforms.h"
#include "config.h"

typedef struct {
    int32_t pitch;
    float velocity;
} event_t;

typedef struct {
    waveform_t waveform;
    float volume;
    float pitch;
    event_t events[CHANNEL_SIZE];
} channel_t;

class Pattern {
public:
    Pattern();
    ~Pattern();
    waveform_t getChannelWaveform(int32_t channelIndex);
    void setChannelWaveform(int32_t channelIndex, waveform_t waveform);
    event_t getChannelEvent(int32_t channelIndex, int32_t eventIndex);
    void setChannelEvent(int32_t channelIndex, int32_t eventIndex, event_t event);
    void setChannelEvents(int32_t channelIndex, const event_t* events);
    int writeToBuffer(uint8_t* buffer);
    int readFromBuffer(const uint8_t* buffer);

private:
    SemaphoreHandle_t xMutex;
    channel_t channels[CHANNEL_COUNT];
};

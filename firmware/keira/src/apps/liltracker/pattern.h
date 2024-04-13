#pragma once

#include <FreeRTOS.h>
#include <semphr.h>

#include "note.h"
#include "waveforms.h"
#include "effects.h"
#include "config.h"

typedef enum {
    EVENT_TYPE_CONT,
    EVENT_TYPE_NORMAL,
    EVENT_TYPE_STOP,
    EVENT_TYPE_COUNT,
} event_type_t;

typedef struct {
    noteinfo_t note; // 2 bytes
    uint8_t volume; // 1 byte
    event_type_t type; // 1 byte
    effect_t effect; // 2 bytes
    uint8_t reserved[10]; // 10 bytes
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

    int calculateWriteBufferSize();
    int writeToBuffer(uint8_t* buffer);
    int readFromBuffer(const uint8_t* buffer);

private:
    SemaphoreHandle_t xMutex;
    channel_t channels[CHANNEL_COUNT];
};

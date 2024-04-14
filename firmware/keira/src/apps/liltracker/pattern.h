#pragma once

#include <FreeRTOS.h>
#include <semphr.h>

#include "note.h"
#include "waveforms.h"
#include "effects.h"
#include "config.h"

typedef enum : uint8_t {
    EVENT_TYPE_CONT,
    EVENT_TYPE_NORMAL,
    EVENT_TYPE_OFF,
    EVENT_TYPE_COUNT,
} event_type_t;

typedef struct {
    noteinfo_t note;
    waveform_t waveform;
    uint8_t volume;
    event_type_t type;
    effect_t effect;
} event_t;

typedef struct {
    uint8_t volume;
    event_t events[CHANNEL_SIZE];
} channel_t;

class Pattern {
public:
    Pattern();
    ~Pattern();
    event_t getChannelEvent(uint8_t channelIndex, int16_t eventIndex);
    void setChannelEvent(uint8_t channelIndex, int16_t eventIndex, event_t event);
    void setChannelEvents(uint8_t channelIndex, const event_t* events);

    int calculateWriteBufferSize();
    int writeToBuffer(uint8_t* buffer);
    int readFromBuffer(const uint8_t* buffer);

private:
    SemaphoreHandle_t xMutex;
    channel_t channels[CHANNEL_COUNT];
};

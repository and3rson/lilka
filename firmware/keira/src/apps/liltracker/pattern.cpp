#include <lilka.h>
#include <freertos/queue.h>

#include "pattern.h"
#include "utils/acquire.h"

#define WRITE_TO_BUFFER(buffer, value)              \
    memcpy(&buffer[offset], &value, sizeof(value)); \
    offset += sizeof(value);

#define READ_FROM_BUFFER(value, buffer)             \
    memcpy(&value, &buffer[offset], sizeof(value)); \
    offset += sizeof(value);

Pattern::Pattern() : xMutex(xSemaphoreCreateMutex()) {
    const waveform_t defaultWaveforms[CHANNEL_COUNT] = {
        WAVEFORM_SQUARE,
        WAVEFORM_SAWTOOTH,
        WAVEFORM_NOISE,
    };
    for (uint8_t channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
        // Set some default waveforms for nicer expeience when starting from scratch
        channels[channelIndex].volume = 1.0f;
        for (int16_t eventIndex = 0; eventIndex < CHANNEL_SIZE; eventIndex++) {
            event_t event = {
                N_C0,
                eventIndex == 0 ? defaultWaveforms[channelIndex] : WAVEFORM_CONT,
                MAX_VOLUME,
                EVENT_TYPE_CONT,
                {EFFECT_TYPE_NONE, 0},
            };
            channels[channelIndex].events[eventIndex] = event;
        }
    }
    xSemaphoreGive(xMutex);
}

Pattern::~Pattern() {
    vSemaphoreDelete(xMutex);
}

event_t Pattern::getChannelEvent(uint8_t channelIndex, int16_t eventIndex) {
    Acquire acquire(xMutex);
    return channels[channelIndex].events[eventIndex];
}

void Pattern::setChannelEvent(uint8_t channelIndex, int16_t eventIndex, event_t event) {
    Acquire acquire(xMutex);
    channels[channelIndex].events[eventIndex] = event;
}

void Pattern::setChannelEvents(uint8_t channelIndex, const event_t* events) {
    Acquire acquire(xMutex);
    for (int16_t eventIndex = 0; eventIndex < CHANNEL_SIZE; eventIndex++) {
        event_t event = events[eventIndex];
        channels[channelIndex].events[eventIndex] = event;
    }
}

int Pattern::calculateWriteBufferSize() {
    Acquire acquire(xMutex);
    int32_t bufferSize = 0;

    // Calculate all channels
    for (uint8_t channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
        bufferSize += sizeof(uint8_t); // Volume

        // Calculate events
        bufferSize += sizeof(int32_t);
        bufferSize += sizeof(event_t) * CHANNEL_SIZE;
    }

    return bufferSize * 2;
}

int Pattern::writeToBuffer(uint8_t* buffer) {
    Acquire acquire(xMutex);
    int32_t offset = 0;

    // Write all channels
    for (uint8_t channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
        // Write channel settings
        WRITE_TO_BUFFER(buffer, channels[channelIndex].volume);

        // Write events
        int16_t eventCount = CHANNEL_SIZE;
        WRITE_TO_BUFFER(buffer, eventCount);
        for (int16_t eventIndex = 0; eventIndex < CHANNEL_SIZE; eventIndex++) {
            WRITE_TO_BUFFER(buffer, channels[channelIndex].events[eventIndex]);
        }
    }

    return offset;
}
int Pattern::readFromBuffer(const uint8_t* buffer) {
    Acquire acquire(xMutex);
    int32_t offset = 0;

    // Read all channels
    for (uint8_t channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
        // Read channel settings
        READ_FROM_BUFFER(channels[channelIndex].volume, buffer);

        // Read events
        int16_t eventCount;
        READ_FROM_BUFFER(eventCount, buffer);
        for (int16_t eventIndex = 0; eventIndex < eventCount; eventIndex++) {
            READ_FROM_BUFFER(channels[channelIndex].events[eventIndex], buffer);
        }
    }

    return offset;
}

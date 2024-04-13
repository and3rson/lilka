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
    for (int32_t channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
        // Set some default waveforms for nicer expeience when starting from scratch
        channels[channelIndex].waveform = defaultWaveforms[channelIndex];
        channels[channelIndex].volume = 1.0f;
        channels[channelIndex].pitch = 1.0f;
        for (int32_t eventIndex = 0; eventIndex < CHANNEL_SIZE; eventIndex++) {
            event_t event = {N_C0, MAX_VOLUME, EVENT_TYPE_CONT, {EFFECT_TYPE_NONE, 0}};
            channels[channelIndex].events[eventIndex] = event;
        }
    }
    xSemaphoreGive(xMutex);
}

Pattern::~Pattern() {
    vSemaphoreDelete(xMutex);
}

waveform_t Pattern::getChannelWaveform(int32_t channelIndex) {
    Acquire acquire(xMutex);
    return channels[channelIndex].waveform;
}

void Pattern::setChannelWaveform(int32_t channelIndex, waveform_t waveform) {
    Acquire acquire(xMutex);
    channels[channelIndex].waveform = waveform;
}

event_t Pattern::getChannelEvent(int32_t channelIndex, int32_t eventIndex) {
    Acquire acquire(xMutex);
    return channels[channelIndex].events[eventIndex];
}

void Pattern::setChannelEvent(int32_t channelIndex, int32_t eventIndex, event_t event) {
    Acquire acquire(xMutex);
    channels[channelIndex].events[eventIndex] = event;
}

void Pattern::setChannelEvents(int32_t channelIndex, const event_t* events) {
    Acquire acquire(xMutex);
    for (int32_t eventIndex = 0; eventIndex < CHANNEL_SIZE; eventIndex++) {
        event_t event = events[eventIndex];
        channels[channelIndex].events[eventIndex] = event;
    }
}

int Pattern::calculateWriteBufferSize() {
    Acquire acquire(xMutex);
    int32_t bufferSize = 0;

    // Count 32 reserved bytes
    bufferSize += 32;

    // Calculate all channels
    for (int32_t channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
        // Calculate channel settings
        bufferSize += sizeof(int32_t);
        bufferSize += sizeof(waveform_t);
        bufferSize += sizeof(float);
        bufferSize += sizeof(float);

        // Calculate events
        bufferSize += sizeof(int32_t);
        bufferSize += sizeof(event_t) * CHANNEL_SIZE;
    }

    return bufferSize;
}

int Pattern::writeToBuffer(uint8_t* buffer) {
    Acquire acquire(xMutex);
    int32_t offset = 0;

    // Write 32 reserved bytes
    while (offset < 32) {
        buffer[offset++] = 0;
    }

    // Write all channels
    for (int32_t channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
        // Write channel settings
        // memcpy(&buffer[offset], &channelIndex, sizeof(int32_t));
        WRITE_TO_BUFFER(buffer, channelIndex);
        WRITE_TO_BUFFER(buffer, channels[channelIndex].waveform);
        WRITE_TO_BUFFER(buffer, channels[channelIndex].volume);
        WRITE_TO_BUFFER(buffer, channels[channelIndex].pitch);

        // Write events
        int32_t eventCount = CHANNEL_SIZE;
        WRITE_TO_BUFFER(buffer, eventCount);
        for (int32_t eventIndex = 0; eventIndex < CHANNEL_SIZE; eventIndex++) {
            WRITE_TO_BUFFER(buffer, channels[channelIndex].events[eventIndex]);
        }
    }

    return offset;
}

int Pattern::readFromBuffer(const uint8_t* buffer) {
    Acquire acquire(xMutex);
    int32_t offset = 0;

    // Skip 32 reserved bytes
    offset = 32;

    // Read all channels
    for (int32_t channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
        // Read channel settings
        READ_FROM_BUFFER(channelIndex, buffer);
        READ_FROM_BUFFER(channels[channelIndex].waveform, buffer);
        READ_FROM_BUFFER(channels[channelIndex].volume, buffer);
        READ_FROM_BUFFER(channels[channelIndex].pitch, buffer);

        // Read events
        int32_t eventCount;
        READ_FROM_BUFFER(eventCount, buffer);
        for (int32_t eventIndex = 0; eventIndex < eventCount; eventIndex++) {
            READ_FROM_BUFFER(channels[channelIndex].events[eventIndex], buffer);
        }
    }

    return offset;
}

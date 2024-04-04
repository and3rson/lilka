#include "pattern.h"
#include <string.h>

class Acquire {
public:
    Acquire(SemaphoreHandle_t xMutex) {
        this->xMutex = xMutex;
        xSemaphoreTake(xMutex, portMAX_DELAY);
    }
    ~Acquire() {
        xSemaphoreGive(xMutex);
    }

private:
    SemaphoreHandle_t xMutex;
};

Pattern::Pattern() : xMutex(xSemaphoreCreateMutex()) {
    for (int32_t channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
        channels[channelIndex].waveform = WAVEFORM_SQUARE;
        channels[channelIndex].volume = 1.0f;
        channels[channelIndex].pitch = 1.0f;
        for (int32_t eventIndex = 0; eventIndex < CHANNEL_SIZE; eventIndex++) {
            channels[channelIndex].events[eventIndex].pitch = 0;
            channels[channelIndex].events[eventIndex].velocity = 1.0f;
        }
    }
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
        channels[channelIndex].events[eventIndex] = events[eventIndex];
    }
}

int Pattern::writeToBuffer(uint8_t* buffer) {
    Acquire acquire(xMutex);
    uint8_t* offset = buffer;
    for (int32_t channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
        memcpy(offset, &channels[channelIndex].waveform, sizeof(waveform_t));
        offset += sizeof(waveform_t);
        memcpy(offset, &channels[channelIndex].volume, sizeof(float));
        offset += sizeof(float);
        memcpy(offset, &channels[channelIndex].pitch, sizeof(float));
        offset += sizeof(float);
        for (int32_t eventIndex = 0; eventIndex < CHANNEL_SIZE; eventIndex++) {
            memcpy(offset, &channels[channelIndex].events[eventIndex].pitch, sizeof(int32_t));
            offset += sizeof(int32_t);
            memcpy(offset, &channels[channelIndex].events[eventIndex].velocity, sizeof(float));
            offset += sizeof(float);
        }
    }
    return offset - buffer;
}

int Pattern::readFromBuffer(const uint8_t* buffer) {
    Acquire acquire(xMutex);
    const uint8_t* offset = buffer;
    for (int32_t channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
        memcpy(&channels[channelIndex].waveform, offset, sizeof(waveform_t));
        offset += sizeof(waveform_t);
        memcpy(&channels[channelIndex].volume, offset, sizeof(float));
        offset += sizeof(float);
        memcpy(&channels[channelIndex].pitch, offset, sizeof(float));
        offset += sizeof(float);
        for (int32_t eventIndex = 0; eventIndex < CHANNEL_SIZE; eventIndex++) {
            memcpy(&channels[channelIndex].events[eventIndex].pitch, offset, sizeof(int32_t));
            offset += sizeof(int32_t);
            memcpy(&channels[channelIndex].events[eventIndex].velocity, offset, sizeof(float));
            offset += sizeof(float);
        }
    }
    return offset - buffer;
}

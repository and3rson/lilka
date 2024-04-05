#pragma once

#include "FreeRTOS.h"
#include <semphr.h>
#include <stdint.h>
#include "pattern.h"

#define MIXER_BUFFER_SIZE 256

class Mixer {
public:
    Mixer();
    ~Mixer();
    void start(int32_t channelIndex, waveform_t waveforms, float pitch, float volume);
    void stop();

private:
    SemaphoreHandle_t xMutex;
    QueueHandle_t xQueue;
    void mixerTask();
    int16_t audioBuffer[MIXER_BUFFER_SIZE];
};

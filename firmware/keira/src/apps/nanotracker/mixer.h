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
    void play(Pattern* pattern, int32_t eventIndex);
    void play(Pattern* pattern);
    void stop();

private:
    SemaphoreHandle_t xMutex;
    QueueHandle_t xQueue;
    void mixerTask();
    int16_t audioBuffer[MIXER_BUFFER_SIZE];
};

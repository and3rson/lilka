#include <I2S.h>
#include "sequencer.h"

Sequencer::Sequencer() : xMutex(xSemaphoreCreateMutex()) {
}

void Sequencer::start() {
    xTaskCreate(
        [](void* pvParameters) {
            static_cast<Sequencer*>(pvParameters)->sequencerTask();
            vTaskDelete(NULL);
        },
        "sequencerTask",
        4096,
        this,
        1,
        nullptr
    );
}

void Sequencer::setChannelEvents(int32_t channelIndex, const event_t* events) {
    for (int32_t eventIndex = 0; eventIndex < channelSize; eventIndex++) {
        channels[channelIndex].events[eventIndex] = events[eventIndex];
    }
}

void Sequencer::sequencerTask() {
    constexpr uint32_t sampleRate = 11025;
    // Calculate the number of samples per beat
    int32_t samplesPerBeat = sampleRate * 60 / bpm;
    I2S.begin(I2S_PHILIPS_MODE, sampleRate, 16);
    uint64_t time = 0;
    while (true) {
        // Iterate over events
        for (int32_t eventIndex = 0; eventIndex < channelSize; eventIndex++) {
            xSemaphoreTake(xMutex, portMAX_DELAY);
            currentEventIndex = eventIndex;
            xSemaphoreGive(xMutex);
            // Process one bar
            for (int i = 0; i < samplesPerBeat; i++) {
                time += 1;
                int32_t mix = 0;
                // Iterate over channels
                for (int32_t channelIndex = 0; channelIndex < channelCount; channelIndex++) {
                    const channel_t* channel = &channels[channelIndex];
                    event_t event = channel->events[eventIndex];
                    waveform_fn_t waveform_fn = nullptr;
                    switch (channel->waveform) {
                        case WAVEFORM_SQUARE:
                            waveform_fn = square;
                            break;
                        case WAVEFORM_SAWTOOTH:
                            waveform_fn = sawtooth;
                            break;
                        case WAVEFORM_TRIANGLE:
                            waveform_fn = triangle;
                            break;
                        case WAVEFORM_SINE:
                            waveform_fn = sine;
                            break;
                        case WAVEFORM_NOISE:
                            break;
                    }
                    int16_t value = 0;
                    if (event.pitch != 0) {
                        float frequency = static_cast<float>(event.pitch);
                        float amplitude = 1.0;
                        float phase = 0.0;
                        value = waveform_fn(((float)time) / sampleRate, frequency, amplitude, phase) * 32767;
                    }
                    mix += value;
                }
                mix = mix / channelCount / 2; // Divide by 2 to reduce volume
                I2S.write(static_cast<int16_t>(mix));
                I2S.write(static_cast<int16_t>(mix));
            }
        }
        taskYIELD();
    }
}

event_t Sequencer::getChannelEvent(int32_t channelIndex, int32_t eventIndex) {
    return channels[channelIndex].events[eventIndex];
}

int Sequencer::getCurrentEventIndex() {
    xSemaphoreTake(xMutex, portMAX_DELAY);
    int32_t currentEventIndexCopy = currentEventIndex;
    xSemaphoreGive(xMutex);
    return currentEventIndexCopy;
}

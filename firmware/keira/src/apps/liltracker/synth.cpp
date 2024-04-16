#include "synth.h"
#include "utils/acquire.h"

Synth::Synth() : xMutex(xSemaphoreCreateMutex()), currentSample(0) {
    for (int i = 0; i < CHANNEL_COUNT; i++) {
        channelStates[i] = {
            .waveform = WAVEFORM_SQUARE,
            .frequency = 0.0f,
            .volume = 1.0f,
            .effect = {EFFECT_TYPE_NONE, 0},
            .effectStartTime = 0.0f,
        };
    }
    xSemaphoreGive(xMutex);
}

void Synth::reset() {
    Acquire lock(xMutex);
    currentSample = 0;
    for (int i = 0; i < CHANNEL_COUNT; i++) {
        channelStates[i] = {
            .waveform = WAVEFORM_SQUARE,
            .frequency = 0.0f,
            .volume = 1.0f,
            .effect = {EFFECT_TYPE_NONE, 0},
            .effectStartTime = 0.0f,
        };
    }
}

void Synth::setWaveform(uint8_t channelIndex, waveform_t waveform) {
    Acquire lock(xMutex);
    channelStates[channelIndex].waveform = waveform;
}

void Synth::setFrequency(uint8_t channelIndex, float frequency) {
    Acquire lock(xMutex);
    channelStates[channelIndex].frequency = frequency;
}

void Synth::setVolume(uint8_t channelIndex, float volume) {
    Acquire lock(xMutex);
    channelStates[channelIndex].volume = volume;
}

void Synth::setEffect(uint8_t channelIndex, effect_t effect) {
    Acquire lock(xMutex);
    channelStates[channelIndex].effect = effect;
    channelStates[channelIndex].effectStartTime = currentSample * SYNTH_SECONDS_PER_SAMPLE;
}

void Synth::advanceTime(uint64_t sampleCount) {
    Acquire lock(xMutex);
    currentSample += sampleCount;
}

void Synth::render(
    int16_t combinedBuffer[SYNTH_BUFFER_SIZE], int16_t channelBuffers[CHANNEL_COUNT][SYNTH_BUFFER_SIZE],
    uint32_t sampleCount, float masterVolume
) {
    Acquire lock(xMutex);
    for (int16_t i = 0; i < sampleCount; i++) {
        float timeSec = ((float)currentSample + i) * SYNTH_SECONDS_PER_SAMPLE;
        for (uint8_t channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
            // event_t event = events[channelIndex];
            const channel_state_t* channelState = &channelStates[channelIndex];
            waveform_fn_t waveform_fn = waveform_functions[channelState->waveform];
            float relTime = timeSec - channelState->effectStartTime;
            float modFrequency = channelState->frequency;
            if (modFrequency == 0.0f) {
                channelBuffers[channelIndex][i] = 0;
            } else {
                float modVolume = channelState->volume;
                float modPhase = 0.0;
                effect_t effect = channelState->effect;
                effect_fn_t effect_fn = effect_functions[effect.type];
                effect_fn(timeSec, relTime, &modFrequency, &modVolume, &modPhase, effect.param);
                channelBuffers[channelIndex][i] =
                    waveform_fn(timeSec, modFrequency, modVolume, modPhase) * masterVolume * 32767;
            }
        }
    }
    for (int16_t i = 0; i < sampleCount; i++) {
        combinedBuffer[i] = 0;
        for (uint8_t channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
            combinedBuffer[i] += channelBuffers[channelIndex][i];
        }
        combinedBuffer[i] /= CHANNEL_COUNT;
    }
}

void Synth::setOff(uint8_t channelIndex) {
    Acquire lock(xMutex);
    channelStates[channelIndex].frequency = 0.0f;
}

Synth::~Synth() {
    vSemaphoreDelete(xMutex);
}

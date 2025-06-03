#include <math.h>
#include <string.h>

#include "sequencer.h"
#include "lilka/serial.h"
#include "utils/acquire.h"

Sequencer::Sequencer(Sink* sink) :
    xMutex(xSemaphoreCreateMutex()), xPlaying(xSemaphoreCreateBinary()), sink(sink), masterVolume(0.25f) {
    playstate.playing = false;
    playstate.pageIndex = 0;
    playstate.loopPage = false;
    playstate.loopTrack = false;
    playstate.eventIndex = 0;
    playstate.track = NULL;

    xSemaphoreGive(xMutex);
    xSemaphoreGive(xPlaying);

    memset(audioBufferCopy, 0, sizeof(int16_t) * SYNTH_BUFFER_SIZE);
    for (uint8_t channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
        memset(channelAudioBuffersCopy[channelIndex], 0, sizeof(int16_t) * SYNTH_BUFFER_SIZE);
    }
}

Sequencer::~Sequencer() {
    vSemaphoreDelete(xMutex);
    vSemaphoreDelete(xPlaying);
}

void Sequencer::setSink(Sink* sink) {
    Acquire acquire(xMutex);
    this->sink = sink;
}

void Sequencer::play(Track* track, uint16_t pageIndex, int8_t channelIndex, uint16_t eventIndex) {
    {
        Acquire acquire(xMutex);
        if (playstate.playing) {
            return;
        }
    }

    xSemaphoreTake(xPlaying, portMAX_DELAY);
    playstate.track = track;
    playstate.pageIndex = pageIndex;
    playstate.channelIndex = channelIndex;
    playstate.eventIndex = eventIndex;
    playstate.loopPage = false;
    playstate.loopTrack = false;
    playstate.playing = true;

    if (xTaskCreatePinnedToCore(
            [](void* pvParameters) {
                Sequencer* sequencer = static_cast<Sequencer*>(pvParameters);
                sequencer->singleEventTask();
                xSemaphoreGive(sequencer->xPlaying);
                vTaskDelete(NULL);
            },
            "sequencerTask",
            8192,
            this,
            1,
            nullptr,
            0
        ) != pdPASS) {
        lilka::serial.log("Failed to create sequencer task");
        xSemaphoreGive(xPlaying);
    }
}

void Sequencer::play(Track* track, uint16_t pageIndex, bool loopTrack) {
    {
        Acquire acquire(xMutex);
        if (playstate.playing) {
            return;
        }
    }

    xSemaphoreTake(xPlaying, portMAX_DELAY);
    playstate.track = track;
    playstate.pageIndex = pageIndex;
    playstate.channelIndex = -1;
    playstate.eventIndex = 0;
    playstate.loopPage = false;
    playstate.loopTrack = loopTrack;
    playstate.playing = true;

    if (xTaskCreatePinnedToCore(
            [](void* pvParameters) {
                Sequencer* sequencer = static_cast<Sequencer*>(pvParameters);
                sequencer->multiEventTask();
                xSemaphoreGive(sequencer->xPlaying);
                vTaskDelete(NULL);
            },
            "sequencerTask",
            8192,
            this,
            1,
            nullptr,
            0
        ) != pdPASS) {
        lilka::serial.log("Failed to create sequencer task");
        xSemaphoreGive(xPlaying);
    }
}

void Sequencer::stop() {
    Acquire acquire(xMutex);
    playstate.playing = false;
}

seq_state_t Sequencer::getSeqState() {
    Acquire acquire(xMutex);
    return playstate;
}

void Sequencer::setMasterVolume(float volume) {
    Acquire lock(xMutex);
    masterVolume = fmaxf(0.0f, fminf(1.0f, volume));
}

float Sequencer::getMasterVolume() {
    Acquire lock(xMutex);
    return masterVolume;
}

int16_t Sequencer::readBuffer(int16_t* targetBuffer) {
    Acquire lock(xMutex);
    memcpy(targetBuffer, audioBufferCopy, sizeof(int16_t) * SYNTH_BUFFER_SIZE);
    return SYNTH_BUFFER_SIZE;
}

int16_t Sequencer::readBuffer(int16_t* targetBuffer, uint8_t channelIndex) {
    Acquire lock(xMutex);
    memcpy(targetBuffer, channelAudioBuffersCopy[channelIndex], sizeof(int16_t) * SYNTH_BUFFER_SIZE);
    return SYNTH_BUFFER_SIZE;
}

void Sequencer::singleEventTask() {
    int16_t audioBuffer[SYNTH_BUFFER_SIZE];
    int16_t channelAudioBuffers[CHANNEL_COUNT][SYNTH_BUFFER_SIZE];

    sink->start();
    synth.reset();

    {
        Acquire acquire(xMutex);
        const page_t* page = playstate.track->getPage(playstate.pageIndex);
        for (uint8_t channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
            Pattern* pattern = playstate.track->getPattern(page->patternIndices[channelIndex]);
            event_t event = pattern->getChannelEvent(channelIndex, playstate.eventIndex);
            if (channelIndex == playstate.channelIndex || playstate.channelIndex == -1) {
                if (event.type == EVENT_TYPE_NORMAL) {
                    synth.setFrequency(channelIndex, event.note.toFrequency());
                } else {
                    synth.setOff(channelIndex);
                }
                if (event.waveform != WAVEFORM_CONT) {
                    synth.setWaveform(channelIndex, event.waveform);
                } else {
                    // Identify the waveform from previous events
                    bool found = false;
                    for (int i = playstate.eventIndex - 1; i >= 0; i--) {
                        event_t prevEvent = pattern->getChannelEvent(channelIndex, i);
                        if (prevEvent.waveform != WAVEFORM_CONT) {
                            synth.setWaveform(channelIndex, prevEvent.waveform);
                            found = true;
                            break;
                        }
                    }
                    // If no previous event has a waveform, use the default
                    if (!found) {
                        synth.setWaveform(channelIndex, WAVEFORM_SQUARE);
                    }
                }
                if (event.effect.type != EFFECT_TYPE_NONE) {
                    synth.setEffect(channelIndex, event.effect);
                } else {
                    synth.setEffect(channelIndex, {EFFECT_TYPE_NONE, 0});
                }
                if (event.volume) {
                    synth.setVolume(channelIndex, ((float)event.volume) / MAX_VOLUME);
                } else {
                    synth.setVolume(channelIndex, 1.0f);
                }
            }
        }
    }

    while (1) {
        synth.render(audioBuffer, channelAudioBuffers, SYNTH_BUFFER_SIZE, masterVolume);
        size_t sampWritten = sink->write(audioBuffer, SYNTH_BUFFER_SIZE);
        copyBuffers(audioBuffer, channelAudioBuffers);
        synth.advanceTime(sampWritten);
        {
            Acquire acquire(xMutex);
            if (!playstate.playing) {
                break;
            }
        }
        taskYIELD();
    }

    sink->stop();
}

void Sequencer::multiEventTask() {
    int16_t audioBuffer[SYNTH_BUFFER_SIZE];
    int16_t channelAudioBuffers[CHANNEL_COUNT][SYNTH_BUFFER_SIZE];

    sink->start();
    synth.reset();

    while (1) {
        // Play the page.
        // If we reach the end of the page:
        // - If loopPage is enabled, start playing the page from the beginning
        // - Else, increment the page index
        // If we reach the end of the track:
        // - If loopTrack is enabled, start playing the track from the beginning
        // - Else, stop playing
        // If we stop playing, yield the task

        {
            Acquire acquire(xMutex);
            const page_t* page = playstate.track->getPage(playstate.pageIndex);
            for (uint8_t channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
                Pattern* pattern = playstate.track->getPattern(page->patternIndices[channelIndex]);
                event_t event = pattern->getChannelEvent(channelIndex, playstate.eventIndex);
                if (event.type == EVENT_TYPE_OFF) {
                    synth.setOff(channelIndex);
                } else if (event.type == EVENT_TYPE_NORMAL) {
                    synth.setFrequency(channelIndex, event.note.toFrequency());
                }
                if (event.waveform != WAVEFORM_CONT) {
                    synth.setWaveform(channelIndex, event.waveform);
                }
                if (event.effect.type != EFFECT_TYPE_NONE) {
                    synth.setEffect(channelIndex, event.effect);
                }
                if (event.volume) {
                    synth.setVolume(channelIndex, ((float)event.volume) / MAX_VOLUME);
                }
            }
        }
        // Render samples
        size_t sampTotal = SYNTH_SAMPLE_RATE * 60 / playstate.track->getBPM();
        size_t sampWritten = 0;
        while (sampWritten < sampTotal) {
            float _masterVolume;
            {
                Acquire acquire(xMutex);
                if (!playstate.playing) {
                    break;
                }
                _masterVolume = masterVolume;
            }
            size_t sampToWrite = sampTotal - sampWritten;
            if (sampToWrite > SYNTH_BUFFER_SIZE) {
                sampToWrite = SYNTH_BUFFER_SIZE;
            }
            synth.render(audioBuffer, channelAudioBuffers, sampToWrite, _masterVolume);
            size_t sampWrittenNow = sink->write(audioBuffer, sampToWrite);
            synth.advanceTime(sampWrittenNow);
            sampWritten += sampWrittenNow;
            copyBuffers(audioBuffer, channelAudioBuffers);
            taskYIELD();
        }
        // Increment the event index
        {
            Acquire acquire(xMutex);
            if (!playstate.playing) {
                // Playback stopped while we were waiting
                break;
            }
            playstate.eventIndex++;
            if (playstate.eventIndex >= CHANNEL_SIZE) {
                // End of the page
                playstate.eventIndex = 0;
                int16_t nextPageIndex = playstate.pageIndex + 1;

                if (playstate.loopPage) {
                    // Loop the page
                } else if (nextPageIndex < playstate.track->getPageCount()) {
                    // Play the next page
                    playstate.pageIndex = nextPageIndex;
                } else {
                    // End of the track
                    playstate.pageIndex = 0;
                    if (playstate.loopTrack) {
                        // Loop the track
                    } else {
                        // Stop playing
                        playstate.playing = false;
                        break;
                    }
                }
            }
        }
    }

    sink->stop();
}

void Sequencer::copyBuffers(
    const int16_t* audioBuffer, const int16_t channelAudioBuffers[CHANNEL_COUNT][SYNTH_BUFFER_SIZE]
) {
    Acquire lock(xMutex);
    memcpy(audioBufferCopy, audioBuffer, sizeof(int16_t) * SYNTH_BUFFER_SIZE);
    for (uint8_t channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
        memcpy(
            channelAudioBuffersCopy[channelIndex],
            channelAudioBuffers[channelIndex],
            sizeof(int16_t) * SYNTH_BUFFER_SIZE
        );
    }
}

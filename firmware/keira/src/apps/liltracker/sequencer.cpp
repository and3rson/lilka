#include "sequencer.h"
#include "lilka/serial.h"
#include "utils/acquire.h"

Sequencer::Sequencer(Mixer* mixer) : xMutex(xSemaphoreCreateBinary()), mixer(mixer) {
    playstate.playing = false;
    playstate.eventIndex = 0;
    playstate.pageIndex = 0;
    playstate.track = NULL;
    playstate.loopPage = false;
    playstate.loopTrack = false;

    xTaskCreatePinnedToCore(
        [](void* pvParameters) {
            static_cast<Sequencer*>(pvParameters)->sequencerTask();
            vTaskDelete(NULL);
        },
        "sequencerTask",
        4096,
        this,
        1,
        nullptr,
        1
    );

    xSemaphoreGive(xMutex);
}

Sequencer::~Sequencer() {
    vSemaphoreDelete(xMutex);
}

void Sequencer::play(Track* track, int16_t pageIndex, bool loopTrack) {
    Acquire acquire(xMutex);
    if (playstate.playing) {
        return;
    }
    playstate.track = track;
    playstate.pageIndex = pageIndex;
    playstate.loopPage = false;
    playstate.loopTrack = loopTrack;
    playstate.eventIndex = 0;
    playstate.playing = true;
    mixer->reset();
}

void Sequencer::play(Track* track, bool loopTrack) {
    Acquire acquire(xMutex);
    if (playstate.playing) {
        return;
    }
    playstate.track = track;
    playstate.pageIndex = 0;
    playstate.loopPage = false;
    playstate.loopTrack = loopTrack;
    playstate.eventIndex = 0;
    playstate.playing = true;
    mixer->reset();
}

void Sequencer::stop() {
    Acquire acquire(xMutex);
    playstate.playing = false;
    mixer->stop();
}

seq_state_t Sequencer::getSeqState() {
    Acquire acquire(xMutex);
    return playstate;
}

void Sequencer::sequencerTask() {
    while (1) { // TODO: make this stoppable
        // Play the page.
        // If we reach the end of the page:
        // - If loopPage is enabled, start playing the page from the beginning
        // - Else, increment the page index
        // If we reach the end of the track:
        // - If loopTrack is enabled, start playing the track from the beginning
        // - Else, stop playing
        // If we stop playing, yield the task

        bool playing;
        {
            Acquire acquire(xMutex);
            playing = playstate.playing;
        }

        if (playing) {
            // Play the page
            {
                Acquire acquire(xMutex);
                const page_t* page = playstate.track->getPage(playstate.pageIndex);
                for (int32_t channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
                    Pattern* pattern = playstate.track->getPattern(page->patternIndices[channelIndex]);
                    event_t event = pattern->getChannelEvent(channelIndex, playstate.eventIndex);
                    // waveform_t waveform = pattern->getChannelWaveform(channelIndex);
                    mixer_command_t cmd;
                    cmd.channelIndex = channelIndex;
                    if (event.type == EVENT_TYPE_OFF) {
                        mixer->sendCommand({channelIndex, MIXER_COMMAND_SET_OFF});
                    } else if (event.type == EVENT_TYPE_NORMAL) {
                        cmd.type = MIXER_COMMAND_SET_FREQUENCY;
                        cmd.frequency = event.note.toFrequency();
                        mixer->sendCommand(cmd);
                    }
                    if (event.waveform != WAVEFORM_CONT) {
                        cmd.type = MIXER_COMMAND_SET_WAVEFORM;
                        cmd.waveform = event.waveform;
                        mixer->sendCommand(cmd);
                    }
                    if (event.effect.type != EFFECT_TYPE_NONE) {
                        cmd.type = MIXER_COMMAND_SET_EFFECT;
                        cmd.effect = event.effect;
                        mixer->sendCommand(cmd);
                    }
                    if (event.volume) {
                        cmd.type = MIXER_COMMAND_SET_VOLUME;
                        cmd.volume = ((float)event.volume) / MAX_VOLUME;
                        mixer->sendCommand(cmd);
                    }
                }
            }
            // Wait according to the BPM
            const int32_t msPerBeat = 60000 / playstate.track->getBPM();
            vTaskDelay(msPerBeat / portTICK_PERIOD_MS);
            // Increment the event index
            {
                Acquire acquire(xMutex);
                if (!playstate.playing) {
                    // Playback stopped while we were waiting
                    mixer->stop();
                    continue;
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
                            mixer->stop();
                        }
                    }
                }
            }
        } else {
            // Yield
            taskYIELD();
        }
    }
}

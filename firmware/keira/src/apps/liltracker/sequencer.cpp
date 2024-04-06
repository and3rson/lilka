#include "sequencer.h"

class AcquireSequencer {
public:
    explicit AcquireSequencer(SemaphoreHandle_t xMutex) : xMutex(xMutex) {
        xSemaphoreTake(xMutex, portMAX_DELAY);
    }
    ~AcquireSequencer() {
        xSemaphoreGive(xMutex);
    }

private:
    SemaphoreHandle_t xMutex;
};

Sequencer::Sequencer(Mixer* mixer) : xMutex(xSemaphoreCreateMutex()), mixer(mixer) {
    playstate.playing = false;
    playstate.loop = false;
    playstate.eventIndex = 0;
    playstate.pattern = NULL;
    playstate.track = NULL;

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
}

Sequencer::~Sequencer() {
    vSemaphoreDelete(xMutex);
}

void Sequencer::play(Pattern* pattern, bool loop) {
    AcquireSequencer acquire(xMutex);
    if (playstate.playing) {
        return;
    }
    playstate.track = NULL;
    playstate.pattern = pattern;
    playstate.loop = loop;
    playstate.eventIndex = 0;
    playstate.playing = true;
}

void Sequencer::play(Track* track, bool loop) {
    AcquireSequencer acquire(xMutex);
    if (playstate.playing) {
        return;
    }
    playstate.track = track;
    playstate.pattern = NULL; // TODO: track->getPattern(0);
    playstate.loop = loop;
    playstate.eventIndex = 0;
    playstate.playing = true;
}

void Sequencer::stop() {
    AcquireSequencer acquire(xMutex);
    playstate.playing = false;
    mixer->stop();
}

seq_state_t Sequencer::getSeqState() {
    AcquireSequencer acquire(xMutex);
    return playstate;
}

void Sequencer::sequencerTask() {
    while (1) { // TODO: make this stoppable
        // Play the pattern.
        // If we reach the end of the pattern:
        // - If we have track, get the next pattern from the track and start playing
        // - Else if we don't have a track and loop is enabled, start playing the pattern from the beginning
        // - Else, stop playing
        // If we reach the end of the track:
        // - If loop is enabled, start playing the track from the beginning
        // - Else, stop playing
        // If we stop playing, yield the task

        bool playing;
        {
            AcquireSequencer acquire(xMutex);
            playing = playstate.playing;
        }

        if (playing) {
            // Play the pattern
            {
                AcquireSequencer acquire(xMutex);
                for (int32_t channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
                    event_t event = playstate.pattern->getChannelEvent(channelIndex, playstate.eventIndex);
                    waveform_t waveform = playstate.pattern->getChannelWaveform(channelIndex);
                    mixer_command_t cmd;
                    cmd.channelIndex = channelIndex;
                    if (event.type == EVENT_TYPE_STOP) {
                        mixer->sendCommand({channelIndex, MIXER_COMMAND_CLEAR});
                    } else {
                        if (event.type == EVENT_TYPE_NORMAL) {
                            cmd.type = MIXER_COMMAND_SET_WAVEFORM;
                            cmd.waveform = waveform;
                            mixer->sendCommand(cmd);
                            cmd.type = MIXER_COMMAND_SET_FREQUENCY;
                            cmd.frequency = event.note.toFrequency();
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
            }
            // Wait according to the BPM
            vTaskDelay(MS_PER_BEAT / portTICK_PERIOD_MS);
            // Increment the event index
            {
                AcquireSequencer acquire(xMutex);
                if (!playstate.playing) {
                    // Playback stopped while we were waiting
                    mixer->stop();
                    continue;
                }
                playstate.eventIndex++;
                if (playstate.eventIndex >= CHANNEL_SIZE) {
                    // End of the pattern
                    playstate.eventIndex = 0;
                    if (playstate.track != NULL) {
                        // Playing the track
                        // Get the next pattern from the track
                        // playstate.pattern = playstate.track->getPattern(0); // TODO
                        // playstate.eventIndex = 0;
                    } else if (playstate.loop) {
                        // Loop the pattern
                        playstate.eventIndex = 0;
                    } else {
                        // Stop playing
                        playstate.playing = false;
                        mixer->stop();
                    }
                }
            }
        } else {
            // Yield
            taskYIELD();
        }
    }
}

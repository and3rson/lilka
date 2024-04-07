#pragma once

#include "pattern.h"
#include "track.h"
#include "mixer.h"

typedef struct {
    Track* track;
    int16_t patternIndex; // TODO: use int16_t consistently for indices
    int16_t eventIndex;
    bool loopPattern;
    bool loopTrack;
    bool playing;
} seq_state_t;

class Sequencer {
public:
    explicit Sequencer(Mixer* mixer);
    ~Sequencer();
    // void play(Track* track, int16_t patternIndex, int16_t eventIndex); // TODO - implement this instead of using mixer directly from LilTrackerApp?
    void play(Track* track, int16_t patternIndex, bool loopPattern);
    void play(Track* track, bool loopTrack);
    void stop();
    seq_state_t getSeqState();

private:
    void sequencerTask();
    Mixer* mixer;
    seq_state_t playstate;
    SemaphoreHandle_t xMutex;
};

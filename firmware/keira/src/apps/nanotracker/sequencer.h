#pragma once

#include "pattern.h"
#include "track.h"
#include "mixer.h"

typedef struct {
    Track* track;
    Pattern* pattern;
    bool loop;
    int32_t eventIndex;
    bool playing;
} seq_state_t;

class Sequencer {
public:
    explicit Sequencer(Mixer* mixer);
    ~Sequencer();
    void play(Pattern* pattern, int32_t eventIndex);
    void play(Pattern* pattern, bool loop);
    void play(Track* track, bool loop);
    void stop();
    seq_state_t getSeqState();

private:
    void sequencerTask();
    Mixer* mixer;
    seq_state_t playstate;
    SemaphoreHandle_t xMutex;
};

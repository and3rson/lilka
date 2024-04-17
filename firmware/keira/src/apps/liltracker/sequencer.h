#pragma once

#include "pattern.h"
#include "track.h"
#include "synth.h"
#include "sink.h"

typedef struct {
    Track* track;
    uint16_t pageIndex;
    int16_t channelIndex;
    uint16_t eventIndex;
    bool loopPage;
    bool loopTrack;
    bool playing;
} seq_state_t;

class Sequencer {
public:
    explicit Sequencer(Sink* sink = NULL);
    ~Sequencer();
    void setSink(Sink* sink);
    void play(Track* track, uint16_t pageIndex, int8_t channelIndex, uint16_t eventIndex);
    void play(Track* track, uint16_t pageIndex, bool loopTrack);
    void stop();
    seq_state_t getSeqState();
    float getMasterVolume();
    void setMasterVolume(float volume);
    int16_t readBuffer(int16_t* targetBuffer);
    int16_t readBuffer(int16_t* targetBuffer, uint8_t channelIndex);

private:
    void singleEventTask();
    void multiEventTask();
    void copyBuffers(const int16_t* audioBuffer, const int16_t channelAudioBuffers[CHANNEL_COUNT][SYNTH_BUFFER_SIZE]);
    Synth synth;
    Sink* sink;
    seq_state_t playstate;
    SemaphoreHandle_t xMutex;
    SemaphoreHandle_t xPlaying;
    float masterVolume;

    int16_t audioBufferCopy[SYNTH_BUFFER_SIZE];
    int16_t channelAudioBuffersCopy[CHANNEL_COUNT][SYNTH_BUFFER_SIZE];
};

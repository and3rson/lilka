#pragma once

#include <vector>

#include "pattern.h"

// Track consists of segments that are played sequentially.
// Each segment tells which patterns to play.
typedef struct {
    int16_t patterns[CHANNEL_COUNT];
} segment_t;

// Track is the top-level container for a song.
// It contains patterns and segments.
// It can be saved to and loaded from a buffer.
class Track {
public:
    Track();
    int16_t getPatternCount();
    void setPatternCount(int16_t count);
    Pattern* getPattern(int16_t index);
    int16_t getSegmentCount();
    void setSegmentCount(int16_t count);
    segment_t* getSegment(int16_t index);

    int32_t writeToBuffer(uint8_t* data);
    int32_t readFromBuffer(const uint8_t* data);

private:
    std::vector<Pattern> patterns;
    std::vector<segment_t> segments;
};

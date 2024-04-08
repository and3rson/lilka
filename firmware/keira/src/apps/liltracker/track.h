#pragma once

#include <vector>

#include "pattern.h"

// Track consists of pages that are played sequentially.
// Each page tells which patterns to play.
typedef struct {
    int16_t patternIndices[CHANNEL_COUNT];
} page_t;

// Track is the top-level container for a song.
// It contains patterns and pages.
// It can be saved to and loaded from a buffer.
class Track {
public:
    Track();
    int16_t getPatternCount();
    void setPatternCount(int16_t count);
    Pattern* getPattern(int16_t index);
    int16_t getPageCount();
    void setPageCount(int16_t count);
    page_t* getPage(int16_t index);

    int32_t writeToBuffer(uint8_t* data);
    int32_t readFromBuffer(const uint8_t* data);

private:
    std::vector<Pattern> patterns;
    std::vector<page_t> pages;
    SemaphoreHandle_t xMutex;
};

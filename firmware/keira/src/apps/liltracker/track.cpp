#include <string.h>

#include "track.h"
#include "utils/acquire.h"

// TODO: Limit page count
// TODO: Limit pattern count
// TODO: Allow unneeded patterns to be removed somehow

#define CLAMP(x, min, max) ((x) < (min) ? (min) : (x) > (max) ? (max) : (x))
#define MAX(a, b)          ((a) > (b) ? (a) : (b))

#define WRITE_TO_BUFFER(buffer, value)              \
    memcpy(&buffer[offset], &value, sizeof(value)); \
    offset += sizeof(value);

#define READ_FROM_BUFFER(value, buffer)             \
    memcpy(&value, &buffer[offset], sizeof(value)); \
    offset += sizeof(value);

Track::Track(int16_t bpm) : xMutex(xSemaphoreCreateRecursiveMutex()), bpm(bpm) {
    // Create default page and pattern
    pages.resize(4);
    patterns.resize(4);
    xSemaphoreGive(xMutex);
}

int16_t Track::getPatternCount() {
    Acquire acquire(xMutex, true);
    return patterns.size();
}

void Track::setPatternCount(int16_t count) {
    Acquire acquire(xMutex, true);
    patterns.resize(MAX(count, 1));
}

Pattern* Track::getPattern(int16_t index) {
    Acquire acquire(xMutex, true);
    // Auto-resize if index is out of bounds
    if (index >= getPatternCount()) {
        setPatternCount(index + 1);
    }
    return &patterns[index];
}

int16_t Track::getPageCount() {
    Acquire acquire(xMutex, true);
    return pages.size();
}

void Track::setPageCount(int16_t count) {
    Acquire acquire(xMutex, true);
    pages.resize(MAX(count, 1));
}

page_t* Track::getPage(int16_t index) {
    Acquire acquire(xMutex, true);
    // Auto-resize if index is out of bounds
    if (index >= getPageCount()) {
        setPageCount(index + 1);
    }
    return &pages[index];
}

int16_t Track::getBPM() {
    Acquire acquire(xMutex, true);
    return bpm;
}

void Track::setBPM(int16_t bpm) {
    Acquire acquire(xMutex, true);
    this->bpm = CLAMP(bpm, 30, 900);
}

void Track::reset() {
    Acquire acquire(xMutex, true);
    pages.resize(0);
    patterns.resize(0);
    pages.resize(4);
    patterns.resize(4);
}

int32_t Track::calculateWriteBufferSize() {
    Acquire acquire(xMutex, true);
    int32_t size = 32 * sizeof(int16_t); // Header (signature + reserved), BPM, pattern count, page count
    for (int16_t i = 0; i < getPatternCount(); i++) {
        size += getPattern(i)->calculateWriteBufferSize();
    }
    size += getPageCount() * CHANNEL_COUNT * sizeof(int16_t); // Pages
    return size;
}

int32_t Track::writeToBuffer(uint8_t* data) {
    Acquire acquire(xMutex, true);
    int32_t offset = 0;

    // Write header signature ("LILT")
    data[offset++] = 'L';
    data[offset++] = 'I';
    data[offset++] = 'L';
    data[offset++] = 'T';

    // Write header reserved bytes until offset 32
    while (offset < 32) {
        data[offset++] = 0;
    }

    // Write BPM
    WRITE_TO_BUFFER(data, bpm);

    // Find largest index of pattern that is used in a page.
    // We won't write trailing patterns that are not used.
    int16_t largestUsedPatternIndex = 0; // There is always at least one pattern
    for (int16_t i = 0; i < getPageCount(); i++) {
        for (int8_t j = 0; j < CHANNEL_COUNT; j++) {
            largestUsedPatternIndex = MAX(largestUsedPatternIndex, getPage(i)->patternIndices[j]);
        }
    }

    // Write patterns
    int16_t patternCount = largestUsedPatternIndex + 1;
    WRITE_TO_BUFFER(data, patternCount);
    for (int16_t i = 0; i < patternCount; i++) {
        offset += getPattern(i)->writeToBuffer(&data[offset]);
    }

    // Write pages
    int16_t pageCount = getPageCount();
    WRITE_TO_BUFFER(data, pageCount);
    for (int16_t i = 0; i < pageCount; i++) {
        for (int8_t j = 0; j < CHANNEL_COUNT; j++) {
            WRITE_TO_BUFFER(data, getPage(i)->patternIndices[j]);
        }
    }

    return offset;
}

int32_t Track::readFromBuffer(const uint8_t* data) {
    Acquire acquire(xMutex, true);
    int32_t offset = 0;

    // Read header signature ("LIL")
    if (data[offset++] != 'L' || data[offset++] != 'I' || data[offset++] != 'L' || data[offset++] != 'T') {
        return -1;
    }

    // Skip header reserved bytes until offset 32
    offset = 32;

    // Read BPM
    READ_FROM_BUFFER(bpm, data);

    // Read patterns
    int16_t patternCount;
    READ_FROM_BUFFER(patternCount, data);
    setPatternCount(patternCount);
    for (int16_t i = 0; i < patternCount; i++) {
        offset += getPattern(i)->readFromBuffer(&data[offset]);
    }

    // Read pages
    int16_t pageCount;
    READ_FROM_BUFFER(pageCount, data);
    setPageCount(pageCount);
    for (int16_t i = 0; i < pageCount; i++) {
        for (int8_t j = 0; j < CHANNEL_COUNT; j++) {
            READ_FROM_BUFFER(getPage(i)->patternIndices[j], data);
        }
    }

    return offset;
}

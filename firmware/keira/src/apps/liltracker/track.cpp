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

int16_t Track::getUsedPatternCount() {
    // Similar to getPatternCount, but only counts patterns that are used in pages
    Acquire acquire(xMutex, true);
    int16_t count = 1;
    for (int16_t i = 0; i < getPageCount(); i++) {
        for (int8_t j = 0; j < CHANNEL_COUNT; j++) {
            count = MAX(count, getPage(i)->patternIndices[j] + 1);
        }
    }
    return count;
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
    int32_t size = 0;
    size += 4; // Signature
    size += 32; // Reserved
    size += sizeof(int16_t); // BPM
    size += sizeof(int16_t); // Pattern count
    // Patterns
    for (int16_t i = 0; i < getUsedPatternCount(); i++) {
        size += getPattern(i)->calculateWriteBufferSize();
    }
    size += sizeof(int16_t); // Page count
    // Pages
    size += getPageCount() * CHANNEL_COUNT * sizeof(int16_t);
    return size;
}

int32_t Track::writeToBuffer(uint8_t* data) {
    Acquire acquire(xMutex, true);
    int32_t offset = 0;

    // Write signature ("LILT")
    data[offset++] = 'L';
    data[offset++] = 'I';
    data[offset++] = 'L';
    data[offset++] = 'T';

    // Write 32 reserved bytes
    for (int8_t i = 0; i < 32; i++) {
        data[offset++] = 0;
    }

    // Write BPM
    WRITE_TO_BUFFER(data, bpm);

    // Write pattern count
    int16_t patternCount = getUsedPatternCount();
    WRITE_TO_BUFFER(data, patternCount);
    // Write patterns
    for (int16_t i = 0; i < patternCount; i++) {
        offset += getPattern(i)->writeToBuffer(&data[offset]);
    }

    // Write page count
    int16_t pageCount = getPageCount();
    WRITE_TO_BUFFER(data, pageCount);
    // Write pages
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

    // Read signature ("LILT")
    if (data[offset++] != 'L' || data[offset++] != 'I' || data[offset++] != 'L' || data[offset++] != 'T') {
        return -1;
    }

    // Skip 32 reserved bytes
    offset += 32;

    // Read BPM
    READ_FROM_BUFFER(bpm, data);

    // Read pattern count
    int16_t patternCount;
    READ_FROM_BUFFER(patternCount, data);
    setPatternCount(patternCount);
    // Read patterns
    for (int16_t i = 0; i < patternCount; i++) {
        offset += getPattern(i)->readFromBuffer(&data[offset]);
    }

    // Read page count
    int16_t pageCount;
    READ_FROM_BUFFER(pageCount, data);
    setPageCount(pageCount);
    // Read pages
    for (int16_t i = 0; i < pageCount; i++) {
        for (int8_t j = 0; j < CHANNEL_COUNT; j++) {
            READ_FROM_BUFFER(getPage(i)->patternIndices[j], data);
        }
    }

    return offset;
}

#include <string.h>

#include "track.h"

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

class AcquireTrack {
public:
    explicit AcquireTrack(SemaphoreHandle_t xMutex) {
        this->xMutex = xMutex;
        xSemaphoreTakeRecursive(xMutex, portMAX_DELAY);
    }
    ~AcquireTrack() {
        xSemaphoreGiveRecursive(xMutex);
    }

private:
    SemaphoreHandle_t xMutex;
};

Track::Track(int16_t bpm) : xMutex(xSemaphoreCreateRecursiveMutex()), bpm(bpm) {
    // Create default page and pattern
    pages.resize(4);
    patterns.resize(4);
    xSemaphoreGive(xMutex);
}

int16_t Track::getPatternCount() {
    AcquireTrack acquire(xMutex);
    return patterns.size();
}

void Track::setPatternCount(int16_t count) {
    AcquireTrack acquire(xMutex);
    patterns.resize(MAX(count, 1));
}

Pattern* Track::getPattern(int16_t index) {
    AcquireTrack acquire(xMutex);
    // Auto-resize if index is out of bounds
    if (index >= getPatternCount()) {
        setPatternCount(index + 1);
    }
    return &patterns[index];
}

int16_t Track::getPageCount() {
    AcquireTrack acquire(xMutex);
    return pages.size();
}

void Track::setPageCount(int16_t count) {
    AcquireTrack acquire(xMutex);
    pages.resize(MAX(count, 1));
}

page_t* Track::getPage(int16_t index) {
    AcquireTrack acquire(xMutex);
    // Auto-resize if index is out of bounds
    if (index >= getPageCount()) {
        setPageCount(index + 1);
    }
    return &pages[index];
}

int16_t Track::getBPM() {
    AcquireTrack acquire(xMutex);
    return bpm;
}

void Track::setBPM(int16_t bpm) {
    AcquireTrack acquire(xMutex);
    this->bpm = CLAMP(bpm, 30, 900);
}

int32_t Track::calculateWriteBufferSize() {
    AcquireTrack acquire(xMutex);
    int32_t size = 3 * sizeof(int16_t); // Header, BPM, pattern count, page count
    for (int16_t i = 0; i < getPatternCount(); i++) {
        size += getPattern(i)->calculateWriteBufferSize();
    }
    size += getPageCount() * CHANNEL_COUNT * sizeof(int16_t); // Pages
    return size;
}

int32_t Track::writeToBuffer(uint8_t* data) {
    AcquireTrack acquire(xMutex);
    int32_t offset = 0;

    // Write header ("LIL")
    data[offset++] = 'L';
    data[offset++] = 'I';
    data[offset++] = 'L';

    // Write BPM
    WRITE_TO_BUFFER(data, bpm);

    // Write patterns
    int16_t patternCount = getPatternCount();
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
    AcquireTrack acquire(xMutex);
    int32_t offset = 0;

    // Read header ("LIL")
    if (data[offset++] != 'L' || data[offset++] != 'I' || data[offset++] != 'L') {
        return -1;
    }

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
#include "track.h"

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

Track::Track() : xMutex(xSemaphoreCreateRecursiveMutex()) {
    // Create default page and pattern
    pages.resize(1);
    patterns.resize(1);
    xSemaphoreGive(xMutex);
}

int16_t Track::getPatternCount() {
    AcquireTrack acquire(xMutex);
    return patterns.size();
}

void Track::setPatternCount(int16_t count) {
    AcquireTrack acquire(xMutex);
    patterns.resize(count);
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
    pages.resize(count);
}

page_t* Track::getPage(int16_t index) {
    AcquireTrack acquire(xMutex);
    return &pages[index];
}

int32_t Track::writeToBuffer(uint8_t* data) {
    AcquireTrack acquire(xMutex);
    int32_t offset = 0;
    // Write header ("LIL")
    data[offset++] = 'L';
    data[offset++] = 'I';
    data[offset++] = 'L';
    *reinterpret_cast<int16_t*>(&data[offset]) = getPatternCount();
    offset += sizeof(int16_t);
    for (int16_t i = 0; i < getPatternCount(); i++) {
        offset += getPattern(i)->writeToBuffer(&data[offset]);
    }
    *reinterpret_cast<int16_t*>(&data[offset]) = getPageCount();
    offset += sizeof(int16_t);
    for (int16_t i = 0; i < getPageCount(); i++) {
        for (int8_t j = 0; j < CHANNEL_COUNT; j++) {
            *reinterpret_cast<int16_t*>(&data[offset]) = (*getPage(i)).patternIndexes[j];
            offset += sizeof(int16_t);
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
    int16_t patternCount = *reinterpret_cast<const int16_t*>(&data[offset]);
    offset += sizeof(int16_t);
    setPatternCount(patternCount);
    for (int16_t i = 0; i < patternCount; i++) {
        offset += getPattern(i)->readFromBuffer(&data[offset]);
    }
    int16_t pageCount = *reinterpret_cast<const int16_t*>(&data[offset]);
    offset += sizeof(int16_t);
    setPageCount(pageCount);
    for (int16_t i = 0; i < pageCount; i++) {
        for (int8_t j = 0; j < CHANNEL_COUNT; j++) {
            (*getPage(i)).patternIndexes[j] = *reinterpret_cast<const int16_t*>(&data[offset]);
            offset += sizeof(int16_t);
        }
    }
    return offset;
}

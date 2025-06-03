#include <string.h>

#include "lilka/serial.h"
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
    reset();
    xSemaphoreGive(xMutex);
}

Track::~Track() {
    {
        Acquire acquire(xMutex, true);
        for (Pattern* pattern : patterns) {
            delete pattern;
        }
        for (page_t* page : pages) {
            delete page;
        }
    }
    vSemaphoreDelete(xMutex);
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
        for (uint8_t j = 0; j < CHANNEL_COUNT; j++) {
            count = MAX(count, getPage(i)->patternIndices[j] + 1);
        }
    }
    return count;
}

void Track::setPatternCount(int16_t count) {
    Acquire acquire(xMutex, true);
    count = MAX(count, 1);
    while (getPatternCount() > count) {
        delete patterns.back();
        patterns.pop_back();
    }
    while (getPatternCount() < count) {
        patterns.push_back(new Pattern());
    }
}

Pattern* Track::getPattern(int16_t index) {
    Acquire acquire(xMutex, true);
    if (index >= getPatternCount()) {
        // Auto-resize if index is out of bounds
        // This will happen when user tries to access pattern index that is not yet allocated
        setPatternCount(index + 1);
    }
    return patterns[index];
}

int16_t Track::getPageCount() {
    Acquire acquire(xMutex, true);
    return pages.size();
}

void Track::setPageCount(int16_t count) {
    Acquire acquire(xMutex, true);
    count = MAX(count, 1);
    while (getPageCount() > count) {
        delete pages.back();
        pages.pop_back();
    }
    while (getPageCount() < count) {
        page_t* lastPage = getPageCount() > 0 ? pages.back() : NULL;
        pages.push_back(new page_t());
        for (uint8_t i = 0; i < CHANNEL_COUNT; i++) {
            if (lastPage != NULL) {
                // Copy pattern indices from last page
                pages.back()->patternIndices[i] = lastPage->patternIndices[i];
            } else {
                // Set pattern indices to 0 (should not happen)
                lilka::serial.err(
                    "Track::setPageCount: suspiciously creating first page with all pattern indices set to 0, index %d",
                    i
                );
            }
        }
    }
}

page_t* Track::getPage(int16_t index) {
    Acquire acquire(xMutex, true);
    if (index >= getPageCount()) {
        // Auto-resize if index is out of bounds
        // (This should not happen in normal operation, since page count is controlled by UI)
        lilka::serial.err("Track::getPage: suspiciously resizing page count to %d", index + 1);
        setPageCount(index + 1);
    }
    return pages[index];
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
    // Create default pages and patterns
    constexpr uint16_t initialPageCount = 4;
    constexpr uint16_t initialPatternCount = 4;
    while (pages.size()) {
        delete pages.back();
        pages.pop_back();
    }
    while (pages.size() < initialPageCount) {
        pages.push_back(new page_t());
    }
    while (patterns.size()) {
        delete patterns.back();
        patterns.pop_back();
    }
    while (patterns.size() < initialPatternCount) {
        patterns.push_back(new Pattern());
    }
}

int32_t Track::calculateWriteBufferSize() {
    Acquire acquire(xMutex, true);
    int32_t size = 0;
    size += 64; // Signature, version, reserved bytes
    size += sizeof(bpm); // BPM
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

    // Write signature ("LIL") & version (1)
    data[offset++] = 'L';
    data[offset++] = 'I';
    data[offset++] = 'L';
    data[offset++] = 1;

    // Write reserved bytes until position 64
    while (offset < 64) {
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
        for (uint8_t j = 0; j < CHANNEL_COUNT; j++) {
            WRITE_TO_BUFFER(data, getPage(i)->patternIndices[j]);
        }
    }

    return offset;
}

int32_t Track::readFromBuffer(const uint8_t* data) {
    Acquire acquire(xMutex, true);
    int32_t offset = 0;

    // Read signature ("LIL")
    if (data[offset++] != 'L' || data[offset++] != 'I' || data[offset++] != 'L') {
        return -1;
    }
    // offset++; // Skip version

    // Skip reserved bytes until position 64
    offset = 64;

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
        for (uint8_t j = 0; j < CHANNEL_COUNT; j++) {
            READ_FROM_BUFFER(getPage(i)->patternIndices[j], data);
        }
    }

    return offset;
}

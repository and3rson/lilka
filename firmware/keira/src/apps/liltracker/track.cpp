#include "track.h"

Track::Track() {
    // Create default segment and pattern
    setSegmentCount(1);
    setPatternCount(1);
}

int16_t Track::getPatternCount() {
    return patterns.size();
}

void Track::setPatternCount(int16_t count) {
    patterns.resize(count);
}

Pattern* Track::getPattern(int16_t index) {
    return &patterns[index];
}

int16_t Track::getSegmentCount() {
    return segments.size();
}

void Track::setSegmentCount(int16_t count) {
    segments.resize(count);
}

segment_t* Track::getSegment(int16_t index) {
    return &segments[index];
}

int32_t Track::writeToBuffer(uint8_t* data) {
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
    *reinterpret_cast<int16_t*>(&data[offset]) = getSegmentCount();
    offset += sizeof(int16_t);
    for (int16_t i = 0; i < getSegmentCount(); i++) {
        for (int8_t j = 0; j < CHANNEL_COUNT; j++) {
            *reinterpret_cast<int16_t*>(&data[offset]) = (*getSegment(i)).patterns[j];
            offset += sizeof(int16_t);
        }
    }
    return offset;
}

int32_t Track::readFromBuffer(const uint8_t* data) {
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
    int16_t segmentCount = *reinterpret_cast<const int16_t*>(&data[offset]);
    offset += sizeof(int16_t);
    setSegmentCount(segmentCount);
    for (int16_t i = 0; i < segmentCount; i++) {
        for (int8_t j = 0; j < CHANNEL_COUNT; j++) {
            (*getSegment(i)).patterns[j] = *reinterpret_cast<const int16_t*>(&data[offset]);
            offset += sizeof(int16_t);
        }
    }
    return offset;
}

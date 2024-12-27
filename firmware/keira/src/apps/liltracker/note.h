#pragma once

#include <stdint.h>

#include "config.h"

typedef struct noteinfo_t {
    uint8_t index; // [0;11]
    uint8_t octave; // [0;9]
    char* toStr();
    void add(int16_t semitoneCount);
    void fromFrequency(float frequency);
    float toFrequency();
} PACKED noteinfo_t;

// Lookup table for powers of 2 in range [-10;10] with a step of 1/12
extern float pow2table[12 * 10 * 2 + 1];

inline float modulate_frequency(float frequency, float semitoneCount) {
    int index = semitoneCount + 12 * 10;
    if (index < 0) {
        index = 0;
    } else if (index > 12 * 10 * 2) {
        index = 12 * 10 * 2;
    }
    return frequency * pow2table[index];
}

#define NOTE_ZERO \
    { 0, 0 }

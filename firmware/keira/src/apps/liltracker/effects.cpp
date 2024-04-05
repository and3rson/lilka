#include <math.h>
#include <stdio.h>

#include "effects.h"
#include "note.h"

void effect_none(float* time, float* frequency, float* amplitude, float* phase, effect_t effect) {
    (void)time;
    (void)frequency;
    (void)amplitude;
    (void)phase;
    (void)effect;
}

void effect_arpeggio(float* time, float* frequency, float* amplitude, float* phase, uint8_t param) {
    // This effect arpeggiates the note by changing the frequency of the note at a fixed interval.
    // It mimics NES arpeggio that consists of up to 3 notes.

    constexpr int8_t count = 3; // 3 notes

    int8_t note2offset = (param & 0xF0) >> 4;
    int8_t note3offset = (param & 0x0F);

    // duration of each note in milliseconds is 1/60 of a second
    float stepDurationMs = 1000.0f / 60.0f;

    // Calculate current arpeggio step
    int8_t step = ((int64_t)(*time / (stepDurationMs / 1000.0f))) % count;

    if (step == 0) {
        // No change
    } else if (step == 1) {
        // Use the second note
        *frequency = modulate_frequency(*frequency, note2offset);
    } else if (step == 2) {
        // Use the third note
        *frequency = modulate_frequency(*frequency, note3offset);
    }
}

void effect_vibrato(float* time, float* frequency, float* amplitude, float* phase, uint8_t param) {
    (void)time;
    (void)frequency;
    (void)amplitude;
    (void)phase;
    (void)param;
}

void effect_tremolo(float* time, float* frequency, float* amplitude, float* phase, uint8_t param) {
    (void)time;
    (void)frequency;
    (void)amplitude;
    (void)phase;
    (void)param;
}

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

void effect_arpeggio(float* time, float* frequency, float* amplitude, float* phase, effect_t effect) {
    // This effect arpeggiates the note by changing the frequency of the note at a fixed interval

    // param1: number of notes in the arpeggio
    int8_t count = effect.param1;
    // param2: step size in semitones
    int8_t stepSize = effect.param2;
    // param3: duration of each note in milliseconds
    float stepDurationMs = effect.param3;

    // Calculate current arpeggio step
    int8_t step = ((int64_t)(*time / (stepDurationMs / 1000.0f))) % count;

    // Set new frequency
    *frequency = modulate_frequency(*frequency, step * stepSize);
}

void effect_vibrato(float* time, float* frequency, float* amplitude, float* phase, effect_t effect) {
    (void)time;
    (void)frequency;
    (void)amplitude;
    (void)phase;
    (void)effect;
}

void effect_tremolo(float* time, float* frequency, float* amplitude, float* phase, effect_t effect) {
    (void)time;
    (void)frequency;
    (void)amplitude;
    (void)phase;
    (void)effect;
}

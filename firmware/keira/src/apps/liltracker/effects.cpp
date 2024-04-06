#include <math.h>
#include <stdio.h>

#include "effects.h"
#include "note.h"

void effect_none(float* time, float* frequency, float* amplitude, float* phase, uint8_t param) {
    (void)time;
    (void)frequency;
    (void)amplitude;
    (void)phase;
    (void)param;
}

void effect_arpeggio(float* time, float* frequency, float* amplitude, float* phase, uint8_t param) {
    // This effect arpeggiates the note by changing the frequency of the note at a fixed interval.
    // It mimics NES arpeggio that consists of up to 3 notes.

    constexpr int8_t count = 3; // 3 notes

    uint8_t note2offset = (param & 0xF0) >> 4;
    uint8_t note3offset = (param & 0x0F);

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
    // This effect modulates the frequency of the note with a sine wave.
    // Upper nibble of the parameter is the speed of the vibrato (in Hz)
    // Lower nibble of the parameter is the depth of the vibrato (in cents)

    uint8_t vibratoFrequency = (param & 0xF0) >> 4;
    uint8_t vibratoDepth = (param & 0x0F);

    // Calculate the vibrato
    float vibrato = sinf(*time * 2.0f * M_PI * vibratoFrequency) * vibratoDepth / 100.0f;

    // Apply the vibrato
    *frequency = modulate_frequency(*frequency, vibrato);
}

void effect_tremolo(float* time, float* frequency, float* amplitude, float* phase, uint8_t param) {
    // This effect modulates the amplitude of the note with a sine wave.
    // Upper nibble of the parameter is the speed of the tremolo (in Hz)
    // Lower nibble of the parameter is the depth of the tremolo (0 to 15, 0 = no tremolo, 15 = max tremolo)

    uint8_t tremoloFrequency = (param & 0xF0) >> 4;
    uint8_t tremoloDepth = (param & 0x0F);

    // Calculate the tremolo into a range of 0.0 to 1.0
    float tremolo = (sinf(*time * 2.0f * M_PI * tremoloFrequency) + 1.0f) / 2.0f;

    // Apply the tremolo
    *amplitude = *amplitude * (1.0f - tremolo * tremoloDepth / 15.0f);
}

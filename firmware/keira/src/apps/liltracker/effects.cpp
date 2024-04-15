#include <math.h>
#include <lilka/fmath.h>

#include "effects.h"
#include "note.h"

void effect_none(float time, float relTime, float* frequency, float* amplitude, float* phase, uint8_t param) {
    (void)time;
    (void)relTime;
    (void)frequency;
    (void)amplitude;
    (void)phase;
    (void)param;
}

void effect_arpeggio(float time, float relTime, float* frequency, float* amplitude, float* phase, uint8_t param) {
    // This effect arpeggiates the note by changing the frequency of the note at a fixed interval.
    // It mimics NES arpeggio that consists of up to 3 notes.

    (void)relTime;
    (void)amplitude;
    (void)param;

    constexpr uint8_t count = 3; // 3 notes

    uint8_t note2offset = (param & 0xF0) >> 4;
    uint8_t note3offset = (param & 0x0F);

    // duration of each note in milliseconds is 1/60 of a second
    float stepDurationMs = 1000.0f / 60.0f;

    // Calculate current arpeggio step
    uint8_t step = ((int64_t)(time / (stepDurationMs / 1000.0f))) % count;

    if (step == 0) {
        // No change
    } else if (step == 1) {
        // Use the second note
        *frequency = modulate_frequency(*frequency, note2offset);
    } else {
        // Use the third note
        *frequency = modulate_frequency(*frequency, note3offset);
    }
}

void effect_vibrato(float time, float relTime, float* frequency, float* amplitude, float* phase, uint8_t param) {
    // This effect modulates the frequency of the note with a sine wave.
    // Upper nibble of the parameter is the speed of the vibrato (in Hz)
    // Lower nibble of the parameter is the depth of the vibrato (0 to 15, 0 = no vibrato, 15 = one full semitone vibrato)

    (void)relTime;
    (void)frequency;
    (void)amplitude;

    uint8_t vibratoFrequency = (param & 0xF0) >> 4;
    uint8_t vibratoDepth = param & 0x0F;

    // Calculate the depth of vibrato in terms of radians
    // TODO: This is not really correct: 1 unit of phase is not 1 semitone, but 1 Hz.
    // So we should convert below value to Hz based on the current frequency of the note.
    float phaseModulation = vibratoDepth / 15.0f * lilka::fSin360(time * vibratoFrequency * 360);

    // Apply the vibrato phase modulation
    *phase += phaseModulation;
}

void effect_tremolo(float time, float relTime, float* frequency, float* amplitude, float* phase, uint8_t param) {
    // This effect modulates the amplitude of the note with a sine wave.
    // Upper nibble of the parameter is the speed of the tremolo (in Hz)
    // Lower nibble of the parameter is the depth of the tremolo (0 to 15, 0 = no tremolo, 15 = max tremolo)

    (void)time;
    (void)relTime;
    (void)frequency;
    (void)phase;

    uint8_t tremoloFrequency = (param & 0xF0) >> 4;
    uint8_t tremoloDepth = (param & 0x0F);

    // Calculate the tremolo into a range of 0.0 to 1.0
    float tremolo = (lilka::fSin360(time * tremoloFrequency * 360) + 1.0f) / 2.0f;

    // Apply the tremolo
    *amplitude = *amplitude * (1.0f - tremolo * tremoloDepth / 15.0f);
}

void effect_volume_slide(float time, float relTime, float* frequency, float* amplitude, float* phase, uint8_t param) {
    // This effect slides the volume of the note.
    // If upper nibble is 0, lower nibble is slide down speed (1s / value)
    // If lower nibble is 0, upper nibble is slide up speed (1s / value)
    // relTime is the relative time of when the effect started

    (void)time;
    (void)frequency;
    (void)phase;

    uint8_t slideUpSpeed = (param & 0xF0) >> 4;
    uint8_t slideDownSpeed = param & 0x0F;

    float newAmplitude = *amplitude;

    if (slideUpSpeed == 0) {
        // Slide down
        newAmplitude = 1.0f - (relTime / (1.0f / slideDownSpeed));
    } else if (slideDownSpeed == 0) {
        // Slide up
        newAmplitude = relTime / (1.0f / slideUpSpeed);
    }

    // Clamp the amplitude
    // TODO: We should probably override the amplitude, and disable this effect if event volume is set in tracker?
    *amplitude *= fminf(fmaxf(newAmplitude, 0.0f), 1.0f);
}

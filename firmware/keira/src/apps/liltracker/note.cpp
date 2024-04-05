#include <math.h>
#include <stdio.h>

#include "note.h"

const float A4_FREQUENCY = 440.0;

const char* note_names[] = {"C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-"};

// Function to convert frequency to the nearest musical note, starting from A0 and ending with B9
#define A4_FREQUENCY 440.0

// Define the number of notes in an octave
#define NOTES_PER_OCTAVE 12

// Define the number of octaves
#define OCTAVES 10

// Define the number of notes in total
#define TOTAL_NOTES (NOTES_PER_OCTAVE * OCTAVES)

void noteinfo_t::fromFrequency(float frequency) {
    // 16.35 Hz is the lowest note, which is C0 (returned as octave 0, index 0)
    int absIndex = round(log2(frequency / A4_FREQUENCY) * NOTES_PER_OCTAVE) + 57;

    if (absIndex < 0) {
        absIndex = 0;
    }
    if (absIndex >= TOTAL_NOTES) {
        absIndex = TOTAL_NOTES - 1;
    }

    index = absIndex % NOTES_PER_OCTAVE;
    octave = absIndex / NOTES_PER_OCTAVE;
}

// Function to convert musical note to frequency
float noteinfo_t::toFrequency() {
    // C0 is the lowest note, which is index 0 (as opposed to MIDI's 12)
    int32_t absIndex = octave * NOTES_PER_OCTAVE + index;
    if (absIndex < 0) {
        absIndex = 0;
    }

    return A4_FREQUENCY * pow(2, ((float)(absIndex - 57)) / 12.0);
}

float modulate_frequency(float frequency, int16_t semitoneCount) {
    // Calculate the frequency ratio for semitone modulation
    float frequencyRatio = pow(2.0, (float)semitoneCount / 12.0);

    // Modulate the frequency
    float modulatedFrequency = frequency * frequencyRatio;

    return modulatedFrequency;
}

// Function to convert note to string
char* noteinfo_t::toStr() {
    static char buffer[4];
    sprintf(buffer, "%s%d", note_names[index], octave);
    return buffer;
}

void noteinfo_t::add(int16_t semitoneCount) {
    // Calculate the new index
    int absIndex = octave * NOTES_PER_OCTAVE + index;
    absIndex += semitoneCount;

    if (absIndex < 0) {
        absIndex = 0;
    }
    if (absIndex >= TOTAL_NOTES) {
        absIndex = TOTAL_NOTES - 1;
    }

    // Update the note
    index = absIndex % NOTES_PER_OCTAVE;
    octave = absIndex / NOTES_PER_OCTAVE;
}

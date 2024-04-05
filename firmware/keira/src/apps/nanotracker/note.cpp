#include <math.h>
#include <stdio.h>

#include "note.h"

const double A4_FREQUENCY = 440.0;

const char* note_names[] = {"C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-"};

// Function to convert frequency to the nearest musical note, starting from A0 and ending with B9
#define A4_FREQUENCY 440.0

// Define the number of notes in an octave
#define NOTES_PER_OCTAVE 12

// Define the number of octaves
#define OCTAVES 10

// Define the number of notes in total
#define TOTAL_NOTES (NOTES_PER_OCTAVE * OCTAVES)

noteinfo_t frequency_to_noteinfo(double frequency) {
    // 16.35 Hz is the lowest note, which is C0 (returned as octave 0, index 0)
    int index = round(log2(frequency / A4_FREQUENCY) * NOTES_PER_OCTAVE) + 57;

    if (index < 0) {
        index = 0;
    }
    if (index >= TOTAL_NOTES) {
        index = TOTAL_NOTES - 1;
    }

    int noteIndex = index % NOTES_PER_OCTAVE;
    int octave = index / NOTES_PER_OCTAVE;

    // Return the note
    return {noteIndex, octave};
}

// Function to convert musical note to frequency
float noteinfo_to_frequency(noteinfo_t noteinfo) {
    // C0 is the lowest note, which is index 0 (as opposed to MIDI's 12)
    int32_t index = noteinfo.octave * NOTES_PER_OCTAVE + noteinfo.index;
    if (index < 0) {
        index = 0;
    }

    return A4_FREQUENCY * pow(2, ((float)(index - 57)) / 12.0);
}

float modulate_frequency(float frequency, int16_t semitoneCount) {
    // Calculate the frequency ratio for semitone modulation
    double frequencyRatio = pow(2.0, (float)semitoneCount / 12.0);

    // Modulate the frequency
    double modulatedFrequency = frequency * frequencyRatio;

    return modulatedFrequency;
}

// Function to convert note to string
char* noteinfo_t::toStr() {
    static char buffer[4];
    sprintf(buffer, "%s%d", note_names[index], octave);
    return buffer;
}

#include <math.h>
#include <stdio.h>

#include "note.h"

const double A4_FREQUENCY = 440.0;
const double SEMITONE_RATIO = pow(2.0, 1.0 / 12.0);

const char* note_names[] = {"A-", "A#", "B-", "C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#"};

// Function to convert frequency to the nearest musical note, starting from A0 and ending with B9
#define A4_FREQUENCY 440.0

// Define the number of notes in an octave
#define NOTES_PER_OCTAVE 12

// Define the number of octaves
#define OCTAVES 10

// Define the number of notes in total
#define TOTAL_NOTES (NOTES_PER_OCTAVE * OCTAVES)

noteinfo_t frequency_to_noteinfo(double frequency) {
    double relative_pitch = log2(frequency / A4_FREQUENCY);

    // Calculate the note index and octave
    int noteIndex = (int)round(NOTES_PER_OCTAVE * relative_pitch) % NOTES_PER_OCTAVE;
    int octave = (int)floor(relative_pitch / NOTES_PER_OCTAVE) + 4; // A4 is in octave 4
    if (noteIndex < 0) {
        noteIndex += NOTES_PER_OCTAVE;
        octave--;
    }

    // Return the note
    return {noteIndex, octave};
}

// Function to convert musical note to frequency
float noteinfo_to_frequency(noteinfo_t noteinfo) {
    // Calculate the relative pitch compared to A4
    double relative_pitch = (noteinfo.index - 9.0) / NOTES_PER_OCTAVE + (noteinfo.octave - 4);

    // Calculate the frequency
    double frequency = A4_FREQUENCY * pow(2.0, relative_pitch);

    return frequency;
}

// Function to convert note to string
char* noteinfo_t::toStr() {
    static char buffer[4];
    sprintf(buffer, "%s%d", note_names[index], octave);
    return buffer;
}

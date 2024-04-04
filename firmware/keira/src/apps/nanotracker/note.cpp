#include <math.h>
#include <stdio.h>

#include "note.h"

const double A4_FREQUENCY = 440.0;
const double SEMITONE_RATIO = pow(2.0, 1.0 / 12.0);

const char* note_names[] = {"A-", "A#", "B-", "C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#"};

// Function to convert frequency to the nearest musical note
noteinfo_t frequency_to_noteinfo(double frequency) {
    // Calculate the number of semitones away from A4
    double semitones_from_a4 = 12.0 * log2(frequency / A4_FREQUENCY);

    // Calculate the note index and octave
    int noteIndex = (int)round(semitones_from_a4) % 12;
    int octave = (int)round((semitones_from_a4 - noteIndex) / 12.0) + 4;

    if (noteIndex < 0) {
        noteIndex += 12;
    }

    // Return the note
    return {noteIndex, octave};
}

// Function to convert musical note to frequency
float noteinfo_to_frequency(noteinfo_t noteinfo) {
    // Calculate the number of semitones away from A4
    double semitones_from_a4 = 12.0 * (noteinfo.octave - 4) + noteinfo.index;

    // Calculate the frequency
    return A4_FREQUENCY * pow(SEMITONE_RATIO, semitones_from_a4);
}

// Function to convert note to string
char* noteinfo_t::toStr() {
    static char buffer[4];
    sprintf(buffer, "%s%d", note_names[index], octave);
    return buffer;
}

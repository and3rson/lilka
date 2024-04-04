#include <math.h>
#include <stdio.h>

const double A4_FREQUENCY = 440.0;
const double SEMITONE_RATIO = pow(2.0, 1.0 / 12.0);

// Function to convert frequency to the nearest musical note
char* frequency_to_note(double frequency) {
    // Create a string to store the result
    static char result[4]; // Assuming maximum length of note name is 3 characters

    // Calculate the number of semitones away from A4
    if (frequency > 0) {
        double semitones_from_a4 = 12.0 * log2(frequency / A4_FREQUENCY);

        // Round to the nearest integer number of semitones
        int rounded_semitones = (int)round(semitones_from_a4);

        // Define the musical note names
        const char* note_names[] = {"A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"};

        // Calculate the octave
        int octave = 4 + rounded_semitones / 12;

        // Calculate the index of the note within the octave
        int note_index = (rounded_semitones % 12 + 12) % 12;

        sprintf(result, "%s%d", note_names[note_index], octave);
    } else {
        sprintf(result, "---");
    }

    return result;
}

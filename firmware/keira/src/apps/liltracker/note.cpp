#include <math.h>
#include <stdio.h>

#include "note.h"

const char* note_names[] = {"C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-"};

// Function to convert frequency to the nearest musical note, starting from A0 and ending with B9
#define A4_FREQUENCY 440.0f

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

    return A4_FREQUENCY * powf(2.0, ((float)(absIndex - 57)) / 12.0f);
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

float pow2table[12 * 10 * 2 + 1] = {
    0.001,   0.001,   0.001,   0.001,   0.001,   0.001,   0.001,   0.001,   0.002,   0.002,   0.002,   0.002,   0.002,
    0.002,   0.002,   0.002,   0.002,   0.003,   0.003,   0.003,   0.003,   0.003,   0.003,   0.004,   0.004,   0.004,
    0.004,   0.005,   0.005,   0.005,   0.006,   0.006,   0.006,   0.007,   0.007,   0.007,   0.008,   0.008,   0.009,
    0.009,   0.010,   0.010,   0.011,   0.012,   0.012,   0.013,   0.014,   0.015,   0.016,   0.017,   0.018,   0.019,
    0.020,   0.021,   0.022,   0.023,   0.025,   0.026,   0.028,   0.029,   0.031,   0.033,   0.035,   0.037,   0.039,
    0.042,   0.044,   0.047,   0.050,   0.053,   0.056,   0.059,   0.062,   0.066,   0.070,   0.074,   0.079,   0.083,
    0.088,   0.094,   0.099,   0.105,   0.111,   0.118,   0.125,   0.132,   0.140,   0.149,   0.157,   0.167,   0.177,
    0.187,   0.198,   0.210,   0.223,   0.236,   0.250,   0.265,   0.281,   0.297,   0.315,   0.334,   0.354,   0.375,
    0.397,   0.420,   0.445,   0.472,   0.500,   0.530,   0.561,   0.595,   0.630,   0.667,   0.707,   0.749,   0.794,
    0.841,   0.891,   0.944,   1.000,   1.059,   1.122,   1.189,   1.260,   1.335,   1.414,   1.498,   1.587,   1.682,
    1.782,   1.888,   2.000,   2.119,   2.245,   2.378,   2.520,   2.670,   2.828,   2.997,   3.175,   3.364,   3.564,
    3.775,   4.000,   4.238,   4.490,   4.757,   5.040,   5.339,   5.657,   5.993,   6.350,   6.727,   7.127,   7.551,
    8.000,   8.476,   8.980,   9.514,   10.079,  10.679,  11.314,  11.986,  12.699,  13.454,  14.254,  15.102,  16.000,
    16.951,  17.959,  19.027,  20.159,  21.357,  22.627,  23.973,  25.398,  26.909,  28.509,  30.204,  32.000,  33.903,
    35.919,  38.055,  40.317,  42.715,  45.255,  47.946,  50.797,  53.817,  57.018,  60.408,  64.000,  67.806,  71.838,
    76.109,  80.635,  85.430,  90.510,  95.892,  101.594, 107.635, 114.035, 120.816, 128.000, 135.611, 143.675, 152.219,
    161.270, 170.859, 181.019, 191.783, 203.187, 215.269, 228.070, 241.632, 256.000, 271.223, 287.350, 304.437, 322.540,
    341.719, 362.039, 383.567, 406.375, 430.539, 456.140, 483.264, 512.000, 542.445, 574.701, 608.874, 645.079, 683.438,
    724.077, 767.133, 812.750, 861.078, 912.280, 966.528, 1024.000
};

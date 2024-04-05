#pragma once

#include <stdint.h>

typedef struct noteinfo_t {
    int index; // [0;11]
    int octave;
    char* toStr();
    void add(int16_t semitoneCount);
    void fromFrequency(float frequency);
    float toFrequency();
} noteinfo_t;

float modulate_frequency(float frequency, float semitoneCount);

#define N_C0 \
    { 0, 0 }

#define N_C2 \
    { 0, 2 }
#define N_CS2 \
    { 1, 2 }
#define N_D2 \
    { 2, 2 }
#define N_DS2 \
    { 3, 2 }
#define N_E2 \
    { 4, 2 }
#define N_F2 \
    { 5, 2 }
#define N_FS2 \
    { 6, 2 }
#define N_G2 \
    { 7, 2 }
#define N_GS2 \
    { 8, 2 }
#define N_A2 \
    { 9, 2 }
#define N_AS2 \
    { 10, 2 }
#define N_B2 \
    { 11, 2 }

#define N_C3 \
    { 0, 3 }
#define N_CS3 \
    { 1, 3 }
#define N_D3 \
    { 2, 3 }
#define N_DS3 \
    { 3, 3 }
#define N_E3 \
    { 4, 3 }
#define N_F3 \
    { 5, 3 }
#define N_FS3 \
    { 6, 3 }
#define N_G3 \
    { 7, 3 }
#define N_GS3 \
    { 8, 3 }
#define N_A3 \
    { 9, 3 }
#define N_AS3 \
    { 10, 3 }
#define N_B3 \
    { 11, 3 }

#define N_C4 \
    { 0, 4 }
#define N_CS4 \
    { 1, 4 }
#define N_D4 \
    { 2, 4 }
#define N_DS4 \
    { 3, 4 }
#define N_E4 \
    { 4, 4 }
#define N_F4 \
    { 5, 4 }
#define N_FS4 \
    { 6, 4 }
#define N_G4 \
    { 7, 4 }
#define N_GS4 \
    { 8, 4 }
#define N_A4 \
    { 9, 4 }
#define N_AS4 \
    { 10, 4 }
#define N_B4 \
    { 11, 4 }

#define N_C5 \
    { 0, 5 }
#define N_CS5 \
    { 1, 5 }
#define N_D5 \
    { 2, 5 }
#define N_DS5 \
    { 3, 5 }
#define N_E5 \
    { 4, 5 }
#define N_F5 \
    { 5, 5 }
#define N_FS5 \
    { 6, 5 }
#define N_G5 \
    { 7, 5 }
#define N_GS5 \
    { 8, 5 }
#define N_A5 \
    { 9, 5 }
#define N_AS5 \
    { 10, 5 }
#define N_B5 \
    { 11, 5 }

#define N_C6 \
    { 0, 6 }
#define N_CS6 \
    { 1, 6 }
#define N_D6 \
    { 2, 6 }
#define N_DS6 \
    { 3, 6 }
#define N_E6 \
    { 4, 6 }
#define N_F6 \
    { 5, 6 }
#define N_FS6 \
    { 6, 6 }
#define N_G6 \
    { 7, 6 }
#define N_GS6 \
    { 8, 6 }
#define N_A6 \
    { 9, 6 }
#define N_AS6 \
    { 10, 6 }
#define N_B6 \
    { 11, 6 }

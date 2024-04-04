#pragma once

typedef struct noteinfo_t {
    int index; // [0;11]
    int octave;
    char* toStr();
} noteinfo_t;

noteinfo_t frequency_to_noteinfo(double frequency);
float noteinfo_to_frequency(noteinfo_t noteinfo);
char* note_to_string(noteinfo_t noteinfo, char buffer[4]);

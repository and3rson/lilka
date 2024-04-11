#pragma once

typedef enum {
    WAVEFORM_SILENCE,
    WAVEFORM_SQUARE,
    WAVEFORM_SAWTOOTH,
    WAVEFORM_TRIANGLE,
    WAVEFORM_SINE,
    WAVEFORM_NOISE,
    WAVEFORM_COUNT,
} waveform_t;

const waveform_t waveforms[WAVEFORM_COUNT] = {
    WAVEFORM_SILENCE,
    WAVEFORM_SQUARE,
    WAVEFORM_SAWTOOTH,
    WAVEFORM_TRIANGLE,
    WAVEFORM_SINE,
    WAVEFORM_NOISE,
};

const char* const waveform_names[WAVEFORM_COUNT] = {
    "OFF",
    "SQR",
    "SAW",
    "TRI",
    "SIN",
    "NOI",
};

typedef float (*waveform_fn_t)(float time, float frequency, float amplitude, float phase);

float silence(float time, float frequency, float amplitude, float phase);
float triangle(float time, float frequency, float amplitude, float phase);
// float sine(float time, float frequency, float amplitude, float phase);
float fast_sine(float time, float frequency, float amplitude, float phase);
float square(float time, float frequency, float amplitude, float phase);
float sawtooth(float time, float frequency, float amplitude, float phase);
float noise(float time, float frequency, float amplitude, float phase);

const waveform_fn_t waveform_functions[WAVEFORM_COUNT] = {
    silence,
    square,
    sawtooth,
    triangle,
    fast_sine,
    noise,
};

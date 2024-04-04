#pragma once

typedef enum {
    WAVEFORM_SQUARE,
    WAVEFORM_SAWTOOTH,
    WAVEFORM_TRIANGLE,
    WAVEFORM_SINE,
    WAVEFORM_NOISE,
    WAVEFORM_COUNT,
} waveform_t;

const waveform_t waveforms[WAVEFORM_COUNT] = {
    WAVEFORM_SQUARE,
    WAVEFORM_SAWTOOTH,
    WAVEFORM_TRIANGLE,
    WAVEFORM_SINE,
    WAVEFORM_NOISE,
};

typedef float (*waveform_fn_t)(float time, float frequency, float amplitude, float phase);

float triangle(float time, float frequency, float amplitude, float phase);
float sine(float time, float frequency, float amplitude, float phase);
float square(float time, float frequency, float amplitude, float phase);
float sawtooth(float time, float frequency, float amplitude, float phase);
float noise(float time, float frequency, float amplitude, float phase);

const waveform_fn_t waveform_functions[WAVEFORM_COUNT] = {
    square,
    sawtooth,
    triangle,
    sine,
    noise,
};

#pragma once

typedef enum {
    WAVEFORM_SQUARE,
    WAVEFORM_SAWTOOTH,
    WAVEFORM_TRIANGLE,
    WAVEFORM_SINE,
    WAVEFORM_NOISE,
} waveform_t;

typedef float (*waveform_fn_t)(float time, float frequency, float amplitude, float phase);

float triangle(float time, float frequency, float amplitude, float phase);
float sine(float time, float frequency, float amplitude, float phase);
float square(float time, float frequency, float amplitude, float phase);
float sawtooth(float time, float frequency, float amplitude, float phase);

#include <math.h>
#include "waveforms.h"

// TODO: Use integers instead of floats for audio samples, time, frequency, amplitude, and phase?

// Silence wave function
float silence(float time, float frequency, float amplitude, float phase) {
    return 0.0f;
}

// Triangle wave function
float triangle(float time, float frequency, float amplitude, float phase) {
    return 2.0f * amplitude * fabsf(2.0f * fmodf(time * frequency + phase, 1.0f) - 1.0f) - amplitude;
}

// Sine wave function
float sine(float time, float frequency, float amplitude, float phase) {
    return amplitude * sinf(2.0f * M_PI * frequency * time + phase);
}

// Square wave function
float square(float time, float frequency, float amplitude, float phase) {
    // return amplitude * (sinf(2.0f * M_PI * frequency * time + phase) > 0.0f ? 1.0f : -1.0f);
    return amplitude * (fmodf(time * frequency + phase, 1.0f) < 0.5f ? 1.0f : -1.0f);
}

// Sawtooth wave function
float sawtooth(float time, float frequency, float amplitude, float phase) {
    return 2.0f * amplitude * (fmodf(time * frequency + phase, 1.0f) - 0.5f);
}

// Noise wave function
float noise(float time, float frequency, float amplitude, float phase) {
    return 2.0f * amplitude * ((float)rand() / RAND_MAX - 0.5f);
}
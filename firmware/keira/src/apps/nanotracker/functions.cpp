#include <math.h>
#include "functions.h"

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
    return amplitude * (sinf(2.0f * M_PI * frequency * time + phase) > 0.0f ? 1.0f : -1.0f);
}

// Sawtooth wave function
float sawtooth(float time, float frequency, float amplitude, float phase) {
    return 2.0f * amplitude * (fmodf(time * frequency + phase, 1.0f) - 0.5f);
}

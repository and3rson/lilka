#pragma once

#include <stdint.h>

typedef enum {
    EFFECT_TYPE_NONE,
    EFFECT_TYPE_ARPEGGIO,
    EFFECT_TYPE_VIBRATO,
    EFFECT_TYPE_TREMOLO,
    EFFECT_TYPE_COUNT,
} effect_type_t;

const effect_type_t effects[EFFECT_TYPE_COUNT] = {
    EFFECT_TYPE_NONE,
    EFFECT_TYPE_ARPEGGIO,
    EFFECT_TYPE_VIBRATO,
    EFFECT_TYPE_TREMOLO,
};

const char effect_signs[EFFECT_TYPE_COUNT] = {
    '.',
    'A',
    'V',
    'T',
};

typedef struct {
    effect_type_t type;
    uint8_t param;
} effect_t;

typedef void (*effect_fn_t)(const float time, float* frequency, float* amplitude, float* phase, uint8_t param);

void effect_none(const float time, float* frequency, float* amplitude, float* phase, uint8_t param);
void effect_arpeggio(const float time, float* frequency, float* amplitude, float* phase, uint8_t param);
void effect_vibrato(const float time, float* frequency, float* amplitude, float* phase, uint8_t param);
void effect_tremolo(const float time, float* frequency, float* amplitude, float* phase, uint8_t param);

const effect_fn_t effect_functions[EFFECT_TYPE_COUNT] = {
    effect_none,
    effect_arpeggio,
    effect_vibrato,
    effect_tremolo,
};

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

typedef struct {
    effect_type_t effect;
    int8_t param1;
    int8_t param2;
    int8_t param3;
} effect_t;

typedef void (*effect_fn_t)(float* time, float* frequency, float* amplitude, float* phase, effect_t effect);

void effect_none(float* time, float* frequency, float* amplitude, float* phase, effect_t effect);
void effect_arpeggio(float* time, float* frequency, float* amplitude, float* phase, effect_t effect);
void effect_vibrato(float* time, float* frequency, float* amplitude, float* phase, effect_t effect);
void effect_tremolo(float* time, float* frequency, float* amplitude, float* phase, effect_t effect);

const effect_fn_t effect_functions[EFFECT_TYPE_COUNT] = {
    effect_none,
    effect_arpeggio,
    effect_vibrato,
    effect_tremolo,
};

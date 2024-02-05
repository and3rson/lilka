#ifndef LILKA_INPUT_H
#define LILKA_INPUT_H

#include "config.h"

extern bool _input_initialized;

typedef struct {
    int up;
    int down;
    int left;
    int right;
    int a;
    int b;
    int select;
    int start;
} lilka_input_t;

void lilka_input_begin();
lilka_input_t lilka_input_read();

#endif // LILKA_INPUT_H

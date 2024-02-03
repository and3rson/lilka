#include <Arduino.h>

#include "input.h"

bool _input_initialized = false;

void lilka_input_begin() {
    pinMode(LILKA_GPIO_UP, INPUT_PULLUP);
    pinMode(LILKA_GPIO_DOWN, INPUT_PULLUP);
    pinMode(LILKA_GPIO_LEFT, INPUT_PULLUP);
    pinMode(LILKA_GPIO_RIGHT, INPUT_PULLUP);
    pinMode(LILKA_GPIO_A, INPUT_PULLUP);
    pinMode(LILKA_GPIO_B, INPUT_PULLUP);
    pinMode(LILKA_GPIO_SELECT, INPUT_PULLUP);
    pinMode(LILKA_GPIO_START, INPUT_PULLUP);
    _input_initialized = true;
}

lilka_input_t lilka_input_read() {
    if (!_input_initialized) {
        lilka_input_begin();
    }
    return (lilka_input_t){
        .up = !digitalRead(LILKA_GPIO_UP),
        .down = !digitalRead(LILKA_GPIO_DOWN),
        .left = !digitalRead(LILKA_GPIO_LEFT),
        .right = !digitalRead(LILKA_GPIO_RIGHT),
        .a = !digitalRead(LILKA_GPIO_A),
        .b = !digitalRead(LILKA_GPIO_B),
        .select = !digitalRead(LILKA_GPIO_SELECT),
        .start = !digitalRead(LILKA_GPIO_START),
    };
}

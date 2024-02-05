#include <Arduino.h>

#include "controller.h"

namespace lilka {

state_t states[8];
int8_t pins[8] = {
    LILKA_GPIO_UP, LILKA_GPIO_DOWN, LILKA_GPIO_LEFT, LILKA_GPIO_RIGHT, LILKA_GPIO_A, LILKA_GPIO_B, LILKA_GPIO_SELECT, LILKA_GPIO_START,
};

Controller::Controller() {
    for (int i = 0; i < LILKA_STATE_COUNT; i++) {
        states[i] = (state_t){
            .pressed = false,
            .time = 0,
        };
    }
}

void IRAM_ATTR handle_interrupt(int stateIndex) {
    state_t *state = &states[stateIndex];
    if (millis() - state->time < LILKA_DEBOUNCE_TIME) {
        return;
    }
    state->pressed = !digitalRead(pins[stateIndex]);
    state->time = micros();
}

void IRAM_ATTR on_up() {
    handle_interrupt(LILKA_STATE_UP);
}

void IRAM_ATTR on_down() {
    handle_interrupt(LILKA_STATE_DOWN);
}

void IRAM_ATTR on_left() {
    handle_interrupt(LILKA_STATE_LEFT);
}

void IRAM_ATTR on_right() {
    handle_interrupt(LILKA_STATE_RIGHT);
}

void IRAM_ATTR on_a() {
    handle_interrupt(LILKA_STATE_A);
}

void IRAM_ATTR on_b() {
    handle_interrupt(LILKA_STATE_B);
}

void IRAM_ATTR on_select() {
    handle_interrupt(LILKA_STATE_SELECT);
}

void IRAM_ATTR on_start() {
    handle_interrupt(LILKA_STATE_START);
}

void Controller::begin() {
    // Array of interrupt handlers

    void (*handlers[])(void) = {
        on_up, on_down, on_left, on_right, on_a, on_b, on_select, on_start,
    };

    for (int i = 0; i < LILKA_STATE_COUNT; i++) {
        pinMode(pins[i], INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(pins[i]), handlers[i], CHANGE);
    }
}

input_t Controller::state() {
    return (input_t){
        .up = states[LILKA_STATE_UP].pressed,
        .down = states[LILKA_STATE_DOWN].pressed,
        .left = states[LILKA_STATE_LEFT].pressed,
        .right = states[LILKA_STATE_RIGHT].pressed,
        .a = states[LILKA_STATE_A].pressed,
        .b = states[LILKA_STATE_B].pressed,
        .select = states[LILKA_STATE_SELECT].pressed,
        .start = states[LILKA_STATE_START].pressed,
    };
}

Controller controller;

} // namespace lilka

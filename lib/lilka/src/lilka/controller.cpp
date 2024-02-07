#include <Arduino.h>

#include "serial.h"
#include "controller.h"

namespace lilka {

Controller *Controller::_instance = NULL;

Controller::Controller() {
    for (int i = 0; i < Button::COUNT; i++) {
        states[i] = (ButtonState){
            .pressed = false,
            .time = 0,
        };
        handlers[i] = NULL;
    }
    Controller::_instance = this;
}

void IRAM_ATTR Controller::handle_interrupt(int stateIndex) {
    ButtonState *state = &states[stateIndex];
    if (millis() - state->time < LILKA_DEBOUNCE_TIME) {
        return;
    }
    state->pressed = !digitalRead(pins[stateIndex]);
    if (handlers[stateIndex] != NULL) {
        handlers[stateIndex](state->pressed);
    }
    state->time = micros();
}

void IRAM_ATTR Controller::on_up() {
    _instance->handle_interrupt(Button::UP);
}

void IRAM_ATTR Controller::on_down() {
    _instance->handle_interrupt(Button::DOWN);
}

void IRAM_ATTR Controller::on_left() {
    _instance->handle_interrupt(Button::LEFT);
}

void IRAM_ATTR Controller::on_right() {
    _instance->handle_interrupt(Button::RIGHT);
}

void IRAM_ATTR Controller::on_a() {
    _instance->handle_interrupt(Button::A);
}

void IRAM_ATTR Controller::on_b() {
    _instance->handle_interrupt(Button::B);
}

void IRAM_ATTR Controller::on_select() {
    _instance->handle_interrupt(Button::SELECT);
}

void IRAM_ATTR Controller::on_start() {
    _instance->handle_interrupt(Button::START);
}

void Controller::begin() {
    serial_log("initializing controller");
    void (*handlers[])(void) = {
        on_up, on_down, on_left, on_right, on_a, on_b, on_select, on_start,
    };

    for (int i = 0; i < Button::COUNT; i++) {
        if (pins[i] < 0) {
            continue;
        }
        pinMode(pins[i], INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(pins[i]), handlers[i], CHANGE);
    }
    serial_log("controller ready");
}

State Controller::state() {
    return (State){
        .up = states[Button::UP].pressed,
        .down = states[Button::DOWN].pressed,
        .left = states[Button::LEFT].pressed,
        .right = states[Button::RIGHT].pressed,
        .a = states[Button::A].pressed,
        .b = states[Button::B].pressed,
        .select = states[Button::SELECT].pressed,
        .start = states[Button::START].pressed,
    };
}

void Controller::setHandler(Button button, void (*handler)(bool)) {
    handlers[button] = handler;
}

void Controller::clearHandlers() {
    for (int i = 0; i < Button::COUNT; i++) {
        handlers[i] = NULL;
    }
}

Controller controller;

} // namespace lilka

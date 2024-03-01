#include <Arduino.h>

#include <driver/uart.h>

#include "serial.h"
#include "controller.h"

namespace lilka {

Controller *Controller::_instance = NULL;

Controller::Controller() {
    for (int i = 0; i < Button::COUNT; i++) {
        state.buttons[i] = (ButtonState){
            .pressed = false,
            .justPressed = false,
            .justReleased = false,
            .time = 0,
        };
    }
    clearHandlers();
    Controller::_instance = this;
}

void IRAM_ATTR Controller::handle_interrupt(int stateIndex) {
    ButtonState *state = &Controller::_instance->state.buttons[stateIndex];
    if (millis() - state->time < LILKA_DEBOUNCE_TIME) {
        return;
    }
    state->pressed = !digitalRead(pins[stateIndex]);
    state->justPressed = state->pressed;
    state->justReleased = !state->pressed;
    if (handlers[stateIndex] != NULL) {
        handlers[stateIndex](state->pressed);
    }
    if (globalHandler != NULL) {
        globalHandler((Button)stateIndex, state->pressed);
    }
    if (state->pressed) {
        if (_seq[_seqIndex++] == stateIndex) {
            if (_seqIndex == 10) {
                _seqIndex = 0;
                // serial_log("do the barrel roll!");
            }
        } else {
            _seqIndex = 0;
        }
    }
    state->time = millis();
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

void IRAM_ATTR Controller::on_c() {
    _instance->handle_interrupt(Button::C);
}

void IRAM_ATTR Controller::on_d() {
    _instance->handle_interrupt(Button::D);
}

void IRAM_ATTR Controller::on_select() {
    _instance->handle_interrupt(Button::SELECT);
}

void IRAM_ATTR Controller::on_start() {
    _instance->handle_interrupt(Button::START);
}

void Controller::resetState() {
    for (int i = 0; i < Button::COUNT; i++) {
        ButtonState *button = &_instance->state.buttons[i];
        button->justPressed = false;
        button->justReleased = false;
    }
}

void Controller::begin() {
    serial_log("initializing controller");
    void (*handlers[])(void) = {
        on_up, on_down, on_left, on_right, on_a, on_b, on_c, on_d, on_select, on_start,
    };

#if LILKA_VERSION == 1
    // Detach UART from GPIO20 & GPIO21 to use them as normal IOs
    // https://esp32developer.com/programming-in-c-c/console/using-uart0-disable-logging-output
    esp_log_level_set("*", ESP_LOG_NONE); // DISABLE ESP32 LOGGING ON UART0
    if (uart_driver_delete(UART_NUM_0) != ESP_OK) {
        serial_err("failed to detach UART0");
    }
    gpio_reset_pin(GPIO_NUM_20);
    gpio_reset_pin(GPIO_NUM_21);
#endif

    for (int i = 0; i < Button::COUNT; i++) {
        if (pins[i] < 0) {
            continue;
        }
        pinMode(pins[i], INPUT_PULLUP);
        int intNum = digitalPinToInterrupt(pins[i]);
        if (intNum < 0) {
            serial_err("failed to get interrupt number");
            continue;
        }
        attachInterrupt(intNum, handlers[i], CHANGE);
    }
    serial_log("controller ready");
}

State Controller::getState() {
    State _current = state;
    resetState();
    return _current;
}

void Controller::setGlobalHandler(void (*handler)(Button, bool)) {
    globalHandler = handler;
}

void Controller::setHandler(Button button, void (*handler)(bool)) {
    handlers[button] = handler;
}

void Controller::clearHandlers() {
    for (int i = 0; i < Button::COUNT; i++) {
        handlers[i] = NULL;
    }
    globalHandler = NULL;
}

Controller controller;

} // namespace lilka

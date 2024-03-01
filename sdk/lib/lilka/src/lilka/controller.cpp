#include <Arduino.h>

#include <driver/uart.h>

#include "serial.h"
#include "controller.h"

namespace lilka {

SemaphoreHandle_t Controller::semaphore;

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
}

void Controller::inputTask(void *arg) {
    Controller *self = (Controller *)arg;
    while (1) {
        // Sleep for 10ms
        xSemaphoreTake(self->semaphore, portMAX_DELAY);
        for (int i = 0; i < Button::COUNT; i++) {
            ButtonState *state = &self->state.buttons[i];
            if (self->pins[i] < 0) {
                continue;
            }
            if (millis() - state->time < LILKA_DEBOUNCE_TIME) {
                continue;
            }
            state->pressed = !digitalRead(self->pins[i]);
            state->justPressed = state->pressed;
            state->justReleased = !state->pressed;
            if (self->handlers[i] != NULL) {
                self->handlers[i](state->pressed);
            }
            if (self->globalHandler != NULL) {
                self->globalHandler((Button)i, state->pressed);
            }
            state->time = millis();
        }
        xSemaphoreGive(self->semaphore);
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}

// void IRAM_ATTR Controller::handle_interrupt(int stateIndex) {
//     ButtonState *state = &Controller::_instance->state.buttons[stateIndex];
//     if (millis() - state->time < LILKA_DEBOUNCE_TIME) {
//         return;
//     }
//     state->pressed = !digitalRead(pins[stateIndex]);
//     state->justPressed = state->pressed;
//     state->justReleased = !state->pressed;
//     if (handlers[stateIndex] != NULL) {
//         handlers[stateIndex](state->pressed);
//     }
//     if (globalHandler != NULL) {
//         globalHandler((Button)stateIndex, state->pressed);
//     }
//     if (state->pressed) {
//         if (_seq[_seqIndex++] == stateIndex) {
//             if (_seqIndex == 10) {
//                 _seqIndex = 0;
//                 // serial_log("do the barrel roll!");
//             }
//         } else {
//             _seqIndex = 0;
//         }
//     }
//     state->time = millis();
// }

void Controller::resetState() {
    xSemaphoreTake(semaphore, portMAX_DELAY);
    _resetState();
    xSemaphoreGive(semaphore);
}

void Controller::begin() {
    serial_log("initializing controller");

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
    }

    // Create RTOS task for handling button presses
    semaphore = xSemaphoreCreateBinary();
    xTaskCreate(Controller::inputTask, "input", 2048, this, 1, NULL);

    serial_log("controller ready");
}

State Controller::getState() {
    xSemaphoreTake(semaphore, portMAX_DELAY);
    State _current = state;
    _resetState();
    return _current;
    xSemaphoreGive(semaphore);
}

void Controller::_resetState() {
    for (int i = 0; i < Button::COUNT; i++) {
        state.buttons[i].justPressed = false;
        state.buttons[i].justReleased = false;
    }
}

void Controller::setGlobalHandler(void (*handler)(Button, bool)) {
    xSemaphoreTake(semaphore, portMAX_DELAY);
    globalHandler = handler;
    xSemaphoreGive(semaphore);
}

void Controller::setHandler(Button button, void (*handler)(bool)) {
    xSemaphoreTake(semaphore, portMAX_DELAY);
    handlers[button] = handler;
    xSemaphoreGive(semaphore);
}

void Controller::clearHandlers() {
    xSemaphoreTake(semaphore, portMAX_DELAY);
    for (int i = 0; i < Button::COUNT; i++) {
        handlers[i] = NULL;
    }
    globalHandler = NULL;
    xSemaphoreGive(semaphore);
}

Controller controller;

} // namespace lilka

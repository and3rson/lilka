#include <Arduino.h>

#include <driver/uart.h>

#include "serial.h"
#include "controller.h"

namespace lilka {

SemaphoreHandle_t Controller::semaphore = NULL;

Controller::Controller() {
    for (int i = 0; i < Button::COUNT; i++) {
        _StateButtons *buttons = reinterpret_cast<_StateButtons *>(&this->state);

        (*buttons)[i] = (ButtonState){
            .pressed = false,
            .justPressed = false,
            .justReleased = false,
            .time = 0,
        };
    }
    _clearHandlers();
}

// example function that uses xSemaphoreTake and xSemaphoreGive

void Controller::inputTask(void *arg) {
    Controller *self = (Controller *)arg;
    while (1) {
        xSemaphoreTake(self->semaphore, portMAX_DELAY);
        for (int i = 0; i < Button::COUNT; i++) {
            if (i == Button::ANY) {
                // Skip "any" key since its state is computed from other keys
                continue;
            }
            _StateButtons *buttons = reinterpret_cast<_StateButtons *>(&self->state);
            ButtonState *state = buttons[i];
            if (self->pins[i] < 0) {
                continue;
            }
            if (millis() - state->time < LILKA_DEBOUNCE_TIME) {
                continue;
            }
            bool pressed = !digitalRead(self->pins[i]);
            if (pressed != state->pressed) {
                state->pressed = pressed;
                state->justPressed = pressed;
                state->justReleased = !pressed;
                self->state.any.pressed = pressed;
                self->state.any.justPressed = self->state.any.justPressed || pressed;
                self->state.any.justReleased = self->state.any.justReleased || !pressed;
                if (self->handlers[i] != NULL) {
                    self->handlers[i](pressed);
                }
                if (self->globalHandler != NULL) {
                    self->globalHandler((Button)i, pressed);
                }
                state->time = millis();
            }
        }
        // Handle "any" key

        xSemaphoreGive(self->semaphore);
        // Sleep for 5ms
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}

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
    xSemaphoreGive(semaphore);
    xTaskCreate(Controller::inputTask, "input", 2048, this, 1, NULL);

    serial_log("controller ready");
}

State Controller::getState() {
    xSemaphoreTake(semaphore, portMAX_DELAY);
    State _current = state;
    _resetState();
    xSemaphoreGive(semaphore);
    return _current;
}

void Controller::_resetState() {
    for (int i = 0; i < Button::COUNT; i++) {
        _StateButtons *buttons = reinterpret_cast<_StateButtons *>(&this->state);
        ButtonState *state = buttons[i];
        state->justPressed = false;
        state->justReleased = false;
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
    _clearHandlers();
    xSemaphoreGive(semaphore);
}

void Controller::_clearHandlers() {
    // xSemaphoreTake(semaphore, portMAX_DELAY);
    for (int i = 0; i < Button::COUNT; i++) {
        handlers[i] = NULL;
    }
    globalHandler = NULL;
    // xSemaphoreGive(semaphore);
}

Controller controller;

} // namespace lilka

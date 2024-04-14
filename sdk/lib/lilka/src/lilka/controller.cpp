#include <Arduino.h>

#include <driver/uart.h>

#include "serial.h"
#include "controller.h"

namespace lilka {

class AcquireController {
public:
    explicit AcquireController(SemaphoreHandle_t semaphore) {
        this->semaphore = semaphore;
        xSemaphoreTakeRecursive(semaphore, portMAX_DELAY);
    }
    ~AcquireController() {
        xSemaphoreGiveRecursive(semaphore);
    }

private:
    SemaphoreHandle_t semaphore;
};

Controller::Controller() : state{}, semaphore(xSemaphoreCreateRecursiveMutex()) {
    for (int i = 0; i < Button::COUNT; i++) {
        _StateButtons& buttons = *reinterpret_cast<_StateButtons*>(&state);

        buttons[i] = (ButtonState){
            .pressed = false,
            .justPressed = false,
            .justReleased = false,
            .time = 0,
            .nextRepeatTime = 0,
            .repeatRate = 0,
            .repeatDelay = 0,
        };
    }
    xSemaphoreGive(semaphore);
    clearHandlers();
}

void Controller::inputTask() {
    while (1) {
        {
            AcquireController acquire(semaphore);
            for (int i = 0; i < Button::COUNT; i++) {
                if (i == Button::ANY) {
                    // Skip "any" key since its state is computed from other keys
                    continue;
                }
                _StateButtons& buttons = *reinterpret_cast<_StateButtons*>(&state);
                ButtonState* buttonState = &buttons[i];
                if (pins[i] < 0) {
                    continue;
                }
                if (millis() - buttonState->time < LILKA_DEBOUNCE_TIME) {
                    continue;
                }

                // Is the button being held down?
                bool pressed = !digitalRead(pins[i]);
                // Should the button repeat right now?
                bool shouldRepeat = buttonState->nextRepeatTime && millis() >= buttonState->nextRepeatTime;

                // Make/break
                if (pressed != buttonState->pressed || shouldRepeat) {
                    buttonState->pressed = pressed;
                    buttonState->justPressed = pressed;
                    buttonState->justReleased = !pressed;
                    state.any.pressed = pressed;
                    state.any.justPressed = state.any.justPressed || pressed;
                    state.any.justReleased = state.any.justReleased || !pressed;
                    if (handlers[i] != NULL) {
                        handlers[i](pressed);
                    }
                    if (globalHandler != NULL) {
                        globalHandler((Button)i, pressed);
                    }
                    buttonState->time = millis();
                }

                // Calculate repeats
                if (pressed) {
                    // Button is being held down, check if we need to repeat
                    if (buttonState->repeatRate && buttonState->repeatDelay) {
                        // Repeat is enabled, set next repeat time
                        if (buttonState->nextRepeatTime == 0) {
                            // This is the first repeat, delay by repeatDelay
                            buttonState->nextRepeatTime = millis() + buttonState->repeatDelay;
                        } else if (millis() >= buttonState->nextRepeatTime) {
                            // Delay subsequent repeats by 1/repeatRate seconds
                            buttonState->nextRepeatTime += 1000 / buttonState->repeatRate;
                        }
                    }
                } else {
                    // Button is not being held down, reset repeat
                    buttonState->nextRepeatTime = 0;
                }
            }
        }

        // Sleep for 5ms
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}

void Controller::resetState() {
    AcquireController acquire(semaphore);
    for (int i = 0; i < Button::COUNT; i++) {
        _StateButtons& buttons = *reinterpret_cast<_StateButtons*>(&state);
        ButtonState* buttonState = &buttons[i];
        buttonState->justPressed = false;
        buttonState->justReleased = false;
    }
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
    xTaskCreate([](void* arg) { static_cast<Controller*>(arg)->inputTask(); }, "input", 2048, this, 1, NULL);

    serial_log("controller ready");
}

State Controller::getState() {
    AcquireController acquire(semaphore);
    State _current = state;
    resetState();
    return _current;
}

State Controller::peekState() {
    AcquireController acquire(semaphore);
    return state;
}

void Controller::setGlobalHandler(void (*handler)(Button, bool)) {
    AcquireController acquire(semaphore);
    globalHandler = handler;
}

void Controller::setHandler(Button button, void (*handler)(bool)) {
    AcquireController acquire(semaphore);
    handlers[button] = handler;
}

void Controller::clearHandlers() {
    AcquireController acquire(semaphore);
    for (int i = 0; i < Button::COUNT; i++) {
        handlers[i] = NULL;
    }
    globalHandler = NULL;
}

void Controller::setAutoRepeat(Button button, uint32_t rate, uint32_t delay) {
    AcquireController acquire(semaphore);
    _StateButtons& buttons = *reinterpret_cast<_StateButtons*>(&state);
    ButtonState* buttonState = &buttons[button];
    buttonState->repeatRate = rate;
    buttonState->repeatDelay = delay;
}

Controller controller;

} // namespace lilka

#include <Arduino.h>

#include "controller.h"

namespace lilka {

Controller::Controller() {
}

void Controller::begin() {
    pinMode(LILKA_GPIO_UP, INPUT_PULLUP);
    pinMode(LILKA_GPIO_DOWN, INPUT_PULLUP);
    pinMode(LILKA_GPIO_LEFT, INPUT_PULLUP);
    pinMode(LILKA_GPIO_RIGHT, INPUT_PULLUP);
    pinMode(LILKA_GPIO_A, INPUT_PULLUP);
    pinMode(LILKA_GPIO_B, INPUT_PULLUP);
    pinMode(LILKA_GPIO_SELECT, INPUT_PULLUP);
    pinMode(LILKA_GPIO_START, INPUT_PULLUP);
}

input_t Controller::read() {
    return (input_t){
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

Controller controller;

} // namespace lilka

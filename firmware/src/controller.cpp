#include <Arduino.h>

#include "hw_config.h"

/* controller is GPIO */

extern "C" void controller_init() {
    pinMode(HW_CONTROLLER_GPIO_UP, INPUT_PULLUP);
    pinMode(HW_CONTROLLER_GPIO_DOWN, INPUT_PULLUP);
    pinMode(HW_CONTROLLER_GPIO_LEFT, INPUT_PULLUP);
    pinMode(HW_CONTROLLER_GPIO_RIGHT, INPUT_PULLUP);
    pinMode(HW_CONTROLLER_GPIO_SELECT, INPUT_PULLUP);
    pinMode(HW_CONTROLLER_GPIO_START, INPUT_PULLUP);
    pinMode(HW_CONTROLLER_GPIO_A, INPUT_PULLUP);
    pinMode(HW_CONTROLLER_GPIO_B, INPUT_PULLUP);
}

extern "C" uint32_t controller_read_input() {
    uint32_t u, d, l, r, s, t, a, b, x, y;

    u = digitalRead(HW_CONTROLLER_GPIO_UP);
    d = digitalRead(HW_CONTROLLER_GPIO_DOWN);
    l = digitalRead(HW_CONTROLLER_GPIO_LEFT);
    r = digitalRead(HW_CONTROLLER_GPIO_RIGHT);

    s = digitalRead(HW_CONTROLLER_GPIO_SELECT);
    t = digitalRead(HW_CONTROLLER_GPIO_START);
    a = digitalRead(HW_CONTROLLER_GPIO_A);
    b = digitalRead(HW_CONTROLLER_GPIO_B);

    return 0xFFFFFFFF ^ ((!u << 0) | (!d << 1) | (!l << 2) | (!r << 3) | (!s << 4) | (!t << 5) | (!a << 6) | (!b << 7) | (!x << 8) | (!y << 9));
}

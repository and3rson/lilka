#include <Arduino.h>

#include "hw_config.h"

#include <lilka/controller.h>

/* controller is GPIO */

extern "C" void controller_init() {}

extern "C" uint32_t controller_read_input() {
    lilka::input_t input = lilka::controller.read();
    return 0xFFFFFFFF ^ ((input.up << 0) | (input.down << 1) | (input.left << 2) | (input.right << 3) | (input.select << 4) | (input.start << 5) | (input.a << 6) | (input.b << 7));
}

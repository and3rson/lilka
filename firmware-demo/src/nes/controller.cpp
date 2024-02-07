#include <Arduino.h>

#include "hw_config.h"

#include <lilka/controller.h>

#include <nes/nes.h>

/* controller is GPIO */

extern "C" void controller_init() {}

extern "C" uint32_t controller_read_input() {
    lilka::State state = lilka::controller.state();
    return 0xFFFFFFFF ^ ((state.up << 0) | (state.down << 1) | (state.left << 2) | (state.right << 3) | (state.select << 4) | (state.start << 5) | (state.a << 6) | (state.b << 7));
}

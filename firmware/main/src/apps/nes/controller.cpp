#include <Arduino.h>

#include "hw_config.h"

#include <lilka/controller.h>

#include <nes/nes.h>

/* controller is GPIO */

extern "C" void controller_init() {}

extern "C" uint32_t controller_read_input() {
    lilka::State state = lilka::controller.getState();
    return 0xFFFFFFFF ^ ((state.up.pressed << 0) | (state.down.pressed << 1) | (state.left.pressed << 2) | (state.right.pressed << 3) | (state.select.pressed << 4) | (state.start.pressed << 5) | (state.a.pressed << 6) | (state.b.pressed << 7));
}

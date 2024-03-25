#include <initializer_list>
#include <Arduino.h>

#include "ext.h"
#include "config.h"

namespace lilka {

void ext_begin() {
    // Iterate over the pins, set them as inputs
    for (int pin : LILKA_EXT_PINS) {
        pinMode(pin, INPUT);
    }
}

} // namespace lilka

#include <Arduino.h>

#include "board.h"
#include "display.h"
#include "config.h"

namespace lilka {

Board::Board() {
}

void Board::begin() {
    // Iterate over the pins, set them as inputs
#if LILKA_VERSION >= 2
    for (int pin : LILKA_EXT_PINS) {
        pinMode(pin, INPUT);
    }
    pinMode(LILKA_SLEEP, OUTPUT);
    digitalWrite(LILKA_SLEEP, HIGH);
#endif
}

void Board::enablePowerSavingMode() {
#if LILKA_VERSION >= 2
    digitalWrite(LILKA_SLEEP, LOW);
#endif
    display.displayOff();
}

void Board::disablePowerSavingMode() {
#if LILKA_VERSION >= 2
    digitalWrite(LILKA_SLEEP, HIGH);
#endif
    display.displayOn();
}

uint8_t Board::getExtPinGPIO(uint8_t index) {
#if LILKA_VERSION == 1
    return ExtPin::INVALID;
#elif LILKA_VERSION == 2
    // TODO: hardcoded values
    if (index == 0 || index == 11) {
        return ExtPin::GND;
    } else if (index == 1 || index == 10) {
        return ExtPin::VCC;
    } else if (index <= 7) {
        return LILKA_EXT_PINS[index - 2];
    } else if (index == 8) {
        return TX;
    } else if (index == 9) {
        return RX;
    } else {
        return ExtPin::INVALID;
    }
#else
#    error "getExtPin is not defined for this board version"
#endif
}

Board board;

} // namespace lilka

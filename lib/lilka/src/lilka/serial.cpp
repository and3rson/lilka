#include <Arduino.h>

#include "serial.h"

namespace lilka {

void serial_begin() {
    Serial.begin(115200);
    Serial.setTxTimeoutMs(0);
}

} // namespace lilka

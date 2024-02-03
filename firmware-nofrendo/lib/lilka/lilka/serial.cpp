#include <Arduino.h>

#include "serial.h"

void lilka_serial_begin() {
    Serial.begin(115200);
    Serial.setTxTimeoutMs(0);
}

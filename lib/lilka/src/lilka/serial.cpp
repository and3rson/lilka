#include <Arduino.h>

#include "serial.h"

namespace lilka {

void serial_begin() {
    Serial.begin(115200);
    Serial.setTxTimeoutMs(0);
}

void serial_log(const char *format, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Serial.print("lilka: ");
    Serial.println(buffer);
}

} // namespace lilka

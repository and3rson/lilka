#include <Arduino.h>

#include "serial.h"

namespace lilka {

void serial_begin() {
    Serial.begin(115200);
    Serial.setTxTimeoutMs(0);
}

void serial_log(const char* format, ...) {
    // TODO: printf cannot be called from an ISR
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    // Serial.print("lilka: ");
    // Serial.println(buffer);
    printf("lilka: %s\n", buffer);
}
void serial_err(const char* format, ...) {
    // TODO: printf cannot be called from an ISR
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    // Serial.print("lilka: error: ");
    // Serial.println(buffer);
    printf("lilka: error: %s\n", buffer);
}

} // namespace lilka

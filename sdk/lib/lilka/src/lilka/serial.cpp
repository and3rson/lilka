#include <Arduino.h>

#include "serial.h"

namespace lilka {

void serial_begin() {
    Serial.begin(115200);
    Serial.setTxTimeoutMs(0);
}

void serial_log(const char* format, ...) {
    // TODO: printf cannot be called from an ISR
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    printf("[lilka] INFO: %s\n", buffer);
}
void serial_err(const char* format, ...) {
    // TODO: printf cannot be called from an ISR
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    printf("[lilka] ERROR: %s\n", buffer);
}

} // namespace lilka

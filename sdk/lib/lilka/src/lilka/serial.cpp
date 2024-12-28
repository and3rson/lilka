#include <Arduino.h>

#include "serial.h"

#define ANSI_COLOR(x) "\033[1;" #x "m"
#define ANSI_RESET    "\033[0m"

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
    printf("[lilka] " ANSI_COLOR(32) "INFO:" ANSI_RESET " %s\n", buffer);
}
void serial_err(const char* format, ...) {
    // TODO: printf cannot be called from an ISR
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    printf("[lilka] " ANSI_COLOR(31) "ERROR:" ANSI_RESET " %s\n", buffer);
}

} // namespace lilka

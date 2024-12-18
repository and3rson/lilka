#pragma once
#include <Arduino.h>
#include <queue>
#include "fileutils.h"

// Serial configuration:
#define SERIAL_BAUD_RATE  115200
#define SERIAL_TX_TIMEOUT 0

#define STRX(x)           #x
#define STR(x)            STRX(x)

#define ANSI_COLOR(x)     "\033[1;" #x "m"
#define ANSI_RESET        "\033[0m"
#define LILKA_LOG_FORMAT  "[ Lilka V" STR(LILKA_VERSION) " ][ " ANSI_COLOR(32) "INFO " ANSI_RESET " ]-> "
#define LILKA_ERR_FORMAT  "[ Lilka V" STR(LILKA_VERSION) " ][ " ANSI_COLOR(31) "ERROR" ANSI_RESET " ]-> "
#define LILKA_IDF_FORMAT  "[ Lilka V" STR(LILKA_VERSION) " ][ " ANSI_COLOR(31) " IDF " ANSI_RESET " ]-> "

#define TX_BUFFER_SIZE    256
#define STDIN_FD          0
#define STDOUT_FD         1
#define STDERR_FD         2
#define STDIO_PATH        "/dev/stdio"

#define STDIN_PATH        "/dev/stdio/in"
#define STDIN_LOCAL_PATH  "/in"
#define STDOUT_PATH       "/dev/stdio/out"
#define STDOUT_LOCAL_PATH "/out"
#define STDERR_PATH       "/dev/stdio/err"
#define STDERR_LOCAL_PATH "/err"

namespace lilka {

class SerialInterface {
public:
    SerialInterface();
    void begin(unsigned long baud = SERIAL_BAUD_RATE);
    void log(const char* format, ...);
    void err(const char* format, ...);
    void idf(const char* format, ...);

private:
    SemaphoreHandle_t serialMutex = xSemaphoreCreateMutex();
    std::queue<String> serialQueue;
    void run();
    void lock();
    void unlock();
};

void serial_log(const char* message, ...);
void serial_err(const char* message, ...);

extern SerialInterface serial;
} // namespace lilka

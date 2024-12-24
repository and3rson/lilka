#pragma once
#include <Arduino.h>
#include <queue>
#include "fileutils.h"

#include <esp32/clk.h>

// Serial configuration:
#define SERIAL_BAUD_RATE  115200
#define SERIAL_TIMEOUT    0

#define STRX(x)           #x
#define STR(x)            STRX(x)

#define ANSI_COLOR(x)     "\033[1;" #x "m"
#define ANSI_RESET        "\033[0m"
#define LILKA_LOG_FORMAT  "[ Lilka V" STR(LILKA_VERSION) " ][ " ANSI_COLOR(32) "INFO " ANSI_RESET " ]-> "
#define LILKA_ERR_FORMAT  "[ Lilka V" STR(LILKA_VERSION) " ][ " ANSI_COLOR(31) "ERROR" ANSI_RESET " ]-> "
#define LILKA_IDF_FORMAT  "[ Lilka V" STR(LILKA_VERSION) " ][ " ANSI_COLOR(34) "IDF  " ANSI_RESET " ]-> "

#define TX_BUFFER_SIZE    256
#define STDIN_FD          0
#define STDOUT_FD         1
#define STDERR_FD         2

#define STDIO_PATH        "/dev/stdio"
#define STDIN_LOCAL_PATH  "/in"
#define STDOUT_LOCAL_PATH "/out"
#define STDERR_LOCAL_PATH "/err"

#define STDIN_PATH        STDIO_PATH STDIN_LOCAL_PATH
#define STDOUT_PATH       STDIO_PATH STDOUT_LOCAL_PATH
#define STDERR_PATH       STDIO_PATH STDERR_LOCAL_PATH

namespace lilka {

class SerialInterface {
public:
    SerialInterface();
    void begin(unsigned long baud = SERIAL_BAUD_RATE);
    void log(const char* format, ...);
    void err(const char* format, ...);
    void idf(const char* format, ...);

private:
    // Doing all work in a separate task
    void run();
    // Some fun
    void writeGreetingMessage();

    // Mutex lock
    void lock();
    void unlock();

    // STDIO VFS :
    static void register_stdio_vfs();

    static int stdio_vfs_open(const char* path, int flags, int mode);
    static int stdio_vfs_close(int fd);
    static ssize_t stdio_vfs_read(int fd, void* dst, size_t size);
    static ssize_t stdio_vfs_write(int fd, const void* data, size_t size);

    // Storage and mutex for this storage
    SemaphoreHandle_t serialMutex = xSemaphoreCreateMutex();
    std::queue<String> serialQueue;
};

// TODO: replace serial_log/serial_err everywhere on serial.log and serial.err
// clang-format off
[[deprecated("Use serial.log instead")]]
// clang-format on
void serial_log(const char* message, ...);
// clang-format off
[[deprecated("Use serial.err instead")]]
// clang-format on
void serial_err(const char* message, ...);

extern SerialInterface serial;
} // namespace lilka

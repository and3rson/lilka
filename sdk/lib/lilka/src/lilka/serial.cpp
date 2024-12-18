#include "serial.h"
#include <esp_vfs.h>

namespace lilka {

// helpers:

void serial_log(const char* format, ...) {
    // // TODO: printf cannot be called from an ISR
    char buffer[TX_BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    // ,
    serial.log(buffer);
}
void serial_err(const char* format, ...) {
    // TODO: printf cannot be called from an ISR
    char buffer[TX_BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    serial.err(buffer);
}
int serial_idf(const char* format, va_list args) {
    char buffer[TX_BUFFER_SIZE];

    int cnt = vsnprintf(buffer, sizeof(buffer), format, args);
    serial.idf(buffer);
    return cnt;
}

int stdio_vfs_open(const char* path, int flags, int mode) {
    serial.log("Trying to open file %s", path);
    if (strcmp(path, STDIN_LOCAL_PATH) == 0) {
        return STDIN_FD;
    } else if (strcmp(path, STDOUT_LOCAL_PATH) == 0) {
        return STDOUT_FD;
    } else if (strcmp(path, STDERR_LOCAL_PATH) == 0) {
        return STDERR_FD;
    }
    // this file can't be opened via our vfs
    return -1;
}
int stdio_vfs_close(int fd) {
    // Just pretend we close something if
    // file descriptor is valid
    if (fd >= STDIN_FD && fd <= STDERR_FD) return 0;
    else return -1;
}

ssize_t stdio_vfs_read(int fd, void* dst, size_t size) {
    // read nothing, return nothing
    return 0;
}

ssize_t stdio_vfs_write(int fd, const void* data, size_t size) {
    // Write to uart
    String str = String(static_cast<const char*>(data), size);
    switch (fd) {
        case STDIN_FD:
            // Lol, who writes to stdin
            break;
        case STDOUT_FD:
            serial.log(str.c_str());
            break;
        case STDERR_FD:
            serial.err(str.c_str());
    }
    return size;
}

// Register stdio VFS operations
static void register_stdio_vfs() {
    esp_vfs_t stdio_vfs = {
        .flags = ESP_VFS_FLAG_DEFAULT,
        .write = stdio_vfs_write,
        .read = stdio_vfs_read,
        .open = stdio_vfs_open,
        .close = stdio_vfs_close,
    };

    // Register the stdio VFS for stdin, stdout, and stderr
    if (esp_vfs_register(STDIO_PATH, &stdio_vfs, NULL) == ESP_OK) {
        serial.log("STDIO VFS Register OK");
    } else serial.err("STDIO VFS Register FAILED");
}

SerialInterface::SerialInterface() {
}
void SerialInterface::lock() {
    xSemaphoreTake(serialMutex, portMAX_DELAY);
}
void SerialInterface::unlock() {
    xSemaphoreGive(serialMutex);
}

void SerialInterface::begin(unsigned long baud) {
    Serial.begin(baud);
    Serial.setTimeout(SERIAL_TX_TIMEOUT);
    // redirect esp logs ->
    esp_log_set_vprintf(&serial_idf);

    // redirect stdio
    register_stdio_vfs();
    auto new_stdin = fopen(STDIN_PATH, "r");
    auto new_stdout = fopen(STDOUT_PATH, "w");
    auto new_stderr = fopen(STDERR_PATH, "w");
    if (new_stdin == 0 || new_stdout == 0 || new_stderr == 0) {
        serial.err("Something wrong with STDIO VFS. Can't open file");
    } else {
        _GLOBAL_REENT->_stdin = new_stdin;
        _GLOBAL_REENT->_stdout = new_stdout;
        _GLOBAL_REENT->_stderr = new_stderr;
        stdin = new_stdin;
        stdout = new_stdout;
        stderr = new_stderr;

        setvbuf(stdout, NULL, _IONBF, 0);
        setvbuf(stdin, NULL, _IONBF, 0);
        setvbuf(stderr, NULL, _IONBF, 0);
    }
    // _GLOBAL_REENT->stdout =
    xTaskCreate([](void* arg) { static_cast<SerialInterface*>(arg)->run(); }, "SerialInterface", 2048, this, 1, NULL);
    serial.log("STDIN FD = %d", fileno(_GLOBAL_REENT->_stdin));
    serial.log("STDOUT FD = %d", fileno(_GLOBAL_REENT->_stdout));
    serial.log("STDERR FD = %d", fileno(_GLOBAL_REENT->_stderr));
}

void SerialInterface::log(const char* format, ...) {
    char buffer[TX_BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    serial.lock();
    char millisCstr[11]; // 10 digits + 1 for null terminator
    snprintf(millisCstr, sizeof(millisCstr), "%010lu", millis());
    serialQueue.push(String("[ ") + millisCstr + " ]" + String(LILKA_LOG_FORMAT) + buffer);
    serial.unlock();
}

void SerialInterface::err(const char* format, ...) {
    char buffer[TX_BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    serial.lock();
    char millisCstr[11]; // 10 digits + 1 for null terminator
    snprintf(millisCstr, sizeof(millisCstr), "%010lu", millis());
    serialQueue.push(String("[ ") + millisCstr + " ]" + String(LILKA_ERR_FORMAT) + buffer);
    serial.unlock();
}
void SerialInterface::idf(const char* format, ...) {
    char buffer[TX_BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    serial.lock();
    char millisCstr[11]; // 10 digits + 1 for null terminator
    snprintf(millisCstr, sizeof(millisCstr), "%010lu", millis());
    serialQueue.push(String("[ ") + millisCstr + " ]" + String(LILKA_IDF_FORMAT) + buffer);
    serial.unlock();
}

void SerialInterface::run() {
    size_t CHUNK_SIZE = TX_BUFFER_SIZE - 32;
    while (1) {
        if (millis() < 1000) {
            // sleep a while, to not miss something
            vTaskDelay(5 / portTICK_PERIOD_MS);
            continue;
        }
        Serial.flush(); // Be sure we've a clean TX buffer
        lock();
        while (!serialQueue.empty()) {
            // lock
            String outputLine = serialQueue.front();
            size_t len = outputLine.length();
            size_t sent = 0;
            Serial.flush();
            while (sent < len) {
                size_t toSend = (len - sent) > CHUNK_SIZE ? CHUNK_SIZE : (len - sent);
                Serial.write(outputLine.c_str() + sent, toSend);
                Serial.flush();

                sent += toSend;
            }
            if (outputLine[len - 1] != '\n') Serial.write("\n", 1);
            Serial.flush();
            serialQueue.pop();
        }
        unlock();
        taskYIELD();
    }
}

SerialInterface serial;
} // namespace lilka
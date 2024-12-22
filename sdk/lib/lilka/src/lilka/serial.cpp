#include "serial.h"
#include <esp_vfs.h>

namespace lilka {

// helpers:

void serial_log(const char* format, ...) {
    char buffer[TX_BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    serial.log(buffer);
}

void serial_err(const char* format, ...) {
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
    Serial.setTimeout(SERIAL_TIMEOUT);
#ifndef NO_GREETING_MESSAGE
    writeGreetingMessage();
#endif
    // redirect esp logs ->
    esp_log_set_vprintf(serial_idf);

    // redirect stdio
    register_stdio_vfs();

    // Create task writing to serial
    xTaskCreate([](void* arg) { static_cast<SerialInterface*>(arg)->run(); }, "SerialInterface", 2048, this, 1, NULL);
    // serial.log("STDIN FD = %d", fileno(_GLOBAL_REENT->_stdin));
    // serial.log("STDOUT FD = %d", fileno(_GLOBAL_REENT->_stdout));
    // serial.log("STDERR FD = %d", fileno(_GLOBAL_REENT->_stderr));

    //Test printf and scanf
    // printf("Testing printf and scanf:\n");
    // printf("Enter an integer: ");
    // int num;
    // scanf("%d", &num);
    // printf("You entered: %d\n", num);
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

void SerialInterface::writeGreetingMessage() {
    log("██      ██ ██      ██   ██  █████");
    log("██      ██ ██      ██  ██  ██   ██");
    log("██      ██ ██      █████   ███████");
    log("██      ██ ██      ██  ██  ██   ██");
    log("███████ ██ ███████ ██   ██ ██   ██");
    log("Starting boot...");
    // Gather some data about where we run
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    log("Chip Model: %d, Cores: %d\n", chip_info.model, chip_info.cores);
    log("Features: WiFi%s%s\n",
        (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
        (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");
    log("Chip Revision: %d\n", chip_info.revision);

    // Memory Information
    log("Total Free Heap: %d bytes\n", esp_get_free_heap_size());
    log("Minimum Free Heap Ever: %d bytes\n", esp_get_minimum_free_heap_size());
    log("DMA Capable Free Heap: %d bytes\n", heap_caps_get_free_size(MALLOC_CAP_DMA));

    // Flash Size
    uint32_t flash_size;
    esp_flash_get_size(NULL, &flash_size);
    log("Flash Size: %d bytes\n", flash_size);

    // CPU Frequency
    log("CPU Frequency: %d Hz\n", esp_clk_cpu_freq());

    // SDK Version
    log("ESP-IDF SDK version: %s\n", esp_get_idf_version());

    // Arduino Version
    log("Arduino framework version: %d.%d.%d(%d)",
        ESP_ARDUINO_VERSION_MAJOR,
        ESP_ARDUINO_VERSION_MINOR,
        ESP_ARDUINO_VERSION_PATCH,
        ESP_ARDUINO_VERSION);

    // Runtime Information
    log("Uptime (microseconds): %lld\n", esp_timer_get_time());

    // Reset Reason
    log("Last Reset Reason: %d\n", esp_reset_reason());
}

void SerialInterface::run() {
    while (1) {
        if (millis() < 1000) {
            // sleep a while, to not miss something
            vTaskDelay(5 / portTICK_PERIOD_MS);
            continue;
        }

        lock();
        while (!serialQueue.empty()) {
            // lock
            String outputLine = serialQueue.front();
            size_t len = outputLine.length();
            size_t sent = 0;
            size_t availableSpace = Serial.availableForWrite();
            while (sent < len) {
                size_t toSend = (len - sent) > availableSpace ? availableSpace : (len - sent);
                Serial.write(outputLine.c_str() + sent, toSend);
                Serial.flush();

                sent += toSend;
            }
            if (outputLine[len - 1] != '\n') Serial.write("\n", 1);
            Serial.flush();
            serialQueue.pop();
        }
        unlock();
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}

// STDIO VFS methods:

// Register stdio VFS operations
void SerialInterface::register_stdio_vfs() {
    esp_vfs_t stdio_vfs = {
        .flags = ESP_VFS_FLAG_DEFAULT,
        .write = stdio_vfs_write,
        .read = stdio_vfs_read,
        .open = stdio_vfs_open,
        .close = stdio_vfs_close,
    };

    if (esp_vfs_register(STDIO_PATH, &stdio_vfs, NULL) == ESP_OK) {
        serial.log("STDIO VFS Register OK");
    } else serial.err("STDIO VFS Register FAILED");

    // Open new file streams for stdio using our vfs
    auto new_stdin = fopen(STDIN_PATH, "r");
    auto new_stdout = fopen(STDOUT_PATH, "w");
    auto new_stderr = fopen(STDERR_PATH, "w");

    if (new_stdin == 0 || new_stdout == 0 || new_stderr == 0) {
        serial.err("Something wrong with STDIO VFS. Can't open file");
    } else {
        // Replace stdio streams for newly created tasks
        _GLOBAL_REENT->_stdin = new_stdin;
        _GLOBAL_REENT->_stdout = new_stdout;
        _GLOBAL_REENT->_stderr = new_stderr;

        // Close old streams to make writing to them not possible:
        // Note, some tasks created before ours via
        // arduino framework may not deliver some logs anymore
        fclose(stdin);
        fclose(stdout);
        fclose(stderr);

        // Replace stdio streams for current task
        stdin = new_stdin;
        stdout = new_stdout;
        stderr = new_stderr;

        // By default, actual write happens after \n character was met
        // in a file stream. This way we could force write to happen
        // immediately, placing our streams into a non-blocking mode
        setvbuf(stdout, NULL, _IONBF, 0);
        setvbuf(stdin, NULL, _IONBF, 0);
        setvbuf(stderr, NULL, _IONBF, 0);
    }
}

// Compare file path and return proper file descriptor
// Note, this function recieves path after path to vfs
int SerialInterface::stdio_vfs_open(const char* path, int flags, int mode) {
    //serial.log("Trying to open file %s", path);

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

// By default, any application could close any stdio file descriptor
// which would affect globally. We don't need that for real
int SerialInterface::stdio_vfs_close(int fd) {
    // Just pretend we close something if
    // file descriptor is valid
    if (fd >= STDIN_FD && fd <= STDERR_FD) return 0;
    else return -1;
}

ssize_t SerialInterface::stdio_vfs_read(int fd, void* dst, size_t size) {
    if (fd == STDIN_FD) {
        size_t bytesRead = 0;
        uint8_t* dataPtr = static_cast<uint8_t*>(dst);
        //serial.log("trying to read %d bytes. Waiting for data...", size);
        while (bytesRead < size) {
            if (Serial.available() > 0) {
                size_t ReadChunk = Serial.readBytes(dataPtr, size - bytesRead);
                // move forward
                dataPtr += ReadChunk;
                bytesRead += ReadChunk;
                //serial.log("Read in this chunk %d, left %d", ReadChunk, size - bytesRead);
            }
            taskYIELD();
        }
        return bytesRead;
    } else return 0; // Can't read something
}

ssize_t SerialInterface::stdio_vfs_write(int fd, const void* data, size_t size) {
    // Write to uart
    String str = String(static_cast<const char*>(data), size);
    switch (fd) {
        case STDIN_FD:
            // Lol, who writes to stdin
            serial.err("Hey, don't use stdin for output!");
            serial.log(str.c_str()); // should be default esp-idf behavior
            break;
        case STDOUT_FD:
            serial.log(str.c_str());
            break;
        case STDERR_FD:
            serial.err(str.c_str());
    }
    return size;
}

SerialInterface serial;
} // namespace lilka
#include <Arduino.h>
#include "lilka.h"
#include "doom_splash.h"

extern "C" {
#include "doomkeys.h"
#include "m_argv.h"
#include "doomgeneric.h"
#include "d_alloc.h"
}

extern void doomgeneric_Create(int argc, char** argv);
extern void doomgeneric_Tick();

typedef struct {
    uint8_t key;
    bool pressed;
} doomkey_t;

doomkey_t keyqueue[16];
uint16_t keyqueueRead = 0;
uint16_t keyqueueWrite = 0;
uint64_t lastRender = 0;

SemaphoreHandle_t inputMutex;
SemaphoreHandle_t backBufferMutex;
EventGroupHandle_t backBufferEvent;
TaskHandle_t gameTaskHandle;
TaskHandle_t drawTaskHandle;

uint32_t* backBuffer = NULL;

void gameTask(void* arg);
void drawTask(void* arg);

void buttonHandler(lilka::Button button, bool pressed) {
    xSemaphoreTake(inputMutex, portMAX_DELAY);
    doomkey_t* key = &keyqueue[keyqueueWrite];
    switch (button) {
        case lilka::Button::UP:
            key->key = KEY_UPARROW;
            break;
        case lilka::Button::DOWN:
            key->key = KEY_DOWNARROW;
            break;
        case lilka::Button::LEFT:
            key->key = KEY_LEFTARROW;
            break;
        case lilka::Button::RIGHT:
            key->key = KEY_RIGHTARROW;
            break;
        // No strafing
        case lilka::Button::A:
            key->key = KEY_FIRE;
            break;
        case lilka::Button::B:
            key->key = KEY_USE;
            break;
        case lilka::Button::C:
            key->key = KEY_TAB;
            break;
        // TODO: Weapon switch
        // Strafing experiment
        // case lilka::Button::A:
        //     key->key = KEY_STRAFE_R;
        //     break;
        // case lilka::Button::B:
        //     key->key = KEY_FIRE;
        //     break;
        // case lilka::Button::C:
        //     key->key = KEY_USE;
        //     break;
        // case lilka::Button::D:
        //     key->key = KEY_STRAFE_L;
        //     break;
        case lilka::Button::SELECT:
            key->key = KEY_ESCAPE;
            break;
        case lilka::Button::START:
            key->key = KEY_ENTER;
            break;
        default:
            // TODO: Log warning?
            xSemaphoreGive(inputMutex);
            return;
    }

    key->pressed = pressed;
    keyqueueWrite = (keyqueueWrite + 1) % 16;
    xSemaphoreGive(inputMutex);
}

void setup() {
    lilka::display.setSplash(doom_splash);
    lilka::begin();

    inputMutex = xSemaphoreCreateMutex();
    xSemaphoreGive(inputMutex);
    backBufferMutex = xSemaphoreCreateMutex();
    xSemaphoreGive(backBufferMutex);
    backBufferEvent = xEventGroupCreate();
    xEventGroupClearBits(backBufferEvent, 1);

    int argc = 3;
    char arg[] = "doomgeneric";
    char arg2[] = "-iwad";
    char arg3[64];

    // Get firmware arg
    String firmwareFile = lilka::multiboot.getFirmwarePath();
    lilka::serial_log("Firmware file: %s\n", firmwareFile.c_str());
    String firmwareDir;
    if (firmwareFile.length()) {
        // Get directory from firmware file
        int lastSlash = firmwareFile.lastIndexOf('/');
        firmwareDir = firmwareFile.substring(0, lastSlash);
        if (firmwareDir.length() == 0) {
            firmwareDir = "/";
        }
    } else {
        firmwareDir = "/";
    }

    bool found = false;
    // Find the WAD file
    File root = SD.open(firmwareDir.c_str());
    File file;
    while ((file = root.openNextFile())) {
        if (file.isDirectory()) {
            file.close();
            continue;
        }
        String name(file.name());
        name.toLowerCase();
        lilka::serial_log("Checking file: %s\n", name.c_str());
        if (name.startsWith("doom") && name.endsWith(".wad")) {
            strcpy(arg3, (String("/sd") + firmwareDir + "/" + file.name()).c_str());
            lilka::serial_log("Found .WAD file: %s\n", arg3);
            found = true;
            file.close();
            break;
        }
        file.close();
    }
    root.close();
    if (!found) {
        lilka::Alert alert("Doom", "Не знайдено .WAD-файлу на картці пам'яті");
        alert.draw(&lilka::display);
        while (!alert.isFinished()) {
            alert.update();
        }
        esp_restart();
    }
    char* argv[3] = {arg, arg2, arg3};

    DG_printf("Doomgeneric starting, WAD file: %s\n", arg3);

    D_AllocBuffers();
    // Back buffer must be allocated before doomgeneric_Create since it calls DG_DrawFrame
    backBuffer = static_cast<uint32_t*>(malloc(DOOMGENERIC_RESX * DOOMGENERIC_RESY * 4));
    doomgeneric_Create(argc, argv);
    if (backBuffer == NULL) {
        DG_printf("Failed to allocate back buffer\n");
        esp_restart();
    }

    lilka::controller.setGlobalHandler(buttonHandler);

    // while (1) {
    //     doomgeneric_Tick();
    // }

    Serial.println("Ready, starting tasks");

    xTaskCreatePinnedToCore(gameTask, "gameTask", 32768, NULL, 1, &gameTaskHandle, 0);
    xTaskCreatePinnedToCore(drawTask, "drawTask", 32768, NULL, 1, &drawTaskHandle, 1);

    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    // D_FreeBuffers(); // TODO - never reached
}

void gameTask(void* arg) {
    while (1) {
        doomgeneric_Tick();
        taskYIELD();
    }
}

void drawTask(void* arg) {
    while (1) {
        // Wait for buffer to be ready
        xEventGroupWaitBits(backBufferEvent, 1, pdTRUE, pdTRUE, portMAX_DELAY);
        xSemaphoreTake(backBufferMutex, portMAX_DELAY);

        // Calculate FPS
        uint64_t now = millis();
        uint64_t delta = now - lastRender;
        lastRender = now;
        lilka::display.startWrite();
        lilka::display.writeAddrWindow(0, 20, 280, 175);
        uint16_t row[280];
        for (int y = 0; y < 175; y++) {
            for (int x = 0; x < 280; x++) {
                int yy = y * 8 / 7;
                int xx = x * 8 / 7;
                uint32_t pixel = backBuffer[yy * 320 + xx];
                uint8_t r = (pixel >> 16) & 0xff;
                uint8_t g = (pixel >> 8) & 0xff;
                uint8_t b = pixel & 0xff;
                row[x] = lilka::display.color565(r, g, b);
            }
            lilka::display.writePixels(row, 280);
        }
        lilka::display.endWrite();
        lilka::display.setTextBound(0, 0, LILKA_DISPLAY_WIDTH, LILKA_DISPLAY_HEIGHT);
        lilka::display.setCursor(32, 16);
        lilka::display.setTextColor(lilka::colors::White, lilka::colors::Black);
        lilka::display.setFont(FONT_6x12);
        lilka::display.print(" FPS: ");
        lilka::display.print(1000 / delta);
        lilka::display.print(" ");

        xSemaphoreGive(backBufferMutex);
        taskYIELD();
    }
}

extern "C" void DG_Init() {
}

extern "C" void DG_DrawFrame() {
    // Frame is ready.
    // Acquire back buffer, swap buffers and set event
    xSemaphoreTake(backBufferMutex, portMAX_DELAY);
    uint32_t* temp = backBuffer;
    backBuffer = DG_ScreenBuffer;
    DG_ScreenBuffer = temp;
    xEventGroupSetBits(backBufferEvent, 1);
    xSemaphoreGive(backBufferMutex);
}

extern "C" void DG_SetWindowTitle(const char* title) {
    Serial.print("DG: window title: ");
    Serial.println(title);
}

extern "C" void DG_SleepMs(uint32_t ms) {
    delay(ms);
}

extern "C" uint32_t DG_GetTicksMs() {
    return millis();
}

extern "C" int DG_GetKey(int* pressed, unsigned char* doomKey) {
    xSemaphoreTake(inputMutex, portMAX_DELAY);
    int ret;
    if (keyqueueRead != keyqueueWrite) {
        const doomkey_t* key = &keyqueue[keyqueueRead];
        printf("Got key: %d, pressed: %d\n", key->key, key->pressed);
        *pressed = key->pressed;
        *doomKey = key->key;
        keyqueueRead = (keyqueueRead + 1) % 16;
        ret = 1;
    } else {
        ret = 0;
    }
    xSemaphoreGive(inputMutex);
    return ret;
}

bool hadNewLine = true;

extern "C" void DG_printf(const char* format, ...) {
    // Save string to buffer
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    printf("[DG log] %s", buffer);
    int bottom = 280 / 2 + 150 / 2;
    lilka::display.setFont(u8g2_font_6x12_t_cyrillic);
    if (hadNewLine) {
        hadNewLine = false;
        lilka::display.fillRect(0, bottom, 240, 280 - bottom, lilka::colors::Black);
        lilka::display.setCursor(0, bottom + 10);
    }
    lilka::display.setTextBound(0, bottom, 240, 280 - bottom);
    lilka::display.print(buffer);
    for (int i = 0; i < strlen(buffer); i++) {
        if (buffer[i] == '\n') {
            hadNewLine = true;
        }
    }
}

void loop() {
}

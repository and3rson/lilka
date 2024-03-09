#include <Arduino.h>
#include "lilka.h"
#include "doom_splash.h"

extern "C" {
#include "doomkeys.h"
#include "m_argv.h"
#include "doomgeneric.h"
#include "d_alloc.h"
}

extern void doomgeneric_Create(int argc, char **argv);
extern void doomgeneric_Tick();

typedef struct {
    uint8_t key;
    bool pressed;
} doomkey_t;

doomkey_t keyqueue[16];
uint16_t keyqueueRead = 0;
uint16_t keyqueueWrite = 0;
uint64_t lastRender = 0;

void buttonHandler(lilka::Button button, bool pressed) {
    doomkey_t *key = &keyqueue[keyqueueWrite];
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
        case lilka::Button::A:
            key->key = KEY_FIRE;
            break;
        case lilka::Button::B:
            key->key = KEY_ESCAPE;
            break;
        case lilka::Button::SELECT:
            key->key = KEY_TAB;
            break;
        case lilka::Button::START:
            key->key = KEY_ENTER;
            break;
        default:
            // TODO: Log warning?
            return;
    }

    key->pressed = pressed;
    keyqueueWrite = (keyqueueWrite + 1) % 16;
}

void setup() {
    lilka::display.setSplash(doom_splash);
    lilka::begin();

    int argc = 3;
    char arg[] = "doomgeneric";
    char arg2[] = "-iwad";
    char arg3[64];
    bool found = false;
    // Find the wad file
    File root = SD.open("/");
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
            strcpy(arg3, (String("/sd/") + file.name()).c_str());
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
        while (!alert.isDone()) {
            alert.update();
        }
        esp_restart();
    }
    char *argv[3] = {arg, arg2, arg3};

    DG_printf("Doomgeneric starting, wad file: %s\n", arg3);

    D_AllocBuffers();
    doomgeneric_Create(argc, argv);

    lilka::controller.setGlobalHandler(buttonHandler);

    while (1) {
        doomgeneric_Tick();
    }
    D_FreeBuffers(); // TODO - never reached
}

extern "C" void DG_Init() {}

extern "C" void DG_DrawFrame() {
    // Calculate FPS
    uint64_t now = millis();
    uint64_t delta = now - lastRender;
    lastRender = now;
    lilka::display.startWrite();
    // lilka::display.writeAddrWindow(0, 65, 240, 150);
    // Цей код - застарілий. Я адаптував Doom Generic для роботи з 240x150 за замовчуванням.
    // Convert 640x400 to 240x150
    // for (int y = 0; y < 150; y++) {
    //     for (int x = 0; x < 240; x++) {
    //         int yy = y * 8 / 3;
    //         int xx = x * 8 / 3;
    //         uint32_t pixel = DG_ScreenBuffer[yy * 640 + xx];
    //         uint8_t r = (pixel >> 16) & 0xff;
    //         uint8_t g = (pixel >> 8) & 0xff;
    //         uint8_t b = pixel & 0xff;
    //         row[x] = lilka::display.color565(r, g, b);
    //     }
    //     lilka::display.writePixels(row, 240);
    // }
    // Нова версія:
    // for (int y = 0; y < 150; y++) {
    //     for (int x = 0; x < 240; x++) {
    //         uint32_t pixel = DG_ScreenBuffer[y * 240 + x];
    //         uint8_t r = (pixel >> 16) & 0xff;
    //         uint8_t g = (pixel >> 8) & 0xff;
    //         uint8_t b = pixel & 0xff;
    //         row[x] = lilka::display.color565(r, g, b);
    //     }
    //     lilka::display.writePixels(row, 240);
    // }
    // Convert 320x200 to 240x150
    // for (int y = 0; y < 150; y++) {
    //     for (int x = 0; x < 240; x++) {
    //         // Map 240x150 to 320x200
    //         int yy = y * 4 / 3;
    //         int xx = x * 4 / 3;
    //         uint32_t pixel = DG_ScreenBuffer[yy * 320 + xx];
    //         uint8_t r = (pixel >> 16) & 0xff;
    //         uint8_t g = (pixel >> 8) & 0xff;
    //         uint8_t b = pixel & 0xff;
    //         row[x] = lilka::display.color565(r, g, b);
    //     }
    //     lilka::display.writePixels(row, 240);
    // }
    // Convert 320x200 to 280x175 by skipping every 8th pixel
    lilka::display.writeAddrWindow(0, 20, 280, 175);
    uint16_t row[280];
    for (int y = 0; y < 175; y++) {
        for (int x = 0; x < 280; x++) {
            int yy = y * 8 / 7;
            int xx = x * 8 / 7;
            uint32_t pixel = DG_ScreenBuffer[yy * 320 + xx];
            uint8_t r = (pixel >> 16) & 0xff;
            uint8_t g = (pixel >> 8) & 0xff;
            uint8_t b = pixel & 0xff;
            row[x] = lilka::display.color565(r, g, b);
        }
        lilka::display.writePixels(row, 280);
    }
    lilka::display.endWrite();
    lilka::display.setTextBound(0, 0, 240, 280);
    lilka::display.setCursor(0, 32);
    lilka::display.setTextColor(lilka::display.color565(255, 255, 255), lilka::display.color565(0, 0, 0));
    lilka::display.setFont(u8g2_font_10x20_t_cyrillic);
    lilka::display.print(" FPS: ");
    lilka::display.print(1000 / delta);
    lilka::display.print(" ");
}

extern "C" void DG_SetWindowTitle(const char *title) {
    Serial.print("DG: window title: ");
    Serial.println(title);
}

extern "C" void DG_SleepMs(uint32_t ms) {
    delay(ms);
}

extern "C" uint32_t DG_GetTicksMs() {
    return millis();
}

extern "C" int DG_GetKey(int *pressed, unsigned char *doomKey) {
    if (keyqueueRead != keyqueueWrite) {
        doomkey_t *key = &keyqueue[keyqueueRead];
        printf("Got key: %d, pressed: %d\n", key->key, key->pressed);
        *pressed = key->pressed;
        *doomKey = key->key;
        keyqueueRead = (keyqueueRead + 1) % 16;
        return 1;
    }
    return 0;
}

bool hadNewLine = true;

extern "C" void DG_printf(const char *format, ...) {
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
        lilka::display.fillRect(0, bottom, 240, 280 - bottom, lilka::display.color565(0, 0, 0));
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

void loop() {}

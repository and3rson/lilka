#include <Arduino.h>
#include "lilka.h"

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
    lilka::begin();

    int argc = 3;
    char arg[] = "doomgeneric";
    char arg2[] = "-iwad";
    char arg3[] = "/sd/doom.wad";
    char *argv[] = {arg, arg2, arg3};

    Serial.begin(115200);
    Serial.println("Doomgeneric starting...");
    delay(1000);

    D_AllocBuffers();
    doomgeneric_Create(argc, argv);

    lilka::controller.setGlobalHandler(buttonHandler);

    while (1) {
        doomgeneric_Tick();
    }
    D_FreeBuffers();
}

extern "C" void DG_Init() {}

extern "C" void DG_DrawFrame() {
    Serial.println("Draw frame");
    lilka::display.startWrite();
    lilka::display.writeAddrWindow(0, 65, 240, 150);
    uint16_t row[240];
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
    for (int y = 0; y < 150; y++) {
        for (int x = 0; x < 240; x++) {
            // Map 240x150 to 320x200
            int yy = y * 4 / 3;
            int xx = x * 4 / 3;
            uint32_t pixel = DG_ScreenBuffer[yy * 320 + xx];
            uint8_t r = (pixel >> 16) & 0xff;
            uint8_t g = (pixel >> 8) & 0xff;
            uint8_t b = pixel & 0xff;
            row[x] = lilka::display.color565(r, g, b);
        }
        lilka::display.writePixels(row, 240);
    }
    lilka::display.endWrite();
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

void loop() {}

#include <Arduino.h>
#include "lilka.h"

extern "C" {
#include "doomkeys.h"
#include "m_argv.h"
#include "doomgeneric.h"
}

extern void doomgeneric_Create(int argc, char **argv);
extern void doomgeneric_Tick();

char foo[1024];

void setup() {
    lilka::begin();

    int argc = 3;
    char arg[] = "doomgeneric";
    char arg2[] = "-iwad";
    char arg3[] = "/sd/doom.wad";
    char *argv[] = {arg, arg2, arg3};

    Serial.begin(115200);
    Serial.println("Doomgeneric starting...");
    delay(2000);

    for (int i = 0; i < 1024; i++) {
        foo[i] = 0;
    }

    doomgeneric_Create(argc, argv);

    for (int i = 0;; i++) {
        doomgeneric_Tick();
    }
}

extern "C" void DG_Init() {}

extern "C" void DG_DrawFrame() {
    Serial.println("Draw frame");
    lilka::display.startWrite();
    lilka::display.writeAddrWindow(0, 65, 240, 150);
    uint16_t row[240];
    // Convert 640x400 to 240x150
    for (int y = 0; y < 150; y++) {
        for (int x = 0; x < 240; x++) {
            int yy = y * 8 / 3;
            int xx = x * 8 / 3;
            uint32_t pixel = DG_ScreenBuffer[yy * 640 + xx];
            uint8_t r = (pixel >> 16) & 0xff;
            uint8_t g = (pixel >> 8) & 0xff;
            uint8_t b = pixel & 0xff;
            row[x] = lilka::display.color565(r, g, b);
        }
        lilka::display.writePixels(row, 240);
    }
    // for (int y = 0; y < 200; y++) {
    //     // For every 8 rows, draw only 3
    //     if (y % 8 != 0 && y % 8 != 3 && y % 8 != 6) {
    //         continue;
    //     }
    //     uint8_t pixelIndex = 0;
    //     for (int x = 0; x < 320; x++) {
    //         // For every 8 pixels, draw only 3
    //         if (x % 8 != 0 && x % 8 != 3 && x % 8 != 6) {
    //             continue;
    //         }
    //         uint32_t pixel = DG_ScreenBuffer[y * 320 + x];
    //         uint8_t r = (pixel >> 16) & 0xff;
    //         uint8_t g = (pixel >> 8) & 0xff;
    //         uint8_t b = pixel & 0xff;
    //         row[pixelIndex++] = lilka::display.color565(r, g, b);
    //     }
    //     lilka::display.writePixels(row, 240);
    // }
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
    return 0;
}

void loop() {}

#include <Arduino.h>

extern "C" {
#include "doomkeys.h"
#include "m_argv.h"
#include "doomgeneric.h"
}

extern void doomgeneric_Create(int argc, char **argv);
extern void doomgeneric_Tick();

char foo[1024];

void setup() {
    int argc = 1;
    char arg[] = "doomgeneric";
    char *argv[] = {arg};

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

extern "C" void DG_Init() {
}

extern "C" void DG_DrawFrame() {
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

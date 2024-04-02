#pragma once
#include "app.h"
#define PIN_COUNT 6
class GPIOManagerApp : public App {
public:
    GPIOManagerApp();

private:
    uint8_t pinNo[PIN_COUNT] = {LILKA_P3, LILKA_P4, LILKA_P5, LILKA_P6, LILKA_P7, LILKA_P8};
    uint8_t pinData[PIN_COUNT];
    uint8_t pinM[PIN_COUNT] = {INPUT, INPUT, INPUT, INPUT, INPUT, INPUT};
    lilka::Menu menu;
    void readPinData();
    void run() override;
};

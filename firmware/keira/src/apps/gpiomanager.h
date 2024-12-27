#pragma once
#include "app.h"
#define PIN_COUNT             6
#define GET_BIT(data, bit_no) ((data) >> (bit_no)) & 0b1

class GPIOManagerApp : public App {
public:
    GPIOManagerApp();

private:
    uint8_t pinNo[PIN_COUNT] = {LILKA_P3, LILKA_P4, LILKA_P5, LILKA_P6, LILKA_P7, LILKA_P8};
    uint8_t pinData[PIN_COUNT] = {LOW, LOW, LOW, LOW, LOW, LOW};
    uint8_t pinM[PIN_COUNT] = {INPUT, INPUT, INPUT, INPUT, INPUT, INPUT};
    lilka::Menu menu;
    void readPinData();
    void run() override;
    // Testing
    String getStrBits(uint32_t num);
    void readSpeedCompare();
};

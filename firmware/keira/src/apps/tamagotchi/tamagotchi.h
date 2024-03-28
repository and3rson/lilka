#pragma once

#include "app.h"

class TamagotchiApp : public App {
public:
    TamagotchiApp();

private:
    void run() override;
    static TamagotchiApp* instance;

    void drawTriangle(uint8_t x, uint8_t y);
    void drawTamaRow(uint8_t tamaLCD_y, uint8_t ActualLCD_y, uint8_t thick);
    void drawTamaSelection(uint8_t y);
};

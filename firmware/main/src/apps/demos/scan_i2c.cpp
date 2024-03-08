#include <Arduino.h>
#include <Wire.h>

#include "scan_i2c.h"

ScanI2CApp::ScanI2CApp() : App("I2C Scanner") {}

void ScanI2CApp::run() {
    Wire.begin(9, 10, 100000);

    canvas->fillScreen(canvas->color565(0, 0, 0));
    canvas->setTextBound(4, 0, LILKA_DISPLAY_WIDTH - 8, LILKA_DISPLAY_HEIGHT);
    canvas->setCursor(4, 48);
    canvas->println("Starting I2C scan...");
    queueDraw();

    uint8_t found = 0;
    for (uint16_t address = 1; address <= 127; address++) {
        // Wire.requestFrom(addr, 0, true);
        Wire.beginTransmission(address);
        if (Wire.endTransmission() == 0) {
            canvas->printf("%02X", address);
            // lilka::display
            found++;
        } else {
            canvas->print(".");
        }
        if (address % 16 == 0 || address == 127) {
            canvas->println();
        }
    }

    canvas->println("I2C scan done.");
    canvas->printf("Found %d devices.", found);
    queueDraw();

    Wire.end();

    while (!lilka::controller.getState().a.justPressed) {
        taskYIELD();
    }
}
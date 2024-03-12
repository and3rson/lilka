#include <Arduino.h>
#include <Wire.h>

#include "scan_i2c.h"

ScanI2CApp::ScanI2CApp() : App("I2C Scanner") {
}

void ScanI2CApp::run() {
    lilka::Canvas buffer(canvas->width(), canvas->height());
    buffer.begin();
    buffer.fillScreen(0);
    buffer.setFont(FONT_9x15);

    buffer.fillScreen(canvas->color565(0, 0, 0));
    buffer.setTextBound(4, 0, canvas->width() - 8, canvas->height());
    buffer.setCursor(4, 20);
    buffer.println("I2C init: SDA=" + String(LILKA_P3) + ", SCL=" + String(LILKA_P4));
    Wire.begin(LILKA_P3, LILKA_P4, 100000);
    buffer.println("Starting I2C scan...");
    canvas->drawCanvas(&buffer);
    queueDraw();

    uint8_t found = 0;
    for (uint16_t address = 1; address <= 127; address++) {
        // Wire.requestFrom(addr, 0, true);
        Wire.beginTransmission(address);
        if (Wire.endTransmission() == 0) {
            buffer.printf("%02X", address);
            // lilka::display
            found++;
        } else {
            buffer.print(".");
        }
        if (address % 24 == 0 || address == 127) {
            buffer.println();
        }
    }

    buffer.println("I2C scan done.");
    buffer.printf("Found %d devices.", found);
    canvas->drawCanvas(&buffer);
    queueDraw();

    Wire.end();

    while (!lilka::controller.getState().a.justPressed) {
        taskYIELD();
    }
}

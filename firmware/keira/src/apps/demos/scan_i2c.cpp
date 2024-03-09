#include <Arduino.h>
#include <Wire.h>

#include "scan_i2c.h"

ScanI2CApp::ScanI2CApp() : App("I2C Scanner") {
}

void ScanI2CApp::run() {
    lilka::Canvas buffer(canvas->width(), canvas->height());
    buffer.begin();
    buffer.fillScreen(0);

    Wire.begin(9, 10, 100000);

    buffer.fillScreen(canvas->color565(0, 0, 0));
    buffer.setTextBound(4, 0, canvas->width() - 8, canvas->height());
    buffer.setCursor(4, 48);
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
        if (address % 16 == 0 || address == 127) {
            buffer.println();
        }
        if (address % 32 == 0 || address == 127) {
            canvas->drawCanvas(&buffer);
            queueDraw();
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

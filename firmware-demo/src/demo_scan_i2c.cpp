#include <Arduino.h>
#include <Wire.h>

#include <lilka.h>

void demo_scan_i2c() {
    Wire.begin(10, 11);

    lilka::display.fillScreen(lilka::display.color565(0, 0, 0));
    lilka::display.setTextBound(4, 0, LILKA_DISPLAY_WIDTH - 8, LILKA_DISPLAY_HEIGHT);
    lilka::display.setCursor(4, 48);

    lilka::display.println("Starting I2C scan...");

    uint8_t found = 0;
    for (uint8_t address = 1; address <= 127; address++) {
        Wire.beginTransmission(address);
        if (Wire.endTransmission() == 0) {
            Serial.printf("%02X", address);
            found++;
        } else {
            Serial.print(".");
        }
        if (address % 16 == 0) {
            Serial.println();
        }
    }

    lilka::display.println("I2C scan done.");
    lilka::display.printf("Found %d devices.", found);
}

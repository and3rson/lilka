#include <Arduino.h>
#include <Wire.h>
#include "keira.h"
#include "scan_i2c.h"

ScanI2CApp::ScanI2CApp() : App("I2C Scanner") {
}

void ScanI2CApp::run() {
#if LILKA_VERSION >= 2
    lilka::Canvas buffer(canvas->width(), canvas->height());
    buffer.begin();
    buffer.fillScreen(0);
    buffer.setFont(FONT_9x15);

    buffer.fillScreen(lilka::colors::Black);
    buffer.setTextBound(4, 0, canvas->width() - 8, canvas->height());
    buffer.setCursor(4, 20);
    buffer.println(K_S_I2C_SCANNER_SDA_PREFIX + String(LILKA_P3) + K_S_I2C_SCANNER_SCL_PREFIX + String(LILKA_P4));
    Wire.begin(LILKA_P3, LILKA_P4, 100000);
    buffer.println(K_S_I2C_SCANNER_SCAN_START);
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

    buffer.println(K_S_I2C_SCANNER_SCAN_DONE);
    buffer.printf(K_S_I2C_SCANNER_DEVICES_FOUND, found);
    canvas->drawCanvas(&buffer);
    queueDraw();

    Wire.end();

    while (!lilka::controller.getState().a.justPressed) {
        taskYIELD();
    }
#else
    lilka::Alert alert(K_S_ERROR, K_S_LILKA_V2_OR_HIGHER_REQUIRED);
    alert.draw(canvas);
    queueDraw();
    while (!alert.isFinished()) {
        alert.update();
    }
#endif
}

#include "gpiomanager.h"

#include "icons/input.h"
#include "icons/output.h"

GPIOManagerApp::GPIOManagerApp() : App("GPIOManager") {
    menu.setTitle("GPIO");
    // Set default pin mode
    for (int i = 0; i < PIN_COUNT; i++) {
        pinMode(pinNo[i], pinM[i]);
    }
    // Back button
    menu.addActivationButton(lilka::Button::D);
    // Change pin value
    menu.addActivationButton(lilka::Button::A);
    // Change pin mode
    menu.addActivationButton(lilka::Button::B);
    // For testing purposes
    //readSpeedCompare();
}

void GPIOManagerApp::readPinData() {
    uint32_t gpioValue = (REG_READ(GPIO_IN_REG));
    uint32_t gpio1Value = (REG_READ(GPIO_IN1_REG));
    // lilka::serial_log("Pin data : %s %s", getStrBits(gpioValue).c_str(), getStrBits(gpio1Value).c_str());
    for (int i = 0; i < PIN_COUNT; i++) {
        if (pinNo[i] < 32) {
            pinData[i] = GET_BIT(gpioValue, pinNo[i]);
        } else {
            pinData[i] = GET_BIT(gpio1Value, pinNo[i] - 32);
        }
    }
    // for (int i = 0; i < PIN_COUNT; i++) {
    //     if (pinData[i] != digitalRead(pinNo[i])) lilka::serial_err("Pin validation failed");
    // }
}
void GPIOManagerApp::run() {
    while (1) {
        while (!menu.isFinished()) {
            int16_t lastPos = menu.getCursor();
            menu.clearItems();
            readPinData();
            for (int i = 0; i < PIN_COUNT; i++) {
                menu.addItem(
                    String(pinNo[i]) + (pinM[i] == INPUT ? " <- IN" : " -> OUT"),
                    pinM[i] == INPUT ? &input : &output,
                    0,
                    pinData[i] == HIGH ? "HIGH" : "LOW"
                );
            }
            menu.setCursor(lastPos);
            // Do menu redraw
            menu.update();
            menu.draw(canvas);
            queueDraw();
            taskYIELD();
        }
        lilka::serial_log("Menu finished");
        int16_t curPos = menu.getCursor();
        if (curPos == PIN_COUNT) break;
        lilka::Button button = menu.getButton();
        if (button == lilka::Button::A) {
            if (pinM[curPos] == INPUT) {
                // alert
                // can't do that cuz pin is in input mode
                // disaster!!
            } else {
                digitalWrite(pinNo[curPos], pinData[curPos] == HIGH ? LOW : HIGH);
            }
        } else if (button == lilka::Button::B) {
            // Swap Pin Modes
            pinM[curPos] = (pinM[curPos] == INPUT ? OUTPUT : INPUT);
            pinMode(pinNo[curPos], pinM[curPos]);
        } else return;
    }
}
// Methods below used for testing purpose
void GPIOManagerApp::readSpeedCompare() {
    lilka::serial_log("Reading pins using REG_READ");
    uint8_t pinDataRegRead[64];
    uint8_t pinDataDigitalRead[64];
    uint64_t time_begin = micros();
    uint32_t gpioValue = (REG_READ(GPIO_IN_REG));
    uint32_t gpio1Value = (REG_READ(GPIO_IN1_REG));
    // lilka::serial_log("Pin data : %s %s", getStrBits(gpioValue).c_str(), getStrBits(gpio1Value).c_str());
    for (int i = 0; i < 64; i++) {
        if (i < 32) {
            pinDataRegRead[i] = GET_BIT(gpioValue, i);
        } else {
            pinDataRegRead[i] = GET_BIT(gpio1Value, i - 32);
        }
    }
    lilka::serial_log("Elapsed time %d micros", micros() - time_begin);
    lilka::serial_log("Using digitalRead");
    time_begin = micros();
    for (int i = 0; i < 64; i++) {
        pinDataDigitalRead[i] = digitalRead(i);
    }

    lilka::serial_log("Elapsed time %d micros", micros() - time_begin);
    for (int i = 0; i < 64; i++) {
        lilka::serial_log("Pin data %d: %d %d", i, pinDataRegRead[i], pinDataDigitalRead[i]);
    }
}
String GPIOManagerApp::getStrBits(uint32_t num) {
    String bitStr = "";
    for (int i = 0; i < 32; i++) {
        if (i % 4 == 0) bitStr += " ";
        bitStr += ((num >> i) & 1 ? "1" : "0");
    }
    return bitStr;
}

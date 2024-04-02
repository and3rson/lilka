#include "gpiomanager.h"

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
}
String GPIOManagerApp::getStrBits(uint32_t num) {
    String bitStr = "";
    for (int i = 0; i < 32; i++) {
        if (i % 4 == 0) bitStr += " ";
        bitStr += ((num >> i) & 1 ? "1" : "0");
    }
    return bitStr;
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
                    String("Pin ") + (pinM[i] == INPUT ? "I " : "O ") + String(pinNo[i]) + ": " +
                    (pinData[i] == HIGH ? "HIGH" : "LOW")
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
        auto button = menu.getButton();
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
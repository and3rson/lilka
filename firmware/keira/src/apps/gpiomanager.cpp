#include "gpiomanager.h"

GPIOManagerApp::GPIOManagerApp() : App("GPIOManager") {
    menu.setTitle("GPIO");
    // Set default pin mode
    for (int i = 0; i < PIN_COUNT; i++) {
        pinMode(pinNo[i], pinM[i]);
    }
    // Change pin value
    menu.addActivationButton(lilka::Button::A);
    // Change pin mode
    menu.addActivationButton(lilka::Button::B);
}
void GPIOManagerApp::readPinData() {
    for (int i = 0; i < PIN_COUNT; i++) {
        pinData[i] = digitalRead(pinNo[i]);
    }
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
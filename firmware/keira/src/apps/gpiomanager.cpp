#include "gpiomanager.h"

#include "icons/input.h"
#include "icons/output.h"

GPIOManagerApp::GPIOManagerApp() : App("GPIOManager") {
    menu.setTitle("GPIO");
    // Set default pin mode
    for (int i = 0; i < PIN_COUNT; i++) {
        pinMode(pinNo[i], pinM[i]);
    }
    // Change pin mode (cycle through INPUT/OUTPUT/PWM/SQUARE)
    menu.addActivationButton(lilka::Button::D);
    // Change pin value/duty cycle/frequency
    menu.addActivationButton(lilka::Button::A);
    // Back
    menu.addActivationButton(lilka::Button::B);
    
    // For testing purposes
    // readSpeedCompare();
}

void GPIOManagerApp::updatePWM(uint8_t pinIndex) {
    if (pinM[pinIndex] != PIN_MODE_PWM) return;

    uint64_t now = micros();
    uint64_t period = now % pwmPeriod;
    
    if (period * 100 < pwmPeriod * pwmDutyCycle[pinIndex]) {
        digitalWrite(pinNo[pinIndex], HIGH);
        pinData[pinIndex] = HIGH;
    } else{
        digitalWrite(pinNo[pinIndex], LOW);
        pinData[pinIndex] = LOW;
    }
}

void GPIOManagerApp::updateSquareWave(uint8_t pinIndex) {
    if (pinM[pinIndex] != PIN_MODE_SQUARE) return;
    
    uint64_t now = micros();
    uint64_t period = 1000000 / squareFrequency[pinIndex]; // Convert frequency to period in microseconds
    uint64_t elapsed = now - lastUpdate[pinIndex];
    
    if (elapsed >= period) {
        lastUpdate[pinIndex] = now;
        pinData[pinIndex] = !pinData[pinIndex];
        digitalWrite(pinNo[pinIndex], pinData[pinIndex]);
    }
}

void GPIOManagerApp::startPWM(uint8_t pinIndex, uint8_t dutyCycle) {
    if (pinIndex >= PIN_COUNT) return;
    pinM[pinIndex] = PIN_MODE_PWM;
    pwmDutyCycle[pinIndex] = dutyCycle;
    lastUpdate[pinIndex] = micros();
    pinMode(pinNo[pinIndex], OUTPUT);
}

void GPIOManagerApp::startSquareWave(uint8_t pinIndex, uint16_t frequency) {
    if (pinIndex >= PIN_COUNT) return;
    pinM[pinIndex] = PIN_MODE_SQUARE;
    squareFrequency[pinIndex] = frequency;
    lastUpdate[pinIndex] = micros();
    pinMode(pinNo[pinIndex], OUTPUT);
}

void GPIOManagerApp::stopPWM(uint8_t pinIndex) {
    if (pinIndex >= PIN_COUNT) return;
    digitalWrite(pinNo[pinIndex], LOW);
    pinData[pinIndex] = LOW;
    pwmDutyCycle[pinIndex] = 0;
    pinM[pinIndex] = INPUT;
    pinMode(pinNo[pinIndex], INPUT);
}

void GPIOManagerApp::stopSquareWave(uint8_t pinIndex) {
    if (pinIndex >= PIN_COUNT) return;
    digitalWrite(pinNo[pinIndex], LOW);
    pinData[pinIndex] = LOW;
    squareFrequency[pinIndex] = 0;
    pinM[pinIndex] = INPUT;
    pinMode(pinNo[pinIndex], INPUT);
}

void GPIOManagerApp::readPinData() {
    uint32_t gpioValue = (REG_READ(GPIO_IN_REG));
    uint32_t gpio1Value = (REG_READ(GPIO_IN1_REG));
    // lilka::serial_log("Pin data : %s %s", getStrBits(gpioValue).c_str(), getStrBits(gpio1Value).c_str());
    for (int i = 0; i < PIN_COUNT; i++) {
        // Update PWM and square wave pins
        if (pinM[i] == PIN_MODE_PWM) {
            updatePWM(i);
        } else if (pinM[i] == PIN_MODE_SQUARE) {
            updateSquareWave(i);
        } else if (pinM[i] == INPUT) {
            // Only read input pins from GPIO registers
            if (pinNo[i] < 32) {
                pinData[i] = GET_BIT(gpioValue, pinNo[i]);
            } else {
                pinData[i] = GET_BIT(gpio1Value, pinNo[i] - 32);
            }
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
                String modeStr;
                String valueStr;
                
                switch (pinM[i]) {
                    case INPUT:
                        modeStr = " <- IN";
                        valueStr = pinData[i] == HIGH ? "HIGH" : "LOW";
                        break;
                    case OUTPUT:
                        modeStr = " -> OUT";
                        valueStr = pinData[i] == HIGH ? "HIGH" : "LOW";
                        break;
                    case PIN_MODE_PWM:
                        modeStr = " ~ PWM";
                        valueStr = String(pwmDutyCycle[i]) + "%";
                        break;
                    case PIN_MODE_SQUARE:
                        modeStr = " ~ SQ";
                        valueStr = String(squareFrequency[i]) + "Hz";
                        break;
                }
                
                menu.addItem(
                    String(pinNo[i]) + modeStr,
                    pinM[i] == INPUT ? &input_img : &output_img,
                    0,
                    valueStr
                );
            }
            
            menu.setCursor(lastPos);
            // Do menu redraw
            menu.update();
            menu.draw(canvas);
            queueDraw();
        }
        lilka::serial_log("Menu finished");
        int16_t curPos = menu.getCursor();
        if (curPos == PIN_COUNT) break;
        
        lilka::Button button = menu.getButton();
        if (button == lilka::Button::A) {
            switch (pinM[curPos]) {
                case INPUT:
                    // !alert: can't do that cuz pin is in input mode
                    // disaster!!
                    break;
                case OUTPUT:
                    digitalWrite(pinNo[curPos], pinData[curPos] == HIGH ? LOW : HIGH);
                    break;
                case PIN_MODE_PWM:
                    pwmDutyCycle[curPos] = (pwmDutyCycle[curPos] + 10) % 110;
                    break;
                case PIN_MODE_SQUARE:
                    squareFrequency[curPos] = (squareFrequency[curPos] * 2);
                    if (squareFrequency[curPos] > 1000 || squareFrequency[curPos] == 0) {
                        squareFrequency[curPos] = 1;
                    }
                    break;
            }
        } else if (button == lilka::Button::D) {
            // Cycle through modes: INPUT -> OUTPUT -> PWM -> SQUARE
            uint8_t currentMode = pinM[curPos];
            if (currentMode == INPUT) {
                pinM[curPos] = OUTPUT;
                pinMode(pinNo[curPos], OUTPUT);
            } else if (currentMode == OUTPUT) {
                startPWM(curPos, 50); // Start with 50% duty cycle
            } else if (currentMode == PIN_MODE_PWM) {
                stopPWM(curPos);
                startSquareWave(curPos, 1); // Start with 1Hz
            } else {
                stopSquareWave(curPos);
            }
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

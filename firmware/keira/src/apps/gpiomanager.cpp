#include "gpiomanager.h"
#include "icons/input.h"
#include "icons/output.h"

GPIOManagerApp::GPIOManagerApp() : App("GPIOManager") {
    menu.setTitle("GPIO");
    // Set default pin mode
    for (int i = 0; i < PIN_COUNT; i++) {
        pinMode(pinNo[i], pinM[i]);
    }
    // Change pin mode (cycle through INPUT/OUTPUT/PWM)
    menu.addActivationButton(lilka::Button::D);
    // Change pin value/duty cycle
    menu.addActivationButton(lilka::Button::A);
    // Change PWM frequency
    menu.addActivationButton(lilka::Button::C);
    // Back
    menu.addActivationButton(lilka::Button::B);

    // For testing purposes
    //readSpeedCompare();
}

void GPIOManagerApp::updatePWM(uint8_t pinIndex) {
    if (pinM[pinIndex] != PIN_MODE_PWM) return;

    // Configure timer and channel for this pin
    uint32_t freq = ledcSetup(ledcConfigs[pinIndex].channel, pwmFrequency[pinIndex], LEDC_TIMER_RESOLUTION);

    // update frequency for pins with same timer
    for (int i = 0; i < PIN_COUNT; i++) {
        if (ledcConfigs[i].timer == ledcConfigs[pinIndex].timer) {
            pwmFrequency[i] = pwmFrequency[pinIndex];
        }
    }

    if (freq == 0) {
        lilka::serial_log("Failed to set PWM frequency %d Hz for pin %d", pwmFrequency[pinIndex], pinNo[pinIndex]);
    }

    // Set duty cycle (0-100 to 0-1023)
    uint32_t duty = (pwmDutyCycle[pinIndex] * 1023) / 100;
    ledcWrite(ledcConfigs[pinIndex].channel, duty);
}

void GPIOManagerApp::startPWM(uint8_t pinIndex, uint8_t dutyCycle) {
    if (pinIndex >= PIN_COUNT) return;

    // Reset to default
    pinM[pinIndex] = PIN_MODE_PWM;
    pwmDutyCycle[pinIndex] = min(dutyCycle, (uint8_t)100);
    pwmFrequency[pinIndex] = PWM_DEFAULT_FREQ;

    // First attach the pin to its assigned channel
    ledcAttachPin(pinNo[pinIndex], ledcConfigs[pinIndex].channel);

    // Then configure timer and set duty cycle
    updatePWM(pinIndex);
}

void GPIOManagerApp::stopPWM(uint8_t pinIndex) {
    if (pinIndex >= PIN_COUNT) return;

    ledcDetachPin(pinNo[pinIndex]);
    digitalWrite(pinNo[pinIndex], LOW);

    pinData[pinIndex] = LOW;
    pwmFrequency[pinIndex] = 0;
    pwmDutyCycle[pinIndex] = 0;
    pinM[pinIndex] = INPUT;
    pinMode(pinNo[pinIndex], INPUT);
}

void GPIOManagerApp::readPinData() {
    uint32_t gpioValue = (REG_READ(GPIO_IN_REG));
    uint32_t gpio1Value = (REG_READ(GPIO_IN1_REG));
    // lilka::serial_log("Pin data : %s %s", getStrBits(gpioValue).c_str(), getStrBits(gpio1Value).c_str());

    for (int i = 0; i < PIN_COUNT; i++) {
        if (pinM[i] == INPUT) {
            // Only read input pins from GPIO registers
            if (pinNo[i] < 32) {
                pinData[i] = GET_BIT(gpioValue, pinNo[i]);
            } else {
                pinData[i] = GET_BIT(gpio1Value, pinNo[i] - 32);
            }
        }
        // PWM pins are handled by hardware LEDC
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
                    case PIN_INPUT:
                        modeStr = " <- IN";
                        valueStr = pinData[i] == HIGH ? "HIGH" : "LOW";
                        break;
                    case PIN_OUTPUT:
                        modeStr = " -> OUT";
                        valueStr = pinData[i] == HIGH ? "HIGH" : "LOW";
                        break;
                    case PIN_MODE_PWM:
                        modeStr = " ~ PWM";
                        if (pwmFrequency[i] >= 1000) {
                            valueStr = "[" + String(pwmFrequency[i] / 1000) + "kHz] " + String(pwmDutyCycle[i]) + "%";
                        } else {
                            valueStr = "[" + String(pwmFrequency[i]) + "Hz] " + String(pwmDutyCycle[i]) + "%";
                        }
                        break;
                }

                menu.addItem(String(pinNo[i]) + modeStr, pinM[i] == INPUT ? &input_img : &output_img, 0, valueStr);
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
                    updatePWM(curPos);
                    break;
            }
        } else if (button == lilka::Button::C) {
            if (pinM[curPos] == PIN_MODE_PWM) {
                uint32_t newFreq = pwmFrequency[curPos];

                if (newFreq < 1000) {
                    newFreq = newFreq + 100;
                } else {
                    newFreq = newFreq + 1000;
                }

                if (newFreq > LEDC_BASE_FREQ_MAX) {
                    newFreq = LEDC_BASE_FREQ_MIN;
                }

                pwmFrequency[curPos] = newFreq;
                updatePWM(curPos);
            }
        } else if (button == lilka::Button::D) {
            // Cycle through modes: INPUT -> OUTPUT -> PWM
            uint8_t currentMode = pinM[curPos];
            if (currentMode == INPUT) {
                pinM[curPos] = OUTPUT;
                pinMode(pinNo[curPos], OUTPUT);
            } else if (currentMode == OUTPUT) {
                startPWM(curPos, 50); // Start with 50% duty cycle
            } else {
                stopPWM(curPos); // Going back to INPUT
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

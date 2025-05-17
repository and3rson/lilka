#pragma once
#include "app.h"
#include "esp32-hal-ledc.h"
#include "driver/ledc.h"

#define PIN_COUNT             6
#define GET_BIT(data, bit_no) ((data) >> (bit_no)) & 0b1

// Pin modes in addition to INPUT/OUTPUT
#define PIN_INPUT    INPUT
#define PIN_OUTPUT   OUTPUT
#define PIN_MODE_PWM 0x05

// LEDC configuration
#define LEDC_TIMER_RESOLUTION LEDC_TIMER_10_BIT
#define LEDC_BASE_FREQ_MIN    100 // 100 Hz minimum
#define LEDC_BASE_FREQ_MAX    20000 // 20 kHz maximum
#define PWM_DEFAULT_FREQ      2000 // Default starting frequency

struct LEDCConfig {
    uint8_t channel;
    ledc_timer_t timer;
};

class GPIOManagerApp : public App {
public:
    GPIOManagerApp();

private:
    uint8_t pinNo[PIN_COUNT] = {LILKA_P3, LILKA_P4, LILKA_P5, LILKA_P6, LILKA_P7, LILKA_P8};
    uint8_t pinData[PIN_COUNT] = {LOW, LOW, LOW, LOW, LOW, LOW};
    uint8_t pinM[PIN_COUNT] = {INPUT, INPUT, INPUT, INPUT, INPUT, INPUT};

    // PWM configuration
    uint8_t pwmDutyCycle[PIN_COUNT] = {0};
    uint32_t pwmFrequency[PIN_COUNT] = {0};

    // LEDC configurations for each pin
    LEDCConfig ledcConfigs[PIN_COUNT] = {
        {LEDC_CHANNEL_0, LEDC_TIMER_0},
        {LEDC_CHANNEL_2, LEDC_TIMER_1},
        {LEDC_CHANNEL_4, LEDC_TIMER_2},
        {LEDC_CHANNEL_6, LEDC_TIMER_3},
        {LEDC_CHANNEL_1, LEDC_TIMER_0}, // Reuse timer 0
        {LEDC_CHANNEL_3, LEDC_TIMER_1} // Reuse timer 1
    };

    lilka::Menu menu;

    // Basic pin operations
    void readPinData();
    bool isValidPin(uint8_t pinIndex) const {
        return pinIndex < PIN_COUNT;
    }

    // PWM operations
    void updatePWM(uint8_t pinIndex);
    void startPWM(uint8_t pinIndex, uint8_t dutyCycle);
    void stopPWM(uint8_t pinIndex);
    bool isValidDutyCycle(uint8_t dutyCycle) const {
        return dutyCycle <= 100;
    }

    void run() override;

    // Testing utilities
    String getStrBits(uint32_t num);
    void readSpeedCompare();
};

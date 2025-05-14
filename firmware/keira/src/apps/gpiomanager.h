#pragma once
#include "app.h"

#define PIN_COUNT             6
#define GET_BIT(data, bit_no) ((data) >> (bit_no)) & 0b1

// Pin modes in addition to INPUT/OUTPUT
#define PIN_MODE_PWM    0x04
#define PIN_MODE_SQUARE 0x08

// PWM and frequency limits
#define MIN_DUTY_CYCLE    0
#define MAX_DUTY_CYCLE    100
#define PWM_PERIOD_US     20000 // 20ms period (50Hz) by default
#define MIN_FREQUENCY_HZ  1
#define MAX_FREQUENCY_HZ  1000

class GPIOManagerApp : public App {
public:
    GPIOManagerApp();

private:
    uint8_t pinNo[PIN_COUNT] = {LILKA_P3, LILKA_P4, LILKA_P5, LILKA_P6, LILKA_P7, LILKA_P8};
    uint8_t pinData[PIN_COUNT] = {LOW, LOW, LOW, LOW, LOW, LOW};
    uint8_t pinM[PIN_COUNT] = {INPUT, INPUT, INPUT, INPUT, INPUT, INPUT};
    
    // PWM configuration (duty cycle 0-100%)
    uint8_t pwmDutyCycle[PIN_COUNT] = {0};
    uint32_t pwmPeriod = PWM_PERIOD_US;
    
    // Square wave configuration (frequency in Hz)
    uint16_t squareFrequency[PIN_COUNT] = {0};
    
    // Timing variables for PWM and square wave
    uint64_t lastUpdate[PIN_COUNT] = {0};
    
    lilka::Menu menu;
    
    // Basic pin operations
    void readPinData();
    bool isValidPin(uint8_t pinIndex) const { return pinIndex < PIN_COUNT; }
    
    // PWM operations
    void updatePWM(uint8_t pinIndex);
    void startPWM(uint8_t pinIndex, uint8_t dutyCycle);
    void stopPWM(uint8_t pinIndex);
    bool isValidDutyCycle(uint8_t dutyCycle) const { return dutyCycle <= MAX_DUTY_CYCLE; }
    
    // Square wave operations
    void updateSquareWave(uint8_t pinIndex);
    void startSquareWave(uint8_t pinIndex, uint16_t frequency);
    void stopSquareWave(uint8_t pinIndex);
    bool isValidFrequency(uint16_t freq) const { return freq >= MIN_FREQUENCY_HZ && freq <= MAX_FREQUENCY_HZ; }
    
    void run() override;

    // Testing utilities
    String getStrBits(uint32_t num);
    void readSpeedCompare();
};

#ifndef LILKA_BATTERY_H
#define LILKA_BATTERY_H

namespace lilka {

// Номінальні значення для LiPo акумулятора
#define LILKA_DEFAULT_EMPTY_VOLTAGE 3.2
#define LILKA_DEFAULT_FULL_VOLTAGE 4.2

// Батарея підключена до ADC0 через дільник напруги (33 КОм і 100 КОм)
#define LILKA_BATTERY_VOLTAGE_DIVIDER (100.0 / (33.0 + 100.0))

// Напруга акумулятора, при якій ми отримуємо максимальне значення АЦП
// Будь-яка напруга, яка є вищою, буде вважатися 100%
#define LILKA_BATTERY_MAX_MEASURABLE_VOLTAGE 3.1 / LILKA_BATTERY_VOLTAGE_DIVIDER // 4.123 V

class Battery {
public:
    Battery();
    void begin();
    int readLevel();

    void setEmptyVoltage(float voltage);
    void setFullVoltage(float voltage);

private:
    float emptyVoltage;
    float fullVoltage;
};

extern Battery battery;

} // namespace lilka

#endif

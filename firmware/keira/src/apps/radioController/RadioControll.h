#pragma once

#include "app.h"

struct CHData {
    int16_t min_value = -100;
    int16_t max_value = 100;
    int16_t mid_value = 0;
    int16_t current_value;
};

struct CHDataArray {
    static constexpr int8_t NUM_CH = 4;
    CHData ch[NUM_CH];
};

class RadioControllApp : public App {
public:
    RadioControllApp();

private:
    void run() override;
    void SettingsMenu();
    CHDataArray data;
    bool readSettings();
    void saveSetting();
};

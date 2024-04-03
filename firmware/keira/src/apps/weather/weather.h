#pragma once

#include "app.h"

typedef struct {
    bool isConfigured;
    float lat;
    float lon;
} settings_t;

class WeatherApp : public App {
public:
    WeatherApp();

private:
    void run() override;
    bool runSettings();
    settings_t getSettings();
};

#pragma once

#include <Arduino.h>
#include <BleGamepad.h>
#include "fps.h"
#include "timer.h"

namespace ble_gamepad_app {

class Controller : public BleGamepad {
public:
    Controller();
    ~Controller();
    bool start();
    void stop();
    static void controllerTimerCallback(TimerHandle_t xTimer);
    void updateControllerState();
    bool isActive();

private:
    static constexpr const int NUM_OF_BUTTONS = 4;

    int batteryLevel;
    FPS mainLoopFps;
    TimerHandle_t controllerTimer;
    Timer checkBatteryLevelTimer;
    bool active;

    bool updateBatteryLevel();
    bool updateButtons();
};

} // namespace ble_gamepad_app
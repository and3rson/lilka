#include <Arduino.h>
#include <NimBLEDevice.h>
#include <lilka.h>
#include "defines.h"
#include "controller.h"

#define BUTTON_A BUTTON_2
#define BUTTON_B BUTTON_1
#define BUTTON_C BUTTON_4
#define BUTTON_D BUTTON_3

namespace ble_gamepad_app {

Controller::Controller() :
    BleGamepad(DEVICE_NAME),
    active(false),
    mainLoopFps(MAIN_LOOP_FPS_WATCHER, CONTROLLER_TIMER_DELAY_MILLIS),
    batteryLevel(BATTERY_LEVEL_UNKNOWN),
    checkBatteryLevelTimer(ONE_MINUTE) {
}

bool Controller::start() {
    lilka::serial_log("[%s] Starting BLE", LOG_TAG);

    controllerTimer = xTimerCreate(
        "bleControllerTimer",
        pdMS_TO_TICKS(CONTROLLER_TIMER_DELAY_MILLIS),
        pdTRUE,
        static_cast<void*>(this),
        Controller::controllerTimerCallback
    );
    if (controllerTimer != NULL) {
        xTimerStart(controllerTimer, 0);

        BleGamepadConfiguration cfg;
        cfg.setControllerType(CONTROLLER_TYPE_GAMEPAD);
        cfg.setAutoReport(false);
        cfg.setButtonCount(NUM_OF_BUTTONS);
        cfg.setIncludeStart(true);
        cfg.setIncludeSelect(true);
        cfg.setWhichAxes(false, false, false, false, false, false, false, false);
        cfg.setWhichSimulationControls(false, false, false, false, false);
        begin(&cfg);

        return active = true;
    }
    return active;
}

bool Controller::isActive() {
    return active;
}

void Controller::controllerTimerCallback(TimerHandle_t xTimer) {
    Controller* instance = static_cast<Controller*>(pvTimerGetTimerID(xTimer));
    if (instance) {
        instance->updateControllerState();
    }
}

void Controller::updateControllerState() {
    mainLoopFps.onStartFrame();
    if (!isConnected()) {
        return;
    }
    bool needReport = false;
    if (checkBatteryLevelTimer.isTimeOnFirstCall()) {
        checkBatteryLevelTimer.go();
        needReport |= updateBatteryLevel();
    }
    needReport |= updateButtons();
    if (needReport) {
        sendReport();
    }
    mainLoopFps.onEndFrame();
    if (DEBUG) {
        mainLoopFps.logEveryOneSecond();
    }
}

bool Controller::updateBatteryLevel() {
    int newBatteryLevel = lilka::battery.readLevel();
    if (batteryLevel == newBatteryLevel || newBatteryLevel < BATTERY_LEVEL_NIN || newBatteryLevel > BATTERY_LEVEL_MAX) {
        return false;
    }
    batteryLevel = newBatteryLevel;
    setBatteryLevel(batteryLevel);
    if (DEBUG) {
        lilka::serial_log("[%s] New battery level: %d", LOG_TAG, batteryLevel);
    }
    return true;
}

bool Controller::updateButtons() {
    lilka::State st = lilka::controller.getState();
    bool needReport = st.any.justPressed || st.any.justReleased;
    if (st.up.justPressed || st.right.justPressed || st.down.justPressed || st.left.justPressed || st.up.justReleased ||
        st.right.justReleased || st.down.justReleased || st.left.justReleased) {
        if (!st.up.pressed && !st.right.pressed && !st.down.pressed && !st.left.pressed) {
            setHat(DPAD_CENTERED);
        } else {
            if (st.up.pressed) {
                if (st.right.pressed) {
                    setHat(DPAD_UP_RIGHT);
                } else if (st.left.pressed) {
                    setHat(DPAD_UP_LEFT);
                } else {
                    setHat(DPAD_UP);
                }
            } else if (st.down.pressed) {
                if (st.right.pressed) {
                    setHat(DPAD_DOWN_RIGHT);
                } else if (st.left.pressed) {
                    setHat(DPAD_DOWN_LEFT);
                } else {
                    setHat(DPAD_DOWN);
                }
            } else if (st.right.pressed) {
                setHat(DPAD_RIGHT);
            } else { // st.left.pressed
                setHat(DPAD_LEFT);
            }
        }
    }
    if (st.a.justPressed) {
        press(BUTTON_A);
    } else if (st.a.justReleased) {
        release(BUTTON_A);
    }
    if (st.b.justPressed) {
        press(BUTTON_B);
    } else if (st.b.justReleased) {
        release(BUTTON_B);
    }
    if (st.c.justPressed) {
        press(BUTTON_C);
    } else if (st.c.justReleased) {
        release(BUTTON_C);
    }
    if (st.d.justPressed) {
        press(BUTTON_D);
    } else if (st.d.justReleased) {
        release(BUTTON_D);
    }
    if (st.select.justPressed) {
        pressSelect();
    } else if (st.select.justReleased) {
        releaseSelect();
    }
    if (st.start.justPressed) {
        pressStart();
    } else if (st.start.justReleased) {
        releaseStart();
    }
    return needReport;
}

void Controller::stop() {
    if (!active) {
        return;
    }
    lilka::serial_log("[%s] Stopping BLE", LOG_TAG);
    int waitTime = 100 / portTICK_PERIOD_MS;
    end();
    if (controllerTimer != NULL) {
        xTimerStop(controllerTimer, pdFALSE);
        xTimerDelete(controllerTimer, 0);
    }
    vTaskDelay(waitTime);
    NimBLEDevice::stopAdvertising();
    vTaskDelay(waitTime);
    std::list<NimBLEClient*>* clients = NimBLEDevice::getClientList();
    for (auto it = clients->begin(); it != clients->end(); ++it) {
        NimBLEClient* client = *it;
        NimBLEDevice::deleteClient(client);
    }
    clients = nullptr;
    vTaskDelay(waitTime);
    NimBLEDevice::deinit(true);
    vTaskDelay(waitTime);
    esp_restart();
    active = false;
}

Controller::~Controller() {
    stop();
}

} // namespace ble_gamepad_app
#pragma once

#include <app.h>
#include "fps.h"
#include "controller.h"

namespace ble_gamepad_app {

class MainApp : public App {
public:
    MainApp();
    ~MainApp() override;

private:
    void run() override;
    void uiLoop();
    void onStop() override;
    bool isExitHotkeyPressed();
    void cleanUp();

    Controller bleGamepadController;
    FPS uiFps;
    int lastSecondsToExit;
};

} // namespace ble_gamepad_app
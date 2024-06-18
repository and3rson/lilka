#include "defines.h"
#include "app.h"
#include "ui.h"

#define EXIT_BUTTON_PRESS_SECONDS 5
#define EXIT_BUTTON_PRESS_DELAY   ONE_SECOND* EXIT_BUTTON_PRESS_SECONDS

namespace ble_gamepad_app {

MainApp::MainApp() :
    App(BLE_GAMEPAD_APP_NAME), uiFps(UI_FPS_WATCHER, UI_DELAY_MILLIS), lastSecondsToExit(EXIT_BUTTON_PRESS_SECONDS) {
}

void MainApp::run() {
    if (!bleGamepadController.start()) {
        lilka::serial_err("[%s] Starting BLE error!", LOG_TAG);
        bleGamepadController.stop();
    }
    uiLoop();
}

void MainApp::uiLoop() {
    UI ui(canvas->width(), canvas->height());
    while (bleGamepadController.isActive()) {
        uiFps.onStartFrame();
        if (isExitHotkeyPressed()) {
            onStop();
            return;
        }
        ui.drawFrame(bleGamepadController.isConnected(), lastSecondsToExit);
        canvas->drawCanvas(ui.getFrameBuffer());
        queueDraw();
        uiFps.onEndFrame();
        vTaskDelay(uiFps.getLimitMillis() / portTICK_PERIOD_MS);
        if (DEBUG) {
            uiFps.logEveryOneSecond();
        }
    }
}

bool MainApp::isExitHotkeyPressed() {
    lilka::State st = lilka::controller.peekState();
    lilka::ButtonState hotkeyState = st.select;
    if (!hotkeyState.pressed) {
        lastSecondsToExit = EXIT_BUTTON_PRESS_SECONDS;
        return false;
    }
    uint64_t hotkeyPressTime = hotkeyState.time;
    uint64_t curTime = millis();
    uint64_t delta = curTime - hotkeyPressTime;
    lastSecondsToExit = EXIT_BUTTON_PRESS_SECONDS - delta / ONE_SECOND;
    if (lastSecondsToExit < 0) {
        lastSecondsToExit = 0;
    }
    return lastSecondsToExit == 0;
}

void MainApp::cleanUp() {
    bleGamepadController.stop();
}

void MainApp::onStop() {
    cleanUp();
}

MainApp::~MainApp() {
    cleanUp();
}

} // namespace ble_gamepad_app

#pragma once

namespace ble_gamepad_app {

class FPS {
public:
    FPS(const char* name, int delayLimitMillis);
    void onStartFrame();
    void onEndFrame();
    unsigned long getLimitMillis();
    void logEveryOneSecond();
    unsigned long getDelta();

private:
    const char* name;
    const int delayLimitMillis, fpsLimit;
    unsigned long startFrameTime, endFrameTime, startLogTime, minFrameTime, maxFrameTime, allFrameTime;
    int fpsPerSecondTotal;
};

} // namespace ble_gamepad_app
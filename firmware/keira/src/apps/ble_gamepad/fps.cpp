#include <Arduino.h>
#include <lilka.h>
#include "defines.h"
#include "fps.h"

namespace ble_gamepad_app {

FPS::FPS(const char* name, int delayLimitMillis) :
    name(name),
    delayLimitMillis(delayLimitMillis),
    fpsLimit(ONE_SECOND / delayLimitMillis),
    startFrameTime(0),
    endFrameTime(0),
    startLogTime(0),
    fpsPerSecondTotal(0),
    minFrameTime(0),
    maxFrameTime(0),
    allFrameTime(0) {
}

void FPS::onStartFrame() {
    startFrameTime = millis();
}

void FPS::onEndFrame() {
    endFrameTime = millis();
}

unsigned long FPS::getDelta() {
    if (startFrameTime > endFrameTime) {
        return (ULONG_MAX - startFrameTime) + endFrameTime + 1;
    }
    return endFrameTime - startFrameTime;
}

unsigned long FPS::getLimitMillis() {
    unsigned long delta = getDelta();
    // Incorrect delay time. Using default limits
    if (delta == 0) {
        return delayLimitMillis;
    }
    // Too long delay time. Using min limits
    if (delta >= delayLimitMillis) {
        return 1;
    }
    // Delay adjustment
    unsigned long res = delayLimitMillis - delta;
    return res;
}

void FPS::logEveryOneSecond() {
    if (!startLogTime) {
        startLogTime = startFrameTime;
    }
    if (!startLogTime) {
        lilka::serial_err("[%s] [%s] The onStartFrame method must be called", LOG_TAG, name);
        return;
    }
    if (!endFrameTime) {
        lilka::serial_err("[%s] [%s] The onEndFrame method must be called", LOG_TAG, name);
        return;
    }
    unsigned long deltaFrame = getDelta();
    if (!minFrameTime) {
        minFrameTime = deltaFrame;
    } else if (deltaFrame < minFrameTime) {
        minFrameTime = deltaFrame;
    }
    if (!maxFrameTime) {
        maxFrameTime = deltaFrame;
    } else if (deltaFrame > maxFrameTime) {
        maxFrameTime = deltaFrame;
    }
    allFrameTime += deltaFrame;
    unsigned long delta = millis() - startLogTime;
    fpsPerSecondTotal++;
    if (delta < ONE_SECOND) {
        return;
    }
    if (DEBUG) {
        lilka::serial_log(
            "[%s] [%s] Total: %lu millis, %d fps; Min: %lu millis; Avg: %lu; Max: %lu millis",
            LOG_TAG,
            name,
            delta,
            fpsPerSecondTotal,
            minFrameTime,
            allFrameTime / fpsPerSecondTotal,
            maxFrameTime
        );
    }

    startLogTime = maxFrameTime = minFrameTime = allFrameTime = 0;
    fpsPerSecondTotal = 0;
}

} // namespace ble_gamepad_app
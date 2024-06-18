#include <Arduino.h>
#include "timer.h"

namespace ble_gamepad_app {

Timer::Timer(int periodMillis) : period(periodMillis), start(0) {
}

unsigned long Timer::go() {
    return start = getTime();
}

bool Timer::isTimeOnFirstCall() {
    return isTime(true);
}

bool Timer::isTime(bool triggerOnFirstCall) {
    if (!start) {
        go();
        return triggerOnFirstCall;
    }
    unsigned long curTime = getTime();
    if (start > curTime) {
        return (ULONG_MAX - start) + curTime + 1;
    }
    return (curTime - start) >= period;
}

unsigned long Timer::getTime() {
    return millis();
}

} // namespace ble_gamepad_app
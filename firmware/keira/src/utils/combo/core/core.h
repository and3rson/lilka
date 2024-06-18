#pragma once

#include <lilka.h>

using lilka::_StateButtons;
using lilka::Button;
using lilka::ButtonState;
using lilka::State;

using ComboCompletedEventHandler = void (*)(void);
using ComboResetEventHandler = void (*)(void);

class ComboBase {
public:
    ComboBase() :
        completed(false),
        started(false),
        autoStart(true),
        autoReset(true),
        completedEventHandler(nullptr),
        resetEventHandler(nullptr) {
    }
    ~ComboBase() {
        completedEventHandler = nullptr;
        resetEventHandler = nullptr;
    }
    bool isCompleted() {
        return completed;
    };
    bool isStarted() {
        return started;
    }
    void start() {
        started = true;
    }
    bool isAutoStart() {
        return autoStart;
    }
    void setAutoStart(bool autoStart) {
        this->autoStart = autoStart;
    }
    bool isAutoReset() {
        return autoReset;
    }
    void setAutoReset(bool autoReset) {
        this->autoReset = autoReset;
    }
    void setCompletedEventHandler(ComboCompletedEventHandler handler) {
        completedEventHandler = handler;
    }
    void setResetEventHandler(ComboResetEventHandler handler) {
        resetEventHandler = handler;
    }
    void loop(State& state) {
        if (!isStarted() && isAutoStart()) {
            start();
        }
        if (isStarted()) {
            loopAction(state);
        }
    };
    void reset() {
        started = false;
        completed = false;
        resetAction();
    };
    static bool isNonTargetButtonPressed(Button target, const _StateButtons& buttonStates) {
        bool anyButtonPressed = buttonStates[Button::ANY].pressed;
        if (!anyButtonPressed || target == Button::ANY) {
            return false;
        }
        size_t size = Button::COUNT - 1; // exclude ANY button
        for (size_t i = 0; i < size; i++) {
            if (i == target) {
                continue;
            }
            if (buttonStates[i].pressed) {
                return true;
            }
        }
        return false;
    }

protected:
    void complete() {
        if (!completed) {
            completed = true;
            onCompleted();
        }
    }
    void onCompleted() {
        if (completedEventHandler) completedEventHandler();
    }
    void onReset() {
        if (resetEventHandler) resetEventHandler();
    }
    void checkAutoReset() {
        if (isAutoReset()) {
            reset();
        }
    }

    virtual void loopAction(State& state) = 0;
    virtual void resetAction() = 0;

private:
    bool completed, started, autoStart, autoReset;
    ComboCompletedEventHandler completedEventHandler;
    ComboResetEventHandler resetEventHandler;
};

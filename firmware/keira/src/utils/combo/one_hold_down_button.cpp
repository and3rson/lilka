#include "one_hold_down_button.h"

ComboOneHoldDownButton::ComboOneHoldDownButton(Button button, uint16_t holdDownTime, bool checkNonTargetButtons) :
    ComboBase(),
    progressEventHandler(nullptr),
    holdDownTime(holdDownTime),
    checkNonTargetButtons(checkNonTargetButtons) {
    buttonStage.changeButtonAndReset(button);
}

ComboOneHoldDownButton::~ComboOneHoldDownButton() {
    progressEventHandler = nullptr;
}

void ComboOneHoldDownButton::onProgress() {
    if (!progressEventHandler) {
        return;
    }
    uint16_t leftTimeToComplete = holdDownTime;
    if (buttonStage.getPressTime()) {
        uint32_t delta = millis() - buttonStage.getPressTime();
        if (delta >= holdDownTime) {
            leftTimeToComplete = 0;
        } else {
            leftTimeToComplete = holdDownTime - delta;
        }
    }
    progressEventHandler(buttonStage.getButton(), buttonStage.getStage(), leftTimeToComplete);
}

bool ComboOneHoldDownButton::isTimeUp() {
    if (buttonStage.getPressTime()) {
        return (millis() - buttonStage.getPressTime()) >= holdDownTime;
    }
    return false;
}

void ComboOneHoldDownButton::loopAction(State& state) {
    if (buttonStage.isReleased()) {
        return;
    }
    const _StateButtons& buttonStates = *reinterpret_cast<const _StateButtons*>(&state);
    if (checkNonTargetButtons && buttonStage.isPressed() && !isCompleted() &&
        isNonTargetButtonPressed(buttonStage.getButton(), buttonStates)) {
        resetAction();
        onReset();
        return;
    }
    const ButtonState& bs = buttonStates[buttonStage.getButton()];
    switch (buttonStage.getStage()) {
        case WAITING:
            if (bs.justPressed) {
                buttonStage.setPressedStage(bs.time);
                onProgress();
            }
            break;
        case PRESSED:
            if (!isCompleted()) {
                onProgress();
                if (isTimeUp()) {
                    complete();
                }
            }
            if (bs.justReleased) {
                if (isCompleted()) {
                    buttonStage.setReleasedStage(bs.time);
                    onProgress();
                    checkAutoReset();
                } else {
                    resetAction();
                    onReset();
                }
            }
            break;
        case RELEASED:
        default:
            break;
    }
}

void ComboOneHoldDownButton::resetAction() {
    buttonStage.reset();
}

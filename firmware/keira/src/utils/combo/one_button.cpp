#include "one_button.h"

ComboOneButton::ComboOneButton(Button button) : ComboBase(), progressEventHandler(nullptr) {
    buttonStage.changeButtonAndReset(button);
}

ComboOneButton::~ComboOneButton() {
    progressEventHandler = nullptr;
}

void ComboOneButton::onProgress() {
    if (!progressEventHandler) {
        return;
    }
    progressEventHandler(buttonStage.getButton(), buttonStage.getStage());
}

void ComboOneButton::loopAction(State& state) {
    if (buttonStage.isReleased()) {
        return;
    }
    const _StateButtons& buttonStates = *reinterpret_cast<const _StateButtons*>(&state);
    const ButtonState& bs = buttonStates[buttonStage.getButton()];
    switch (buttonStage.getStage()) {
        case WAITING:
            if (bs.justPressed) {
                buttonStage.setStage(PRESSED);
                onProgress();
                complete();
            }
            break;
        case PRESSED:
            if (bs.justReleased) {
                buttonStage.setStage(RELEASED);
                onProgress();
                checkAutoReset();
            }
            break;
        case RELEASED:
        default:
            break;
    }
}

void ComboOneButton::resetAction() {
    buttonStage.reset();
}

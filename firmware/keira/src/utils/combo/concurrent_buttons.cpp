#include "concurrent_buttons.h"

ComboConcurrentButtons::ComboConcurrentButtons(std::initializer_list<Button> buttonList) :
    ComboBase(),
    NUM_BUTTONS(buttonList.size() ? buttonList.size() : 1),
    buttons(new Button[NUM_BUTTONS]),
    progressEventHandler(nullptr) {
    initButtons(buttonList);
}

ComboConcurrentButtons::~ComboConcurrentButtons() {
    buttons.reset();
    progressEventHandler = nullptr;
}

void ComboConcurrentButtons::initButtons(std::initializer_list<Button> buttonList) {
    if (buttonList.size()) {
        auto it = buttonList.begin();
        for (size_t i = 0; i < NUM_BUTTONS && it != buttonList.end(); ++i, ++it) {
            buttons[i] = *it;
        }
    } else {
        buttons[0] = Button::ANY;
    }
    buttonStage.changeButtonAndReset(Button::ANY);
}

void ComboConcurrentButtons::onProgress() {
    if (!progressEventHandler) {
        return;
    }
    progressEventHandler(NUM_BUTTONS, buttons.get(), buttonStage.getStage());
}

void ComboConcurrentButtons::loopAction(State& state) {
    if (buttonStage.isReleased()) {
        return;
    }
    const _StateButtons& buttonStates = *reinterpret_cast<const _StateButtons*>(&state);
    switch (buttonStage.getStage()) {
        case WAITING:
            checkAllButtonsPressed(buttonStates);
            break;
        case PRESSED:
            checkAllButtonsReleased(buttonStates);
            break;
        case RELEASED:
        default:
            break;
    }
}

void ComboConcurrentButtons::resetAction() {
    buttonStage.reset();
}

bool ComboConcurrentButtons::isTargetButton(size_t buttonIndex) {
    for (size_t i = 0; i < NUM_BUTTONS; i++) {
        if (buttons[i] == Button::ANY || buttonIndex == buttons[i]) {
            return true;
        }
    }
    return false;
}

bool ComboConcurrentButtons::isNonTargetButtonsPressed(const _StateButtons& buttonStates) {
    if (!buttonStates[Button::ANY].pressed) {
        return false;
    }
    size_t size = Button::COUNT - 1; // exclude ANY button
    for (size_t i = 0; i < size; i++) {
        if (buttonStates[i].pressed && !isTargetButton(i)) {
            return true;
        }
    }
    return false;
}

void ComboConcurrentButtons::checkAllButtonsPressed(const _StateButtons& buttonStates) {
    if (isNonTargetButtonsPressed(buttonStates)) {
        return;
    }
    bool allButtonsPressed = true;
    bool anyButtonJustPressed = false;
    for (size_t i = 0; i < NUM_BUTTONS; i++) {
        const ButtonState& bs = buttonStates[buttons[i]];
        if (!bs.pressed) {
            allButtonsPressed = false;
            break;
        }
        if (bs.justPressed) {
            anyButtonJustPressed = true;
        }
    }
    if (allButtonsPressed && anyButtonJustPressed) {
        buttonStage.setStage(PRESSED);
        onProgress();
        complete();
    }
}

void ComboConcurrentButtons::checkAllButtonsReleased(const _StateButtons& buttonStates) {
    bool allButtonsReleased = true;
    bool anyButtonJustReleased = false;
    for (size_t i = 0; i < NUM_BUTTONS; i++) {
        const ButtonState& bs = buttonStates[buttons[i]];
        if (bs.pressed) {
            allButtonsReleased = false;
            break;
        }
        if (bs.justReleased) {
            anyButtonJustReleased = true;
        }
    }
    if (allButtonsReleased && anyButtonJustReleased) {
        buttonStage.setStage(RELEASED);
        onProgress();
        checkAutoReset();
    }
}

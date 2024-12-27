#include "sequence_buttons.h"

#define DEFAULT_TIMEOUT 750

ComboButtonSequence::ComboButtonSequence(std::initializer_list<Button> buttonList) :
    ComboBase(),
    NUM_BUTTONS(buttonList.size() ? buttonList.size() : 1),
    buttonIndex(0),
    buttons(new Button[NUM_BUTTONS]),
    progressEventHandler(nullptr),
    firstComboButtonAlreadyPressed(false),
    timeout(DEFAULT_TIMEOUT) {
    initButtons(buttonList);
}

ComboButtonSequence::~ComboButtonSequence() {
    buttons.reset();
    progressEventHandler = nullptr;
}

void ComboButtonSequence::initButtons(std::initializer_list<Button> buttonList) {
    if (buttonList.size()) {
        auto it = buttonList.begin();
        for (size_t i = 0; i < NUM_BUTTONS && it != buttonList.end(); ++i, ++it) {
            buttons[i] = *it;
        }
    } else {
        buttons[0] = Button::ANY;
    }
    buttonStage.changeButtonAndReset(buttons[0]);
}

void ComboButtonSequence::onProgress() {
    if (!progressEventHandler) {
        return;
    }
    progressEventHandler(buttonIndex, buttonStage.getButton(), buttonStage.getStage());
}

void ComboButtonSequence::loopAction(State& state) {
    if (areAllButtonsProcessed()) {
        return;
    }
    const _StateButtons& buttonStates = *reinterpret_cast<const _StateButtons*>(&state);
    if (firstComboButtonAlreadyPressed && !isCompleted()) {
        if (buttonStage.isWaiting() && isTimeout(buttonStage.getPrevReleaseTime()) ||
            buttonStage.isPressed() && isTimeout(buttonStage.getPressTime()) ||
            isNonTargetButtonPressed(buttonStage.getButton(), buttonStates)) {
            resetAction();
            onReset();
            return;
        }
    }

    const ButtonState& bs = buttonStates[buttonStage.getButton()];
    switch (buttonStage.getStage()) {
        case WAITING:
            if (bs.justPressed) {
                firstComboButtonAlreadyPressed = true;
                buttonStage.setPressedStage(bs.time);
                onProgress();
                if (isLastButtonProcessed()) {
                    complete();
                }
            }
            break;
        case PRESSED:
            if (bs.justReleased) {
                buttonStage.setReleasedStage(bs.time);
                onProgress();
                selectNextButtonForProcessing();
                if (areAllButtonsProcessed()) {
                    checkAutoReset();
                }
            }
            break;
        case RELEASED:
        default:
            break;
    }
}

void ComboButtonSequence::resetAction() {
    firstComboButtonAlreadyPressed = false;
    buttonIndex = 0;
    buttonStage.changeButtonAndReset(buttons[buttonIndex]);
}

bool ComboButtonSequence::isLastButtonProcessed() {
    return buttonIndex == NUM_BUTTONS - 1;
}

bool ComboButtonSequence::areAllButtonsProcessed() {
    return buttonIndex >= NUM_BUTTONS;
}

void ComboButtonSequence::selectNextButtonForProcessing() {
    if (areAllButtonsProcessed()) {
        return;
    }
    if (isLastButtonProcessed()) {
        buttonIndex = NUM_BUTTONS;
        return;
    }
    buttonStage.changeButtonAndReset(buttons[++buttonIndex]);
}

bool ComboButtonSequence::isTimeout(uint32_t time) {
    return (millis() - time) >= timeout;
}

void ComboButtonSequence::setTimeout(uint16_t timeout) {
    this->timeout = timeout;
}

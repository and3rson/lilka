#pragma once

#include <lilka.h>

using lilka::Button;

typedef enum { WAITING = 0, PRESSED, RELEASED } ButtonStage;

class ComboButtonStage {
public:
    ComboButtonStage() : button(Button::ANY), stage(WAITING) {
    }
    virtual void reset() {
        stage = WAITING;
    }
    virtual void changeButtonAndReset(Button btn) {
        button = btn;
        reset();
    }
    inline Button getButton() {
        return button;
    }
    inline ButtonStage getStage() {
        return stage;
    }
    void setStage(ButtonStage bs) {
        stage = bs;
    }
    bool isWaiting() {
        return stage == WAITING;
    }
    bool isPressed() {
        return stage == PRESSED;
    }
    bool isReleased() {
        return stage == RELEASED;
    }

private:
    Button button;
    ButtonStage stage;
};
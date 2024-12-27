#pragma once

#include "stage.h"

class ComboButtonWithTimeStage : public ComboButtonStage {
public:
    ComboButtonWithTimeStage() : pressTime(0), releaseTime(0), prevReleaseTime(0) {
    }
    void reset() override {
        ComboButtonStage::reset();
        prevReleaseTime = releaseTime;
        pressTime = releaseTime = 0;
    }
    void changeButtonAndReset(Button btn) override {
        ComboButtonStage::changeButtonAndReset(btn);
    }
    void setPressedStage(uint32_t time) {
        setStage(PRESSED);
        pressTime = time ? time : millis();
    }
    void setReleasedStage(uint32_t time) {
        setStage(RELEASED);
        releaseTime = time ? time : millis();
    }
    uint32_t getPressTime() {
        return pressTime;
    }
    uint32_t getReleaseTime() {
        return releaseTime;
    }
    uint32_t getPrevReleaseTime() {
        return prevReleaseTime;
    }

private:
    uint32_t pressTime, releaseTime, prevReleaseTime;
};
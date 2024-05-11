#pragma once

#include "core/core.h"
#include "core/stage_time.h"

using ComboOneHoldDownButtonProgressEventHandler =
    void (*)(const Button button, const ButtonStage stage, uint16_t leftTimeToComplete);

class ComboOneHoldDownButton : public ComboBase {
public:
    ComboOneHoldDownButton(Button button, uint16_t holdDownTime, bool checkNonTargetButtons = true);
    ~ComboOneHoldDownButton();

    void setProgressEventHandler(ComboOneHoldDownButtonProgressEventHandler handler) {
        progressEventHandler = handler;
    }

private:
    const uint16_t holdDownTime;
    const bool checkNonTargetButtons;
    ComboButtonWithTimeStage buttonStage;
    ComboOneHoldDownButtonProgressEventHandler progressEventHandler;
    bool isTimeUp();
    void onProgress();
    void loopAction(State& state) override;
    void resetAction() override;
};
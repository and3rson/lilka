#pragma once

#include "core/core.h"
#include "core/stage.h"

using ComboConcurrentButtonsProgressEventHandler =
    void (*)(const size_t numOfButtons, const Button buttons[], const ButtonStage stage);

class ComboConcurrentButtons : public ComboBase {
public:
    ComboConcurrentButtons(std::initializer_list<Button> buttonList);
    ~ComboConcurrentButtons();

    void setProgressEventHandler(ComboConcurrentButtonsProgressEventHandler handler) {
        progressEventHandler = handler;
    }

private:
    const size_t NUM_BUTTONS;
    std::unique_ptr<Button[]> buttons;
    ComboButtonStage buttonStage;
    ComboConcurrentButtonsProgressEventHandler progressEventHandler;

    void initButtons(std::initializer_list<Button> buttonList);
    bool isTargetButton(size_t buttonIndex);
    bool isNonTargetButtonsPressed(const _StateButtons& buttonStates);
    void checkAllButtonsPressed(const _StateButtons& buttonStates);
    void checkAllButtonsReleased(const _StateButtons& buttonStates);
    void onProgress();
    void loopAction(State& state) override;
    void resetAction() override;
};

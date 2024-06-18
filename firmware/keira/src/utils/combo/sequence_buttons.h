#pragma once

#include "core/core.h"
#include "core/stage_time.h"

using ComboButtonSequenceProgressEventHandler =
    void (*)(const size_t buttonIndex, const Button button, const ButtonStage stage);

class ComboButtonSequence : public ComboBase {
public:
    ComboButtonSequence(std::initializer_list<Button> buttonList);
    ~ComboButtonSequence();

    void setProgressEventHandler(ComboButtonSequenceProgressEventHandler handler) {
        progressEventHandler = handler;
    }
    void setTimeout(uint16_t time);

private:
    const size_t NUM_BUTTONS;
    std::unique_ptr<Button[]> buttons;
    size_t buttonIndex;
    bool firstComboButtonAlreadyPressed;
    uint16_t timeout;
    ComboButtonWithTimeStage buttonStage;
    ComboButtonSequenceProgressEventHandler progressEventHandler;

    void initButtons(std::initializer_list<Button> buttonList);
    void selectNextButtonForProcessing();
    bool areAllButtonsProcessed();
    bool isLastButtonProcessed();
    bool isTimeout(uint32_t time);
    void onProgress();
    void loopAction(State& state) override;
    void resetAction() override;
};
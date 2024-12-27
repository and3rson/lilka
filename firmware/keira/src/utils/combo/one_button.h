#pragma once

#include "core/core.h"
#include "core/stage.h"

using ComboOneButtonProgressEventHandler = void (*)(const Button button, const ButtonStage stage);

class ComboOneButton : public ComboBase {
public:
    explicit ComboOneButton(Button button);
    ~ComboOneButton();

    void setProgressEventHandler(ComboOneButtonProgressEventHandler handler) {
        progressEventHandler = handler;
    }

private:
    ComboButtonStage buttonStage;
    ComboOneButtonProgressEventHandler progressEventHandler;
    void onProgress();
    void loopAction(State& state) override;
    void resetAction() override;
};
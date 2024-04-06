#pragma once

#include "app.h"
#include "sequencer.h"

class LilTrackerApp : public App {
public:
    LilTrackerApp();

private:
    Mixer mixer;
    Sequencer sequencer;
    void run() override;
    int printText(
        const char* text, int16_t x, int16_t y, lilka::Alignment hAlign, lilka::Alignment vAlign, bool editing,
        bool focused, bool dimmed
    );
};

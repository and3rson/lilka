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
    void printText(int16_t y, int16_t itemHeight, const char* text, bool editing, bool focused, bool dimmed);
};

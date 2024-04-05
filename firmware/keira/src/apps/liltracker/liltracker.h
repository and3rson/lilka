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
};

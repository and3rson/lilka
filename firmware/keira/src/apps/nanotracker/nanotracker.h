#pragma once

#include "app.h"
#include "sequencer.h"

class NanoTrackerApp : public App {
public:
    NanoTrackerApp();

private:
    Sequencer sequencer;
    void run() override;
};

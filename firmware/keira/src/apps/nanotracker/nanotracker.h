#pragma once

#include "app.h"
#include "mixer.h"

class NanoTrackerApp : public App {
public:
    NanoTrackerApp();

private:
    Mixer mixer;
    void run() override;
};

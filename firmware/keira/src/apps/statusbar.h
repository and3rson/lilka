#pragma once

#include "app.h"

class StatusBarApp : public App {
public:
    StatusBarApp();

private:
    void run() override;
    int16_t drawIcons(lilka::Canvas* canvas);
};

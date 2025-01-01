#pragma once

#include "app.h"

class CallBackTestApp : public App {
public:
    CallBackTestApp();

private:
    void CallbackExample();
    bool exiting = false;
    lilka::Menu menu;
    void run() override;
};

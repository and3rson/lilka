#pragma once

#include "app.h"

class CallBackTestApp : public App {
public:
    CallBackTestApp();
    void CallbackExample();

private:
    bool exiting = false;
    lilka::Menu menu;
    void run() override;
};

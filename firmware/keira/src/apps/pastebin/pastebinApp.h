#pragma once

#include <app.h>

class pastebinApp : public App {
public:
    pastebinApp();

private:
    void run() override;
    void uiLoop();
};
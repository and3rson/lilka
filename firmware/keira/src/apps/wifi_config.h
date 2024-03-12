#pragma once

#include "app.h"

class WiFiConfigApp : public App {
public:
    WiFiConfigApp();

private:
    void run() override;
};

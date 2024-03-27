#pragma once

#include "service.h"

class ScreenshotService : public Service {
public:
    ScreenshotService();

private:
    void run() override;
};

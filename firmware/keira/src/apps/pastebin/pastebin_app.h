#pragma once

#include <app.h>

namespace pastebin_app {

class MainApp : public App {
public:
    MainApp();
    ~MainApp() override;

private:
    void run() override;
    void uiLoop();
};

} // namespace pastebin_app
#pragma once
///////////////////////////////////////////////////////////////////////////////////////////////////////
// [^_^]==\~ Text Viewer  for Keira OS header file                                                   //
///////////////////////////////////////////////////////////////////////////////////////////////////////

#include "app.h"

class TextViewerApp : public App {
public:
    TextViewerApp(String path);

private:
    uint32_t offset = 0; // points to first character in file
    void run() override; // main loop

    // Settings
    String path;
    void readSettingsFromNVS();
    void saveSettingsToNVS();
};

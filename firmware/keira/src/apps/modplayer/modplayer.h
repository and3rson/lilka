#pragma once

#include "app.h"
#include <lilka.h>
#include "AudioGeneratorMOD.h"
#include "AudioOutputI2S.h"
#include "AudioFileSourceSD.h"

class ModPlayerApp : public App {
public:
    explicit ModPlayerApp(String path);
    void run() override;

private:
    void mainWindow(String fileName, float gain, bool isFinished);
    String path;
    AudioGeneratorMOD *mod;
    AudioFileSourceSD *modSource;
    AudioOutputI2S *out;
};

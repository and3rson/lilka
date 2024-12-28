#include <lilka.h>
#include "app.h"
#define VOLUME_STEP         5
#define VOLUME_BUTTON_DELAY 200

class SoundConfigApp : public App {
public:
    SoundConfigApp();

private:
    void saveSettings();
    int volumeLevel = lilka::audio.getVolume();
    bool startupSound = lilka::audio.getStartupSoundEnabled();
    bool startupBuzzer = lilka::buzzer.getStartupBuzzerEnabled();
    void run() override;
};
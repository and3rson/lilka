#include <lilka.h>
#include "app.h"
#define VOLUME_STEP          5
#define VOLUME_BUTTON_DELAY  150
#define VOLUME_MIN_MAX_DELAY 800

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
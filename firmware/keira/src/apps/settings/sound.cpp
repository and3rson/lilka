#include "sound.h"

SoundConfigApp::SoundConfigApp() : App("SoundConfig") {
}
void SoundConfigApp::saveSettings() {
    lilka::audio.setVolume(volumeLevel);
    lilka::audio.setStartupSoundEnabled(startupSound);
    lilka::buzzer.setStartupBuzzerEnabled(startupBuzzer);
    lilka::serial.log(
        "Saving sound settings: volume %d, startup %d, buzzer %d", volumeLevel, startupSound, startupBuzzer
    );
}

void SoundConfigApp::run() {
    lilka::Menu soundMenu("Звук");

    lilka::MenuItem mItems[] = {
        {"Гучність:", NULL, lilka::colors::White, String("< ") + String(volumeLevel) + " >"},
        {"Звук вітання:", NULL, lilka::colors::White, (startupSound) ? "ON" : "OFF"},
        {"Вітання бузером:", NULL, lilka::colors::White, (startupBuzzer) ? "ON" : "OFF"},
        {"<< Зберегти", NULL, lilka::colors::White, ""}
    };

    for (auto i = 0; i < sizeof(mItems) / sizeof(mItems[0]); i++)
        soundMenu.addItem(mItems[i].title, mItems[i].icon, mItems[i].color, mItems[i].postfix);

    soundMenu.addActivationButton(lilka::Button::B); // Back
    soundMenu.removeActivationButton(lilka::Button::A); // Remove default activation button
    auto lastVolumeChange = millis();
    auto volumeDelay = VOLUME_BUTTON_DELAY; // inital value
    while (!soundMenu.isFinished()) {
        soundMenu.update();
        soundMenu.draw(canvas);
        queueDraw();
        auto index = soundMenu.getCursor();
        switch (index) {
            case 0: { // Volume
                auto state = lilka::controller.peekState();
                auto tmpTime = millis();
                if (tmpTime - lastVolumeChange > volumeDelay) {
                    if (state.a.pressed) {
                        lilka::controller.resetState();
                        volumeLevel += VOLUME_STEP;
                        lastVolumeChange = tmpTime;
                    } else if (state.d.pressed) {
                        lilka::controller.resetState();
                        volumeLevel -= VOLUME_STEP;
                        lastVolumeChange = tmpTime;
                    }
                }
                volumeLevel = volumeLevel > 100 ? 0 : volumeLevel < 0 ? 100 : volumeLevel;
                volumeDelay = (volumeLevel == 100 || volumeLevel == 0) ? VOLUME_MIN_MAX_DELAY : VOLUME_BUTTON_DELAY;

                mItems[index].postfix = String("< ") + String(volumeLevel) + " >";
                soundMenu.setItem(
                    index, mItems[index].title, mItems[index].icon, mItems[index].color, mItems[index].postfix
                );
                break;
            }
            case 1: { // startupSound
                auto state = lilka::controller.peekState();

                if (state.a.justPressed) {
                    lilka::controller.resetState();
                    startupSound = !startupSound;
                    mItems[index].postfix = (startupSound) ? "ON" : "OFF";
                    soundMenu.setItem(
                        index, mItems[index].title, mItems[index].icon, mItems[index].color, mItems[index].postfix
                    );
                }
                break;
            }
            case 2: { //startupBuzzer
                auto state = lilka::controller.peekState();
                if (state.a.justPressed) {
                    lilka::controller.resetState();
                    startupBuzzer = !startupBuzzer;
                    mItems[index].postfix = (startupBuzzer) ? "ON" : "OFF";
                    soundMenu.setItem(
                        index, mItems[index].title, mItems[index].icon, mItems[index].color, mItems[index].postfix
                    );
                }
                break;
            }
            default: { //exit
                auto state = lilka::controller.peekState();
                if (state.a.justPressed) {
                    lilka::controller.resetState();
                    saveSettings();
                    return;
                }
            }
        }
    }
    saveSettings();
}

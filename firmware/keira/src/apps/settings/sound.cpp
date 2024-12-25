#include "sound.h"

SoundConfigApp::SoundConfigApp() : App("SoundConfig") {
}

void SoundConfigApp::run() {
    int volumeLevel = lilka::audio.getVolume();
    bool startupSound = lilka::audio.getStartupSoundEnabled();
    bool startupBuzzer = lilka::buzzer.getStartupBuzzerEnabled();

    lilka::Menu menu("Звук");
    menu.addActivationButton(lilka::Button::B);
    menu.addItem("Гучність:", 0, 0U, (String)volumeLevel);
    menu.addItem("Звук вітання:", 0, 0U, (startupSound) ? "ON" : "OFF");
    menu.addItem("Вітання бузером:", 0, 0U, (startupBuzzer) ? "ON" : "OFF");
    menu.addItem("<< Зберегти");

    while (true) {
        while (!menu.isFinished()) {
            menu.update();
            menu.draw(canvas);
            queueDraw();
        }

        if (menu.getButton() == lilka::Button::B) {
            break;
        }
        int16_t index = menu.getCursor();
        if (index == 0) {
            if (volumeLevel < 100) {
                volumeLevel += 10;
            } else {
                volumeLevel = 0;
            }

            lilka::MenuItem volumeItem;
            menu.getItem(0, &volumeItem);
            volumeItem.postfix = volumeLevel;
            menu.setItem(0, volumeItem.title, volumeItem.icon, volumeItem.color, volumeItem.postfix);
        } else if (index == 1) {
            startupSound = !startupSound;

            lilka::MenuItem startupItem;
            menu.getItem(1, &startupItem);
            startupItem.postfix = (startupSound) ? "ON" : "OFF";
            menu.setItem(1, startupItem.title, startupItem.icon, startupItem.color, startupItem.postfix);
        } else if (index == 2) {
            startupBuzzer = !startupBuzzer;

            lilka::MenuItem buzzerItem;
            menu.getItem(2, &buzzerItem);
            buzzerItem.postfix = (startupBuzzer) ? "ON" : "OFF";
            menu.setItem(2, buzzerItem.title, buzzerItem.icon, buzzerItem.color, buzzerItem.postfix);
        } else if (index == 3) {
            break;
        }
    }

    lilka::serial_log(
        "Saving sound settings: volume %d, startup %d, buzzer %d", volumeLevel, startupSound, startupBuzzer
    );

    lilka::audio.setVolume(volumeLevel);
    lilka::audio.setStartupSoundEnabled(startupSound);
    lilka::buzzer.setStartupBuzzerEnabled(startupBuzzer);
}

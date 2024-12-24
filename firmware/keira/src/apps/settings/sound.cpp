#include "sound.h"
#include "Preferences.h"

SonudConfigApp::SonudConfigApp() : App("VolumeConfig") {
}

void SonudConfigApp::run() {
    Preferences prefs;
    prefs.begin("sound", false);
    uint32_t volumeLevel = prefs.getUInt("volumeLevel", 50);
    bool startupSound = prefs.getBool("startupSound", true);
    bool startupBuzzer = prefs.getBool("startupBuzzer", true);
    // prefs.end();

    lilka::Menu menu("Звук");
    menu.addActivationButton(lilka::Button::B);
    menu.addItem("Гучність:");
    menu.addItem("Звук вітання:");
    menu.addItem("Вітання бузером:");
    menu.addItem("<< Зберегти");

    while (true) {
        while (!menu.isFinished()) {
            lilka::MenuItem volumeItem;
            menu.getItem(0, &volumeItem);
            volumeItem.postfix = volumeLevel;
            menu.setItem(0, volumeItem.title, volumeItem.icon, volumeItem.color, volumeItem.postfix);

            lilka::MenuItem startupItem;
            menu.getItem(1, &startupItem);
            startupItem.postfix = (startupSound) ? "ON" : "OFF";
            menu.setItem(1, startupItem.title, startupItem.icon, startupItem.color, startupItem.postfix);

            lilka::MenuItem buzzerItem;
            menu.getItem(2, &buzzerItem);
            buzzerItem.postfix = (startupBuzzer) ? "ON" : "OFF";
            menu.setItem(2, buzzerItem.title, buzzerItem.icon, buzzerItem.color, buzzerItem.postfix);

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
        } else if (index == 1) {
            startupSound = !startupSound;
        } else if (index == 2) {
            startupBuzzer = !startupBuzzer;
        } else if (index == 3) {
            break;
        }

        lilka::MenuItem item;
        menu.getItem(index, &item);
    }

    lilka::serial_log(
        "Saving sound settings: volume %d, startup %d, buzzer %d", volumeLevel, startupSound, startupBuzzer
    );

    // prefs.begin("sound", false);
    prefs.putUInt("volumeLevel", volumeLevel);
    prefs.putBool("startupSound", startupSound);
    prefs.putBool("startupBuzzer", startupBuzzer);
    prefs.end();

    lilka::audio.updateSettings();

    return;
}

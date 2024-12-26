#include "sound.h"

SoundConfigApp::SoundConfigApp() : App("SoundConfig") {
}

void SoundConfigApp::run() {
    int volumeLevel = lilka::audio.getVolume();
    bool startupSound = lilka::audio.getStartupSoundEnabled();
    bool startupBuzzer = lilka::buzzer.getStartupBuzzerEnabled();

    lilka::Menu menu("Звук");
    menu.addActivationButton(lilka::Button::B);
    menu.addActivationButton(lilka::Button::D);
    menu.addItem("Гучність:", 0, 0U, "< " + (String)volumeLevel + " >");
    menu.addItem("Звук вітання:", 0, 0U, (startupSound) ? "ON" : "OFF");
    menu.addItem("Вітання бузером:", 0, 0U, (startupBuzzer) ? "ON" : "OFF");
    menu.addItem("<< Назад");

    while (true) {
        while (!menu.isFinished()) {
            menu.update();
            menu.draw(canvas);
            queueDraw();
            lilka::controller.setAutoRepeat(lilka::Button::A, 0, 0);
            lilka::controller.setAutoRepeat(lilka::Button::D, 0, 0);
        }

        if (menu.getButton() == lilka::Button::B) {
            break;
        }
        int16_t index = menu.getCursor();
        if (index == 0) {
            int increment = 5;
            if (menu.getButton() == lilka::Button::D) increment = -increment;

            volumeLevel += increment;
            if (volumeLevel > 100) volumeLevel = 0;
            if (volumeLevel < 0) volumeLevel = 100;

            lilka::MenuItem volumeItem;
            menu.getItem(0, &volumeItem);
            volumeItem.postfix = "< " + (String)volumeLevel + " >";
            menu.setItem(0, volumeItem.title, volumeItem.icon, volumeItem.color, volumeItem.postfix);

            delay(100); // AutoRepeat чомусь не працює правильно в меню
            lilka::controller.setAutoRepeat(lilka::Button::A, 100, 500);
            lilka::controller.setAutoRepeat(lilka::Button::D, 100, 500);
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

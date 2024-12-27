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
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }

        int16_t index = menu.getCursor();

        if (index == 3 || menu.getButton() == lilka::Button::B) {
            break;
        } else if (index == 0) {
            bool justPressed = true;

            lilka::State state = lilka::controller.getState();

            while (state.a.pressed || state.d.pressed) {
                unsigned long repeatStart = millis();

                volumeLevel = (menu.getButton() == lilka::Button::D) ? volumeLevel - VOLUME_INCREMENT
                                                                     : volumeLevel + VOLUME_INCREMENT;
                if (volumeLevel > 100) volumeLevel = 0;
                if (volumeLevel < 0) volumeLevel = 100;

                lilka::MenuItem volumeItem;
                menu.getItem(0, &volumeItem);
                volumeItem.postfix = "< " + (String)volumeLevel + " >";
                menu.setItem(0, volumeItem.title, volumeItem.icon, volumeItem.color, volumeItem.postfix);

                int repeatInterval = REPEAT_INTERVAL;

                if (justPressed || volumeLevel == 0 || volumeLevel == 100) {
                    repeatInterval = 5 * REPEAT_INTERVAL;
                    justPressed = false;
                }

                while ((state.a.pressed || state.d.pressed) && millis() - repeatStart < repeatInterval) {
                    menu.update();
                    menu.draw(canvas);
                    queueDraw();
                    vTaskDelay(10 / portTICK_PERIOD_MS);
                    state = lilka::controller.getState();
                }
            }

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
        }
    }

    lilka::serial_log(
        "Saving sound settings: volumeLevel=%d, startupSound=%d, startupBuzzer=%d",
        volumeLevel,
        startupSound,
        startupBuzzer
    );

    lilka::audio.setVolume(volumeLevel);
    lilka::audio.setStartupSoundEnabled(startupSound);
    lilka::buzzer.setStartupBuzzerEnabled(startupBuzzer);
}

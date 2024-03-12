#include "keyboard.h"

KeyboardApp::KeyboardApp() : App("Keyboard") {
}

void KeyboardApp::run() {
    lilka::InputDialog dialog("Введіть текст:");

    while (true) {
        dialog.update();
        dialog.draw(canvas);
        queueDraw();
        if (dialog.isDone()) {
            break;
        }
    }

    lilka::Alert alert("Ви ввели:", dialog.getValue());
    alert.draw(canvas);
    queueDraw();
    while (true) {
        alert.update();
        if (alert.isDone()) {
            break;
        }
    }
}

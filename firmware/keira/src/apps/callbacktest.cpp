#include "callbacktest.h"

CallBackTestApp::CallBackTestApp() : App("CallBackTest") {
}
void CallBackTestApp::CallbackExample() {
    lilka::Alert alert("Callback", "");
    const String buttonNames[] = {"UP", "DOWN", "LEFT", "RIGHT", "A", "B", "C", "D", "SELECT", "START", "ANY"};

    auto button = menu.getButton();

    if (button == lilka::Button::B) {
        exiting = true;
        return;
    }
    // skip
    if (button == lilka::Button::ANY) return;
    // allow to exit alert by B
    alert.addActivationButton(lilka::Button::B);
    alert.setMessage(String("Натиснуто кнопку ") + buttonNames[menu.getButton()]);
    while (!alert.isFinished()) {
        alert.update();
        alert.draw(canvas);
        queueDraw();
    }
}
void CallBackTestApp::run() {
    menu.setTitle("CallbackTest");
    menu.addItem(
        "Натисніть будь яку кнопку",
        0,
        0U,
        "",
        reinterpret_cast<lilka::PMenuItemCallback>(&CallBackTestApp::CallbackExample),
        reinterpret_cast<void*>(this)
    );
    // Add All buttons
    for (int i = 0; i < lilka::Button::COUNT; i++) {
        menu.addActivationButton(static_cast<lilka::Button>(i));
    }
    while (!exiting) {
        while (!menu.isFinished()) {
            menu.update();
            menu.draw(canvas);
            queueDraw();
        }
    }
}

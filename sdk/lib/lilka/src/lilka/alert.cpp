#include "ui.h"

namespace lilka {

Alert::Alert(String title, String message) {
    this->title = title;
    this->message = message;
    this->done = false;
    this->button = Button::COUNT;
    activationButtons.push_back(Button::A);
}

void Alert::setTitle(String title) {
    this->title = title;
}

void Alert::setMessage(String message) {
    this->message = message;
}

void Alert::update() {
    State state = controller.getState();
    lilka::_StateButtons& buttonsArray = *reinterpret_cast<lilka::_StateButtons*>(&state);
    std::vector<Button>::iterator it =
        std::find_if(activationButtons.begin(), activationButtons.end(), [&buttonsArray](Button activationButton) {
            return buttonsArray[activationButton].justPressed;
        });
    if (it != activationButtons.end()) {
        button = *it;
        done = true;
    }
    vTaskDelay(LILKA_UI_UPDATE_DELAY_MS / portTICK_PERIOD_MS);
}

void Alert::draw(Arduino_GFX* canvas) {
    int top = canvas->height() / 8;
    int mid = canvas->height() / 8 * 2;
    int bottom = canvas->height() / 8 * 7;

    int left = canvas->width() / 8;
    int right = canvas->width() / 8 * 7;
    int width = right - left;
    int xMargin = 4;

    canvas->setTextColor(lilka::colors::White);

    canvas->fillRect(left, top, width, mid - top, lilka::colors::Midnight_blue);
    canvas->setFont(FONT_6x13);
    canvas->setTextSize(2);
    canvas->setTextBound(left + xMargin, top, width - xMargin * 2, mid - top);
    canvas->setCursor(left + xMargin, top + 13 * 2);
    canvas->println(title);

    canvas->fillRect(left, mid, width, bottom - mid, canvas->color565(32, 96, 96));
    canvas->setFont(FONT_9x15);
    canvas->setTextSize(1);
    canvas->setTextBound(left + xMargin, mid, width - xMargin * 2, bottom - mid);
    canvas->setCursor(left + xMargin, mid + 20);
    canvas->println(message);
}

bool Alert::isFinished() {
    if (done) {
        done = false;
        return true;
    }
    return false;
}

void Alert::addActivationButton(Button activationButton) {
    activationButtons.push_back(activationButton);
}

Button Alert::getButton() {
    return button;
}

} // namespace lilka

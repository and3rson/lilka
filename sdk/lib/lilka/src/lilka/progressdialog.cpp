#include "ui.h"

#include "lilka/display.h"
#include "lilka/controller.h"

#include "icons/shift.h"
#include "icons/shifted.h"
#include "icons/backspace.h"
#include "icons/whitespace.h"

namespace lilka {

ProgressDialog::ProgressDialog(String title, String message) {
    this->title = title;
    this->message = message;
    this->progress = 0;
}

void ProgressDialog::setProgress(int16_t progress) {
    this->progress = progress;
}

void ProgressDialog::setMessage(String message) {
    this->message = message;
}

void ProgressDialog::draw(Arduino_GFX* canvas) {
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
    canvas->setTextBound(left + xMargin, top, width - xMargin * 2, bottom - mid);
    canvas->setCursor(left + xMargin, mid + 20);
    canvas->println(message);

    char buf[8];
    int16_t x, y;
    uint16_t w, h;

    sprintf(buf, "%d%%", progress);
    canvas->getTextBounds(buf, 0, 0, &x, &y, &w, &h);
    // canvas->fillRect(x, y, w, h, lilka::colors::Black);
    // canvas->println(buf);

    int barMargin = 8;
    int barHeight = 8;

    int center = (left + right) / 2;

    canvas->fillRect(
        left + barMargin, bottom - barMargin - barHeight, width - barMargin * 2, barHeight, lilka::colors::Persian_plum
    );
    canvas->fillRect(
        left + barMargin,
        bottom - barMargin - barHeight,
        (width - barMargin * 2) * progress / 100,
        barHeight,
        lilka::colors::Dark_orange
    );
    canvas->setCursor(center - w / 2, bottom - barMargin - barHeight - barMargin);
    canvas->setTextBound(0, 0, canvas->width(), canvas->height());
    canvas->print(buf);
}

} // namespace lilka

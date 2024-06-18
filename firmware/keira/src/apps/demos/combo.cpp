#include "combo.h"
#include "utils/combo/core/aggregator.h"
#include "utils/combo/one_button.h"
#include "utils/combo/one_hold_down_button.h"
#include "utils/combo/sequence_buttons.h"
#include "utils/combo/concurrent_buttons.h"

#define SECONDS_FOR_EXIT 3

using lilka::Alignment;
using lilka::Button;

uint8_t leftSecondsForExit = SECONDS_FOR_EXIT;
bool exitApp = false;
uint8_t currentScene = 0;

void exit_process_handler(const Button button, const ButtonStage stage, uint16_t leftTimeToComplete) {
    leftSecondsForExit = leftTimeToComplete / 1000;
}

void exit_reset_handler() {
    leftSecondsForExit = SECONDS_FOR_EXIT;
}

void exit_completed_handler() {
    exitApp = true;
}

void completed_handler() {
    currentScene++;
}

ComboApp::ComboApp() : App("Combo") {
    leftSecondsForExit = SECONDS_FOR_EXIT;
    exitApp = false;
    currentScene = 0;
}

void ComboApp::run() {
    ComboAggregator comboAggregator;
    lilka::Canvas buffer(canvas->width(), canvas->height());

    ComboOneHoldDownButton comboExit(Button::SELECT, SECONDS_FOR_EXIT * 1000);
    comboExit.setAutoReset(false);
    comboExit.setCompletedEventHandler(exit_completed_handler);
    comboExit.setResetEventHandler(exit_reset_handler);
    comboExit.setProgressEventHandler(exit_process_handler);
    comboAggregator.add(&comboExit);

    ComboOneButton comboContinue(Button::START);
    comboContinue.setAutoStart(false);
    comboContinue.setAutoReset(false);
    comboContinue.setCompletedEventHandler(completed_handler);
    comboAggregator.add(&comboContinue);

    ComboOneButton comboScene1PressA(Button::A);
    comboScene1PressA.setAutoStart(false);
    comboScene1PressA.setAutoReset(false);
    comboScene1PressA.setCompletedEventHandler(completed_handler);
    comboAggregator.add(&comboScene1PressA);

    ComboOneHoldDownButton comboScene2HoldAny(Button::ANY, 1000);
    comboScene2HoldAny.setAutoStart(false);
    comboScene2HoldAny.setAutoReset(false);
    comboScene2HoldAny.setCompletedEventHandler(completed_handler);
    comboAggregator.add(&comboScene2HoldAny);

    ComboConcurrentButtons comboScene3UpB({Button::UP, Button::B});
    comboScene3UpB.setAutoStart(false);
    comboScene3UpB.setAutoReset(false);
    comboScene3UpB.setCompletedEventHandler(completed_handler);
    comboAggregator.add(&comboScene3UpB);

    ComboButtonSequence comboSequenceLast(
        {Button::DOWN, Button::UP, Button::UP, Button::DOWN, Button::A, Button::B, Button::B, Button::A}
    );
    comboSequenceLast.setAutoStart(false);
    comboSequenceLast.setAutoReset(false);
    comboSequenceLast.setCompletedEventHandler(completed_handler);
    comboAggregator.add(&comboSequenceLast);

    while (!exitApp) {
        lilka::State st = lilka::controller.getState();
        comboAggregator.loop(st);

        buffer.fillScreen(lilka::colors::Black);
        mainUI(buffer, st);

        switch (currentScene) {
            case 0:
                comboContinue.start();
                scene_0(buffer, st);
                break;
            case 1:
                comboScene1PressA.start();
                scene_1(buffer, st);
                break;
            case 2:
                comboScene2HoldAny.start();
                scene_2(buffer, st);
                break;
            case 3:
                comboScene3UpB.start();
                scene_3(buffer, st);
                break;
            case 4:
                comboSequenceLast.setTimeout(750);
                comboSequenceLast.start();
                scene_4(buffer, st, 750);
                break;
            case 5:
                comboSequenceLast.setTimeout(500);
                if (comboSequenceLast.isCompleted()) {
                    comboSequenceLast.reset();
                }
                comboSequenceLast.start();
                scene_4(buffer, st, 500);
                break;
            case 6:
                comboSequenceLast.setTimeout(250);
                if (comboSequenceLast.isCompleted()) {
                    comboSequenceLast.reset();
                }
                comboSequenceLast.start();
                scene_4(buffer, st, 250);
                break;
            case 7:
                scene_final(buffer, st);
                break;
            default:
                break;
        }
        canvas->drawCanvas(&buffer);
        queueDraw();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

void ComboApp::mainUI(lilka::Canvas& buffer, lilka::State& st) {
    uint16_t hor_center = canvas->width() / 2;
    lilka::display.setFont(FONT_10x20_MONO);
    buffer.drawTextAligned("Demo Combo", hor_center, 20, Alignment::ALIGN_CENTER, Alignment::ALIGN_START);
    lilka::display.setFont(FONT_8x13_MONO);
    buffer.drawTextAligned(
        "Для виходу затисніть",
        hor_center,
        canvas->height() - 40,
        lilka::Alignment::ALIGN_CENTER,
        lilka::Alignment::ALIGN_START
    );

    const char* pressByExitMessage = "[SELECT] на %d сек";
    char pressByExit[strlen(pressByExitMessage)];
    sprintf(pressByExit, pressByExitMessage, leftSecondsForExit);
    buffer.drawTextAligned(
        pressByExit, hor_center, canvas->height() - 20, Alignment::ALIGN_CENTER, Alignment::ALIGN_START
    );
}

void ComboApp::scene_0(lilka::Canvas& buffer, lilka::State& st) {
    uint16_t hor_center = canvas->width() / 2;
    lilka::display.setFont(FONT_8x13_MONO);
    buffer.drawTextAligned("Натисніть [START]", hor_center, 90, Alignment::ALIGN_CENTER, Alignment::ALIGN_START);

    buffer.drawTextAligned("для продовження", hor_center, 110, Alignment::ALIGN_CENTER, Alignment::ALIGN_START);
}

void ComboApp::scene_1(lilka::Canvas& buffer, lilka::State& st) {
    uint16_t hor_center = canvas->width() / 2;
    lilka::display.setFont(FONT_8x13_MONO);
    buffer.drawTextAligned("Натисніть", hor_center, 90, Alignment::ALIGN_CENTER, Alignment::ALIGN_START);

    buffer.drawTextAligned("[A]", hor_center, 110, Alignment::ALIGN_CENTER, Alignment::ALIGN_START);
}

void ComboApp::scene_2(lilka::Canvas& buffer, lilka::State& st) {
    uint16_t hor_center = canvas->width() / 2;
    lilka::display.setFont(FONT_8x13_MONO);
    buffer.drawTextAligned("Затисніть будь-яку", hor_center, 90, Alignment::ALIGN_CENTER, Alignment::ALIGN_START);

    buffer.drawTextAligned("кнопку на 1 сек", hor_center, 110, Alignment::ALIGN_CENTER, Alignment::ALIGN_START);
}

void ComboApp::scene_3(lilka::Canvas& buffer, lilka::State& st) {
    uint16_t hor_center = canvas->width() / 2;
    lilka::display.setFont(FONT_8x13_MONO);
    buffer.drawTextAligned("Натисніть разом", hor_center, 90, Alignment::ALIGN_CENTER, Alignment::ALIGN_START);

    buffer.drawTextAligned("[UP] + [B]", hor_center, 110, Alignment::ALIGN_CENTER, Alignment::ALIGN_START);
}

void ComboApp::scene_4(lilka::Canvas& buffer, lilka::State& st, int timeout) {
    uint16_t hor_center = canvas->width() / 2;
    lilka::display.setFont(FONT_8x13_MONO);
    buffer.drawTextAligned("DOWN,UP,UP,DOWN,A,B,B,A", hor_center, 90, Alignment::ALIGN_CENTER, Alignment::ALIGN_START);

    const char* txt = "Таймаут %d мс";
    char bf[100];
    sprintf(bf, txt, timeout);
    buffer.drawTextAligned(bf, hor_center, 110, Alignment::ALIGN_CENTER, Alignment::ALIGN_START);
}

void ComboApp::scene_final(lilka::Canvas& buffer, lilka::State& st) {
    uint16_t hor_center = canvas->width() / 2;
    lilka::display.setFont(FONT_8x13_MONO);
    buffer.drawTextAligned("Кінець!", hor_center, 90, Alignment::ALIGN_CENTER, Alignment::ALIGN_START);
}

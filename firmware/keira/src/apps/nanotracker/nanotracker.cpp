#include <lilka.h>

#include "nanotracker.h"
#include "freq.h"
#include "sequencer.h"

constexpr int32_t scoreTop = 0;
const int32_t scoreBottom = lilka::display.height();

NanoTrackerApp::NanoTrackerApp() : App("NanoTracker", 0, 0, lilka::display.width(), lilka::display.height()) {
    this->setFlags(APP_FLAG_FULLSCREEN);
}

xSemaphoreHandle xMutex;

void NanoTrackerApp::run() {
    event_t channel0[] = {
        {lilka::NOTE_E4},
        {0},
        {lilka::NOTE_E4},
        {0},
        {0},
        {0},
        {0},
        {lilka::NOTE_E4},
        {0},
        {lilka::NOTE_E4},
        {0},
        {lilka::NOTE_D4},
        {lilka::NOTE_E4},
        {0},
        {lilka::NOTE_D4},
        {0},
        {lilka::NOTE_C4},
        {0},
        {lilka::NOTE_C4},
        {0},
        {0},
        {0},
        {0},
        {lilka::NOTE_A3},
        {0},
        {lilka::NOTE_B3},
        {0},
        {lilka::NOTE_A3},
        {lilka::NOTE_C4},
        {0},
        {lilka::NOTE_B3},
        {0},
    };
    event_t channel1[] = {
        {lilka::NOTE_E2},
        {lilka::NOTE_G2},
        {lilka::NOTE_A2},
        {lilka::NOTE_B2},
        //
        {0}, // {lilka::NOTE_E2},
        {lilka::NOTE_G2},
        {lilka::NOTE_A2},
        {lilka::NOTE_B2},
        //
        {0}, // {lilka::NOTE_E2},
        {lilka::NOTE_G2},
        {lilka::NOTE_A2},
        {0}, // {lilka::NOTE_B2},
        //
        {lilka::NOTE_E2},
        {lilka::NOTE_G2},
        {0}, // {lilka::NOTE_A2},
        {lilka::NOTE_B2},
        //
        {lilka::NOTE_C2},
        {lilka::NOTE_E2},
        {lilka::NOTE_F2},
        {lilka::NOTE_G2},
        //
        {0}, // {lilka::NOTE_C2},
        {lilka::NOTE_E2},
        {lilka::NOTE_F2},
        {lilka::NOTE_G2},
        //
        {0}, // {lilka::NOTE_C2},
        {lilka::NOTE_E2},
        {lilka::NOTE_F2},
        {0}, // {lilka::NOTE_G2},
        //
        {lilka::NOTE_C2},
        {lilka::NOTE_E2},
        {0}, // {lilka::NOTE_F2},
        {lilka::NOTE_G2},
    };
    event_t channel2[] = {
        {lilka::NOTE_E6},
        {0},
        {0},
        {0},
        {lilka::NOTE_E6},
        {0},
        {lilka::NOTE_E6},
        {0},
        {0},
        {lilka::NOTE_E6},
        {0},
        {0},
        {lilka::NOTE_E6},
        {0},
        {0},
        {0},
        {lilka::NOTE_C6},
        {0},
        {0},
        {0},
        {lilka::NOTE_C6},
        {0},
        {lilka::NOTE_C6},
        {0},
        {0},
        {lilka::NOTE_C6},
        {0},
        {0},
        {lilka::NOTE_C6},
        {0},
        {0},
        {0},
    };

    sequencer.setChannelEvents(0, channel0);
    sequencer.setChannelEvents(1, channel1);
    sequencer.setChannelEvents(2, channel2);
    sequencer.start();

    while (1) {
        canvas->fillScreen(lilka::colors::Black);
        canvas->setFont(FONT_10x20);
        canvas->setTextBound(0, 0, canvas->width(), canvas->height());
        canvas->setTextColor(lilka::colors::White);
        constexpr int16_t itemHeight = 20;
        const int16_t centerY = canvas->height() / 2;
        int currentEventIndex = sequencer.getCurrentEventIndex();
        for (int eventIndex = 0; eventIndex < channelSize; eventIndex++) {
            // Draw score, with current event in the middle
            int16_t y = centerY + (eventIndex - currentEventIndex) * itemHeight;
            if (y < scoreTop || y > scoreBottom) {
                continue;
            }
            if (eventIndex == currentEventIndex) {
                canvas->fillRect(0, y, canvas->width(), itemHeight, lilka::colors::Blue);
            }
            canvas->setCursor(24, y + 18);
            canvas->printf("| %2d | ", eventIndex + 1);
            ;
            canvas->printf("%-3s | ", frequency_to_note(sequencer.getChannelEvent(0, eventIndex).pitch));
            canvas->printf("%-3s | ", frequency_to_note(sequencer.getChannelEvent(1, eventIndex).pitch));
            canvas->printf("%-3s | ", frequency_to_note(sequencer.getChannelEvent(2, eventIndex).pitch));
        }
        queueDraw();
    }
}

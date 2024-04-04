#include <lilka.h>

#include "nanotracker.h"
#include "note.h"

constexpr int32_t scoreTop = 0;
const int32_t scoreBottom = lilka::display.height();

NanoTrackerApp::NanoTrackerApp() : App("NanoTracker", 0, 0, lilka::display.width(), lilka::display.height()) {
    this->setFlags(APP_FLAG_FULLSCREEN);
}

xSemaphoreHandle xMutex;

void NanoTrackerApp::run() {
    event_t channel0[] = {
        {lilka::NOTE_E4, 1.0},
        {0, 1.0},
        {lilka::NOTE_E4, 1.0},
        {0, 1.0},
        {0, 1.0},
        {0, 1.0},
        {0, 1.0},
        {lilka::NOTE_E4, 1.0},
        {0, 1.0},
        {lilka::NOTE_E4, 1.0},
        {0, 1.0},
        {lilka::NOTE_D4, 1.0},
        {lilka::NOTE_E4, 1.0},
        {0, 1.0},
        {lilka::NOTE_D4, 1.0},
        {0, 1.0},
        {lilka::NOTE_C4, 1.0},
        {0, 1.0},
        {lilka::NOTE_C4, 1.0},
        {0, 1.0},
        {0, 1.0},
        {0, 1.0},
        {0, 1.0},
        {lilka::NOTE_A3, 1.0},
        {0, 1.0},
        {lilka::NOTE_B3, 1.0},
        {0, 1.0},
        {lilka::NOTE_A3, 1.0},
        {lilka::NOTE_C4, 1.0},
        {0, 1.0},
        {lilka::NOTE_B3, 1.0},
        {0, 1.0},
    };
    event_t channel1[] = {
        {lilka::NOTE_E2, 1.0},
        {lilka::NOTE_G2, 1.0},
        {lilka::NOTE_A2, 1.0},
        {lilka::NOTE_B2, 1.0},
        //
        {0, 1.0}, // {lilka::NOTE_E2, 1.0},
        {lilka::NOTE_G2, 1.0},
        {lilka::NOTE_A2, 1.0},
        {lilka::NOTE_B2, 1.0},
        //
        {0, 1.0}, // {lilka::NOTE_E2, 1.0},
        {lilka::NOTE_G2, 1.0},
        {lilka::NOTE_A2, 1.0},
        {0, 1.0}, // {lilka::NOTE_B2, 1.0},
        //
        {lilka::NOTE_E2, 1.0},
        {lilka::NOTE_G2, 1.0},
        {0, 1.0}, // {lilka::NOTE_A2, 1.0},
        {lilka::NOTE_B2, 1.0},
        //
        {lilka::NOTE_C2, 1.0},
        {lilka::NOTE_E2, 1.0},
        {lilka::NOTE_F2, 1.0},
        {lilka::NOTE_G2, 1.0},
        //
        {0, 1.0}, // {lilka::NOTE_C2, 1.0},
        {lilka::NOTE_E2, 1.0},
        {lilka::NOTE_F2, 1.0},
        {lilka::NOTE_G2, 1.0},
        //
        {0, 1.0}, // {lilka::NOTE_C2, 1.0},
        {lilka::NOTE_E2, 1.0},
        {lilka::NOTE_F2, 1.0},
        {0, 1.0}, // {lilka::NOTE_G2, 1.0},
        //
        {lilka::NOTE_C2, 1.0},
        {lilka::NOTE_E2, 1.0},
        {0, 1.0}, // {lilka::NOTE_F2, 1.0},
        {lilka::NOTE_G2, 1.0},
    };
    event_t channel2[] = {
        {lilka::NOTE_E6, 1.0},
        {0, 1.0},
        {0, 1.0},
        {0, 1.0},
        {lilka::NOTE_E6, 1.0},
        {0, 1.0},
        {lilka::NOTE_E6, 1.0},
        {0, 1.0},
        {0, 1.0},
        {lilka::NOTE_E6, 1.0},
        {0, 1.0},
        {0, 1.0},
        {lilka::NOTE_E6, 1.0},
        {0, 1.0},
        {0, 1.0},
        {0, 1.0},
        {lilka::NOTE_C6, 1.0},
        {0, 1.0},
        {0, 1.0},
        {0, 1.0},
        {lilka::NOTE_C6, 1.0},
        {0, 1.0},
        {lilka::NOTE_C6, 1.0},
        {0, 1.0},
        {0, 1.0},
        {lilka::NOTE_C6, 1.0},
        {0, 1.0},
        {0, 1.0},
        {lilka::NOTE_C6, 1.0},
        {0, 1.0},
        {0, 1.0},
        {0, 1.0},
    };

    Pattern pattern;

    pattern.setChannelEvents(0, channel0);
    pattern.setChannelWaveform(0, WAVEFORM_SQUARE);
    pattern.setChannelEvents(1, channel1);
    pattern.setChannelWaveform(1, WAVEFORM_SAWTOOTH);
    pattern.setChannelEvents(2, channel2);
    // pattern.setChannelWaveform(2, WAVEFORM_TRIANGLE);
    pattern.setChannelWaveform(2, WAVEFORM_SINE);

    int cursorX = 0;
    int cursorY = 0;

    while (1) {
        canvas->fillScreen(lilka::colors::Black);
        canvas->setFont(FONT_9x15);
        canvas->setTextBound(0, 0, canvas->width(), canvas->height());
        canvas->setTextColor(lilka::colors::White);
        constexpr int16_t itemHeight = 15;
        const int16_t centerY = canvas->height() / 2;
        for (int eventIndex = 0; eventIndex < CHANNEL_SIZE; eventIndex++) {
            // Draw score, with current event in the middle
            int16_t y = centerY + (eventIndex - cursorY) * itemHeight;
            if (y < scoreTop || y > scoreBottom) {
                continue;
            }
            if (eventIndex == cursorY) {
                canvas->fillRect(0, y, canvas->width(), itemHeight, lilka::colors::Indigo_dye);
            }
            canvas->setCursor(24, y + 13);
            canvas->printf("| %2d | ", eventIndex + 1);
            for (int channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
                noteinfo_t noteinfo;
                event_t event = pattern.getChannelEvent(channelIndex, eventIndex);
                char str[4];
                if (event.pitch == 0) {
                    strcpy(str, "---");
                } else if (event.pitch == 1) {
                    strcpy(str, "XXX");
                } else {
                    noteinfo = frequency_to_noteinfo(event.pitch);
                    canvas->setTextColor(
                        (cursorY == eventIndex && cursorX == channelIndex) ? lilka::colors::White
                                                                           : lilka::colors::Uranian_blue
                    );
                    strcpy(str, noteinfo.toStr());
                }
                canvas->printf("%3s", str);
                canvas->setTextColor(lilka::colors::White);
                canvas->printf(" | ");
            }
        }
        lilka::State state = lilka::controller.getState();
        if (state.up.justPressed) {
            cursorY = (cursorY - 1 + CHANNEL_SIZE) % CHANNEL_SIZE;
        } else if (state.down.justPressed) {
            cursorY = (cursorY + 1) % CHANNEL_SIZE;
        } else if (state.left.justPressed) {
            cursorX = (cursorX - 1 + CHANNEL_COUNT) % CHANNEL_COUNT;
        } else if (state.right.justPressed) {
            cursorX = (cursorX + 1) % CHANNEL_COUNT;
        }
        if (state.a.justPressed) {
            mixer.play(&pattern, cursorY);
        } else if (state.a.justReleased) {
            mixer.stop();
        }
        if (state.b.justPressed) {
            mixer.play(&pattern);
        }
        queueDraw();
    }
}

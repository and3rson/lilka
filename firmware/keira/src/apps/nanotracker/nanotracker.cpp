#include <lilka.h>

#include "nanotracker.h"
#include "note.h"

constexpr int32_t scoreTop = 0;
const int32_t scoreBottom = lilka::display.height();

NanoTrackerApp::NanoTrackerApp() :
    App("NanoTracker", 0, 0, lilka::display.width(), lilka::display.height()), mixer(), sequencer(&mixer) {
    this->setFlags(APP_FLAG_FULLSCREEN);
}

xSemaphoreHandle xMutex;

void NanoTrackerApp::run() {
    event_t channel0[] = {
        {lilka::NOTE_E4, 1.0, EVENT_TYPE_NORMAL},
        {0, 1.0, EVENT_TYPE_STOP},
        {lilka::NOTE_E4, 1.0, EVENT_TYPE_NORMAL},
        {0, 1.0, EVENT_TYPE_STOP},
        //
        {0, 1.0, EVENT_TYPE_STOP},
        {0, 1.0, EVENT_TYPE_STOP},
        {0, 1.0, EVENT_TYPE_STOP},
        {lilka::NOTE_E4, 1.0, EVENT_TYPE_NORMAL},
        //
        {0, 1.0, EVENT_TYPE_STOP},
        {lilka::NOTE_E4, 1.0, EVENT_TYPE_NORMAL},
        {0, 1.0, EVENT_TYPE_STOP},
        {lilka::NOTE_D4, 1.0, EVENT_TYPE_NORMAL},
        //
        {lilka::NOTE_E4, 1.0, EVENT_TYPE_NORMAL},
        {0, 1.0, EVENT_TYPE_STOP},
        {lilka::NOTE_D4, 1.0, EVENT_TYPE_NORMAL},
        {0, 1.0, EVENT_TYPE_STOP},
        //
        {lilka::NOTE_C4, 1.0, EVENT_TYPE_NORMAL},
        {0, 1.0, EVENT_TYPE_STOP},
        {lilka::NOTE_C4, 1.0, EVENT_TYPE_NORMAL},
        {0, 1.0, EVENT_TYPE_STOP},
        //
        {0, 1.0, EVENT_TYPE_STOP},
        {0, 1.0, EVENT_TYPE_STOP},
        {0, 1.0, EVENT_TYPE_STOP},
        {lilka::NOTE_A3, 1.0, EVENT_TYPE_NORMAL},
        //
        {0, 1.0, EVENT_TYPE_STOP},
        {lilka::NOTE_B3, 1.0, EVENT_TYPE_NORMAL},
        {0, 1.0, EVENT_TYPE_STOP},
        {lilka::NOTE_A3, 1.0, EVENT_TYPE_NORMAL},
        //
        {lilka::NOTE_C4, 1.0, EVENT_TYPE_NORMAL},
        {0, 1.0, EVENT_TYPE_STOP},
        {lilka::NOTE_B3, 1.0, EVENT_TYPE_NORMAL},
        {0, 1.0, EVENT_TYPE_STOP},
    };
    event_t channel1[] = {
        {lilka::NOTE_E2, 1.0, EVENT_TYPE_NORMAL},
        {lilka::NOTE_G2, 1.0, EVENT_TYPE_NORMAL},
        {lilka::NOTE_A2, 1.0, EVENT_TYPE_NORMAL},
        {lilka::NOTE_B2, 1.0, EVENT_TYPE_NORMAL},
        //
        {0, 1.0, EVENT_TYPE_STOP}, // {lilka::NOTE_E2, 1.0},
        {lilka::NOTE_G2, 1.0, EVENT_TYPE_NORMAL},
        {lilka::NOTE_A2, 1.0, EVENT_TYPE_NORMAL},
        {lilka::NOTE_B2, 1.0, EVENT_TYPE_NORMAL},
        //
        {0, 1.0, EVENT_TYPE_STOP}, // {lilka::NOTE_E2, 1.0},
        {lilka::NOTE_G2, 1.0, EVENT_TYPE_NORMAL},
        {lilka::NOTE_A2, 1.0, EVENT_TYPE_NORMAL},
        {0, 1.0, EVENT_TYPE_STOP}, // {lilka::NOTE_B2, 1.0},
        //
        {lilka::NOTE_E2, 1.0, EVENT_TYPE_NORMAL},
        {lilka::NOTE_G2, 1.0, EVENT_TYPE_NORMAL},
        {0, 1.0, EVENT_TYPE_STOP}, // {lilka::NOTE_A2, 1.0},
        {lilka::NOTE_B2, 1.0, EVENT_TYPE_NORMAL},
        //
        {lilka::NOTE_C2, 1.0, EVENT_TYPE_NORMAL},
        {lilka::NOTE_E2, 1.0, EVENT_TYPE_NORMAL},
        {lilka::NOTE_F2, 1.0, EVENT_TYPE_NORMAL},
        {lilka::NOTE_G2, 1.0, EVENT_TYPE_NORMAL},
        //
        {0, 1.0, EVENT_TYPE_STOP}, // {lilka::NOTE_C2, 1.0},
        {lilka::NOTE_E2, 1.0, EVENT_TYPE_NORMAL},
        {lilka::NOTE_F2, 1.0, EVENT_TYPE_NORMAL},
        {lilka::NOTE_G2, 1.0, EVENT_TYPE_NORMAL},
        //
        {0, 1.0, EVENT_TYPE_STOP}, // {lilka::NOTE_C2, 1.0},
        {lilka::NOTE_E2, 1.0, EVENT_TYPE_NORMAL},
        {lilka::NOTE_F2, 1.0, EVENT_TYPE_NORMAL},
        {0, 1.0, EVENT_TYPE_STOP}, // {lilka::NOTE_G2, 1.0},
        //
        {lilka::NOTE_C2, 1.0, EVENT_TYPE_NORMAL},
        {lilka::NOTE_E2, 1.0, EVENT_TYPE_NORMAL},
        {0, 1.0, EVENT_TYPE_STOP}, // {lilka::NOTE_F2, 1.0},
        {lilka::NOTE_G2, 1.0, EVENT_TYPE_NORMAL},
    };
    event_t channel2[] = {
        {lilka::NOTE_E6, 1.0, EVENT_TYPE_NORMAL},
        {0, 1.0, EVENT_TYPE_STOP},
        {0, 1.0, EVENT_TYPE_STOP},
        {0, 1.0, EVENT_TYPE_STOP},
        //
        {lilka::NOTE_E6, 1.0, EVENT_TYPE_NORMAL},
        {0, 1.0, EVENT_TYPE_STOP},
        {lilka::NOTE_E6, 1.0, EVENT_TYPE_NORMAL},
        {0, 1.0, EVENT_TYPE_STOP},
        //
        {0, 1.0, EVENT_TYPE_STOP},
        {lilka::NOTE_E6, 1.0, EVENT_TYPE_NORMAL},
        {0, 1.0, EVENT_TYPE_STOP},
        {0, 1.0, EVENT_TYPE_STOP},
        //
        {lilka::NOTE_E6, 1.0, EVENT_TYPE_NORMAL},
        {0, 1.0, EVENT_TYPE_STOP},
        {0, 1.0, EVENT_TYPE_STOP},
        {0, 1.0, EVENT_TYPE_STOP},
        //
        {lilka::NOTE_C6, 1.0, EVENT_TYPE_NORMAL},
        {0, 1.0, EVENT_TYPE_STOP},
        {0, 1.0, EVENT_TYPE_STOP},
        {0, 1.0, EVENT_TYPE_STOP},
        //
        {lilka::NOTE_C6, 1.0, EVENT_TYPE_NORMAL},
        {0, 1.0, EVENT_TYPE_STOP},
        {lilka::NOTE_C6, 1.0, EVENT_TYPE_NORMAL},
        {0, 1.0, EVENT_TYPE_STOP},
        //
        {0, 1.0, EVENT_TYPE_STOP},
        {lilka::NOTE_C6, 1.0, EVENT_TYPE_NORMAL},
        {0, 1.0, EVENT_TYPE_STOP},
        {0, 1.0, EVENT_TYPE_STOP},
        //
        {lilka::NOTE_C6, 1.0, EVENT_TYPE_NORMAL},
        {0, 1.0, EVENT_TYPE_STOP},
        {0, 1.0, EVENT_TYPE_STOP},
        {0, 1.0, EVENT_TYPE_STOP},
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

    bool isEditing = false;

    while (1) {
        seq_state_t seqState = sequencer.getSeqState();

        if (seqState.playing) {
            cursorY = seqState.eventIndex;
        }

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
            if (eventIndex % 4 == 0) {
                canvas->fillRect(0, y, canvas->width(), itemHeight, lilka::colors::Delft_blue);
            }
            if (eventIndex == cursorY) {
                canvas->drawRect(0, y, canvas->width(), itemHeight, lilka::colors::Blue);
            }
            canvas->setCursor(24, y + 13);
            canvas->printf("| %02X | ", eventIndex);
            for (int channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
                noteinfo_t noteinfo;
                event_t event = pattern.getChannelEvent(channelIndex, eventIndex);
                char str[4];
                if (event.type == EVENT_TYPE_CONT) {
                    strcpy(str, "...");
                } else if (event.type == EVENT_TYPE_NORMAL) {
                    noteinfo = frequency_to_noteinfo(event.pitch);
                    strcpy(str, noteinfo.toStr());
                } else if (event.type == EVENT_TYPE_STOP) {
                    strcpy(str, "OFF");
                } else {
                    strcpy(str, "???");
                }
                uint16_t textColor;
                int32_t bgColor;
                if (cursorY == eventIndex && cursorX == channelIndex) {
                    if (isEditing) {
                        textColor = lilka::colors::Black;
                        bgColor = lilka::colors::Green;
                    } else {
                        textColor = lilka::colors::Black;
                        bgColor = lilka::colors::Uranian_blue;
                    }
                } else {
                    if (event.type == EVENT_TYPE_NORMAL) {
                        textColor = lilka::colors::White;
                    } else {
                        textColor = lilka::colors::Uranian_blue;
                    }
                    bgColor = -1;
                }
                canvas->setTextColor(textColor);
                if (bgColor != -1) {
                    int16_t cx = canvas->getCursorX();
                    int16_t cy = canvas->getCursorY();
                    int16_t _x, _y;
                    uint16_t _w, _h;
                    canvas->getTextBounds(str, cx, cy, &_x, &_y, &_w, &_h);
                    canvas->fillRect(_x - 2, y + 1, _w + 4, itemHeight - 2, bgColor);
                }
                canvas->printf("%s", str);
                canvas->setTextColor(lilka::colors::White);
                canvas->printf(" | ");
            }
        }
        lilka::State state = lilka::controller.getState();

        if (isEditing) {
            // Edit mode
            if (state.a.justPressed) {
                // Exit edit mode
                mixer.stop();
                isEditing = false;
            }
            if (state.up.justPressed || state.down.justPressed || state.left.justPressed || state.right.justPressed ||
                state.c.justPressed) {
                // Adjust note
                event_t event = pattern.getChannelEvent(cursorX, cursorY);
                noteinfo_t noteinfo = frequency_to_noteinfo(event.pitch);
                if (event.type == EVENT_TYPE_NORMAL) {
                    if (state.up.justPressed) {
                        noteinfo.index++;
                        if (noteinfo.index >= 12) {
                            noteinfo.index = 0;
                            noteinfo.octave++;
                        }
                    } else if (state.down.justPressed) {
                        noteinfo.index--;
                        if (noteinfo.index < 0) {
                            noteinfo.index = 11;
                            noteinfo.octave--;
                        }
                    }
                    if (state.left.justPressed) {
                        noteinfo.octave--;
                        if (noteinfo.octave < 0) {
                            noteinfo.octave = 0;
                        }
                    } else if (state.right.justPressed) {
                        noteinfo.octave++;
                        if (noteinfo.octave >= 9) {
                            noteinfo.octave = 9;
                        }
                    }
                }
                if (state.c.justPressed) {
                    event.type = static_cast<event_type_t>((event.type + 1) % EVENT_TYPE_COUNT);
                }
                event.pitch = noteinfo_to_frequency(noteinfo);
                pattern.setChannelEvent(cursorX, cursorY, event);
            }
            if (state.b.justPressed) {
                // Play single event
                for (int channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
                    event_t event = pattern.getChannelEvent(channelIndex, cursorY);
                    if (channelIndex == cursorX && event.type == EVENT_TYPE_NORMAL) {
                        mixer.start(
                            channelIndex, pattern.getChannelWaveform(channelIndex), event.pitch, event.velocity
                        );
                    } else {
                        mixer.start(channelIndex, WAVEFORM_SILENCE, 0.0, 0.0);
                    }
                }
            } else if (state.b.justReleased) {
                // Stop playing single event
                mixer.stop();
            }
        } else {
            // Normal mode
            if (seqState.playing) {
                // Is playing
                cursorY = seqState.eventIndex;
                if (state.start.justPressed) {
                    // Stop playing
                    sequencer.stop();
                }
            } else {
                // Not playing
                if (state.b.justPressed) {
                    // Play all events from this row
                    for (int channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
                        event_t event = pattern.getChannelEvent(channelIndex, cursorY);
                        waveform_t waveform = pattern.getChannelWaveform(channelIndex);
                        if (event.type != EVENT_TYPE_NORMAL) {
                            waveform = WAVEFORM_SILENCE;
                        }
                        mixer.start(channelIndex, waveform, event.pitch, event.velocity);
                    }
                } else if (state.b.justReleased) {
                    // Stop playing single event
                    mixer.stop();
                }

                if (state.a.justPressed) {
                    // Exit edit mode
                    mixer.stop();
                    isEditing = true;
                }

                if (state.up.justPressed) {
                    cursorY = (cursorY - 1 + CHANNEL_SIZE) % CHANNEL_SIZE;
                } else if (state.down.justPressed) {
                    cursorY = (cursorY + 1) % CHANNEL_SIZE;
                } else if (state.left.justPressed) {
                    cursorX = (cursorX - 1 + CHANNEL_COUNT) % CHANNEL_COUNT;
                } else if (state.right.justPressed) {
                    cursorX = (cursorX + 1) % CHANNEL_COUNT;
                }

                if (state.start.justPressed) {
                    // Start playing
                    sequencer.play(&pattern, true);
                }
                if (state.a.justPressed) {
                    // Enter edit mode
                    isEditing = true;
                }
            }
        }

        queueDraw();
    }
}

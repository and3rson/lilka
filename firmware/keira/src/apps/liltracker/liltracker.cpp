#include <lilka.h>

#include "liltracker.h"
#include "note.h"

constexpr int32_t SCORE_ITEM_HEIGHT = 15;
constexpr int32_t SCORE_HEADER_TOP = 64;
constexpr int32_t SCORE_TOP = SCORE_HEADER_TOP + SCORE_ITEM_HEIGHT;
const int32_t SCORE_HEIGHT = lilka::display.height() - SCORE_TOP;
const uint8_t* FONT = FONT_8x13_MONO;
const int32_t SCORE_COUNTER_WIDTH = 32;
const int32_t SCORE_EVENT_WIDTH = (lilka::display.width() - SCORE_COUNTER_WIDTH) / CHANNEL_COUNT;
const int32_t SCORE_ROW_COUNT = SCORE_HEIGHT / SCORE_ITEM_HEIGHT;
const int32_t SCORE_MIDDLE_ROW_INDEX = SCORE_ROW_COUNT / 2;

LilTrackerApp::LilTrackerApp() :
    App("LilTracker", 0, 0, lilka::display.width(), lilka::display.height()), mixer(), sequencer(&mixer) {
    this->setFlags(APP_FLAG_FULLSCREEN);
    this->setCore(1);
}

xSemaphoreHandle xMutex;

void LilTrackerApp::run() {
    const effect_t tremolo = {EFFECT_TYPE_TREMOLO, 0xF8};
    const effect_t arpeggio = {EFFECT_TYPE_ARPEGGIO, 0x6C};
    event_t channel0[] = {
        {N_E4, 64, EVENT_TYPE_NORMAL},
        {N_C0, 64, EVENT_TYPE_STOP},
        {N_E4, 64, EVENT_TYPE_NORMAL},
        {N_C0, 64, EVENT_TYPE_STOP},
        //
        {N_C0, 64, EVENT_TYPE_STOP},
        {N_C0, 64, EVENT_TYPE_STOP},
        {N_C0, 64, EVENT_TYPE_STOP},
        {N_E4, 64, EVENT_TYPE_NORMAL},
        //
        {N_C0, 64, EVENT_TYPE_STOP},
        {N_E4, 64, EVENT_TYPE_NORMAL},
        {N_C0, 64, EVENT_TYPE_STOP},
        {N_D4, 64, EVENT_TYPE_NORMAL},
        //
        {N_E4, 64, EVENT_TYPE_NORMAL},
        {N_C0, 64, EVENT_TYPE_STOP},
        {N_D4, 64, EVENT_TYPE_NORMAL},
        {N_C0, 64, EVENT_TYPE_STOP},
        //
        {N_C4, 64, EVENT_TYPE_NORMAL},
        {N_C0, 64, EVENT_TYPE_STOP},
        {N_C4, 64, EVENT_TYPE_NORMAL},
        {N_C0, 64, EVENT_TYPE_STOP},
        //
        {N_C0, 64, EVENT_TYPE_STOP},
        {N_C0, 64, EVENT_TYPE_STOP},
        {N_C0, 64, EVENT_TYPE_STOP},
        {N_A3, 64, EVENT_TYPE_NORMAL},
        //
        {N_C0, 64, EVENT_TYPE_STOP},
        {N_B3, 64, EVENT_TYPE_NORMAL},
        {N_C0, 64, EVENT_TYPE_STOP},
        {N_A3, 64, EVENT_TYPE_NORMAL},
        //
        {N_C4, 64, EVENT_TYPE_NORMAL},
        {N_C0, 64, EVENT_TYPE_STOP},
        {N_B3, 64, EVENT_TYPE_NORMAL},
        {N_C0, 64, EVENT_TYPE_STOP},
    };
    event_t channel1[] = {
        {N_E2, 0, EVENT_TYPE_NORMAL, tremolo},
        {N_G2, 0, EVENT_TYPE_STOP},
        {N_A2, 0, EVENT_TYPE_NORMAL},
        {N_B2, 0, EVENT_TYPE_NORMAL},
        //
        {N_C0, 0, EVENT_TYPE_STOP}, // {N_E2, 0},
        {N_G2, 0, EVENT_TYPE_NORMAL},
        {N_A2, 0, EVENT_TYPE_NORMAL},
        {N_B2, 0, EVENT_TYPE_NORMAL},
        //
        {N_C0, 0, EVENT_TYPE_STOP}, // {N_E2, 0},
        {N_G2, 0, EVENT_TYPE_NORMAL},
        {N_A2, 0, EVENT_TYPE_NORMAL},
        {N_B2, 0, EVENT_TYPE_NORMAL}, // {N_B2, 0},
        //
        {N_G2, 0, EVENT_TYPE_NORMAL},
        {N_A2, 0, EVENT_TYPE_NORMAL},
        {N_B2, 0, EVENT_TYPE_NORMAL}, // {N_A2, 0},
        {N_E2, 0, EVENT_TYPE_NORMAL},
        //
        {N_C2, 0, EVENT_TYPE_NORMAL},
        {N_E2, 0, EVENT_TYPE_STOP},
        {N_F2, 0, EVENT_TYPE_NORMAL},
        {N_G2, 0, EVENT_TYPE_NORMAL},
        //
        {N_C0, 0, EVENT_TYPE_STOP}, // {N_C2, 0},
        {N_E2, 0, EVENT_TYPE_NORMAL},
        {N_F2, 0, EVENT_TYPE_NORMAL},
        {N_G2, 0, EVENT_TYPE_NORMAL},
        //
        {N_C0, 0, EVENT_TYPE_STOP}, // {N_C2, 0},
        {N_E2, 0, EVENT_TYPE_NORMAL},
        {N_F2, 0, EVENT_TYPE_NORMAL},
        {N_G2, 0, EVENT_TYPE_NORMAL}, // {N_G2, 0},
        //
        {N_E2, 0, EVENT_TYPE_NORMAL},
        {N_F2, 0, EVENT_TYPE_NORMAL},
        {N_G2, 0, EVENT_TYPE_NORMAL}, // {N_F2, 0},
        {N_C2, 0, EVENT_TYPE_NORMAL},
    };
    event_t channel2[] = {
        {N_E6, 127, EVENT_TYPE_NORMAL, arpeggio},
        {N_C0, 96, EVENT_TYPE_CONT},
        {N_C0, 64, EVENT_TYPE_CONT},
        {N_C0, 32, EVENT_TYPE_CONT},
        //
        {N_E6, 0, EVENT_TYPE_NORMAL},
        {N_C0, 0, EVENT_TYPE_STOP},
        {N_E6, 0, EVENT_TYPE_NORMAL},
        {N_C0, 0, EVENT_TYPE_STOP},
        //
        {N_C0, 0, EVENT_TYPE_STOP},
        {N_E6, 0, EVENT_TYPE_NORMAL, arpeggio},
        {N_C0, 0, EVENT_TYPE_STOP},
        {N_C0, 0, EVENT_TYPE_STOP},
        //
        {N_E6, 0, EVENT_TYPE_NORMAL},
        {N_C0, 0, EVENT_TYPE_STOP},
        {N_C0, 0, EVENT_TYPE_STOP},
        {N_C0, 0, EVENT_TYPE_STOP},
        //
        {N_C6, 127, EVENT_TYPE_NORMAL, arpeggio},
        {N_C0, 96, EVENT_TYPE_CONT},
        {N_C0, 64, EVENT_TYPE_CONT},
        {N_C0, 32, EVENT_TYPE_CONT},
        //
        {N_C6, 0, EVENT_TYPE_NORMAL},
        {N_C0, 0, EVENT_TYPE_STOP},
        {N_C6, 0, EVENT_TYPE_NORMAL},
        {N_C0, 0, EVENT_TYPE_STOP},
        //
        {N_C0, 0, EVENT_TYPE_STOP},
        {N_C6, 0, EVENT_TYPE_NORMAL, arpeggio},
        {N_C0, 0, EVENT_TYPE_STOP},
        {N_C0, 0, EVENT_TYPE_STOP},
        //
        {N_C6, 0, EVENT_TYPE_NORMAL},
        {N_C0, 0, EVENT_TYPE_STOP},
        {N_C0, 0, EVENT_TYPE_STOP},
        {N_C0, 0, EVENT_TYPE_STOP},
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
    int currentChannel = 0;
    int currentSegment = 0;

    bool isEditing = false;

    char str[32];

    while (1) {
        seq_state_t seqState = sequencer.getSeqState();

        if (seqState.playing) {
            cursorY = seqState.eventIndex;
        }

        canvas->fillScreen(lilka::colors::Black);
        canvas->setFont(FONT);
        canvas->setTextBound(0, 0, canvas->width(), canvas->height());
        canvas->setTextColor(lilka::colors::White);
        canvas->drawTextAligned(
            "##", SCORE_COUNTER_WIDTH / 2, SCORE_HEADER_TOP, lilka::ALIGN_CENTER, lilka::ALIGN_START
        );

        for (int channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
            canvas->drawTextAligned(
                waveform_names[pattern.getChannelWaveform(channelIndex)],
                SCORE_COUNTER_WIDTH + channelIndex * SCORE_EVENT_WIDTH,
                SCORE_HEADER_TOP,
                lilka::ALIGN_START,
                lilka::ALIGN_START
            );
        }

        for (int eventIndex = 0; eventIndex < CHANNEL_SIZE; eventIndex++) {
            int scoreRowIndex = eventIndex - cursorY + SCORE_MIDDLE_ROW_INDEX - 1;
            if (scoreRowIndex < 0 || scoreRowIndex >= SCORE_ROW_COUNT) {
                // Not visible
                continue;
            }
            // Top coordinate of the score item on screen
            int16_t y = SCORE_TOP + scoreRowIndex * SCORE_ITEM_HEIGHT;
            // Draw score, with current event in the middle
            if (eventIndex % 4 == 0) {
                canvas->fillRect(0, y, canvas->width(), SCORE_ITEM_HEIGHT, lilka::colors::Delft_blue);
            }
            if (eventIndex == cursorY) {
                canvas->drawRect(0, y, canvas->width(), SCORE_ITEM_HEIGHT, lilka::colors::Blue);
            }
            sprintf(str, "%02X", eventIndex);
            canvas->drawTextAligned(
                str, SCORE_COUNTER_WIDTH / 2, y + SCORE_ITEM_HEIGHT / 2, lilka::ALIGN_CENTER, lilka::ALIGN_CENTER
            );
            for (int channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
                event_t event = pattern.getChannelEvent(channelIndex, eventIndex);
                int xOffset = SCORE_COUNTER_WIDTH + channelIndex * SCORE_EVENT_WIDTH;
                if (event.type == EVENT_TYPE_CONT) {
                    strcpy(str, "...");
                } else if (event.type == EVENT_TYPE_NORMAL) {
                    strcpy(str, event.note.toStr());
                } else if (event.type == EVENT_TYPE_STOP) {
                    strcpy(str, "OFF");
                } else {
                    strcpy(str, "???");
                }
                bool eventFocused = channelIndex == currentChannel && cursorY == eventIndex;
                bool isDimmed = event.type != EVENT_TYPE_NORMAL;
                // Note
                xOffset += printText(
                    str,
                    xOffset,
                    y + SCORE_ITEM_HEIGHT / 2,
                    lilka::ALIGN_START,
                    lilka::ALIGN_CENTER,
                    isEditing,
                    eventFocused && currentSegment == 0,
                    isDimmed
                );
                xOffset += 4;
                // Volume
                if (event.volume == 0) {
                    sprintf(str, "..");
                } else {
                    sprintf(str, "%02X", event.volume);
                }
                xOffset += printText(
                    str,
                    xOffset,
                    y + SCORE_ITEM_HEIGHT / 2,
                    lilka::ALIGN_START,
                    lilka::ALIGN_CENTER,
                    isEditing,
                    eventFocused && currentSegment == 1,
                    isDimmed
                );
                xOffset += 4;
                // Effect
                sprintf(str, "%c", effect_signs[event.effect.type]);
                xOffset += printText(
                    str,
                    xOffset,
                    y + SCORE_ITEM_HEIGHT / 2,
                    lilka::ALIGN_START,
                    lilka::ALIGN_CENTER,
                    isEditing,
                    eventFocused && currentSegment == 2,
                    isDimmed
                );
                xOffset += 4;
                // Effect param
                sprintf(str, "%02X", event.effect.param);
                xOffset += printText(
                    str,
                    xOffset,
                    y + SCORE_ITEM_HEIGHT / 2,
                    lilka::ALIGN_START,
                    lilka::ALIGN_CENTER,
                    isEditing,
                    eventFocused && currentSegment == 3,
                    isDimmed
                );
                xOffset += 4;
                (void)xOffset;
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
                event_t event = pattern.getChannelEvent(currentChannel, cursorY);
                if (currentSegment == 0) {
                    if (event.type == EVENT_TYPE_NORMAL) {
                        if (state.up.justPressed) {
                            event.note.add(1);
                        } else if (state.down.justPressed) {
                            event.note.add(-1);
                        }
                        if (state.left.justPressed) {
                            event.note.add(-12);
                        } else if (state.right.justPressed) {
                            event.note.add(12);
                        }
                    }
                } else if (currentSegment == 1) {
                    // Adjust volume
                    if (state.up.justPressed) {
                        event.volume = (event.volume + 1) % (MAX_VOLUME + 1);
                    } else if (state.down.justPressed) {
                        event.volume = (event.volume - 1 + (MAX_VOLUME + 1)) % (MAX_VOLUME + 1);
                    }
                    if (state.left.justPressed) {
                        event.volume = (event.volume - 16 + (MAX_VOLUME + 1)) % (MAX_VOLUME + 1);
                    } else if (state.right.justPressed) {
                        event.volume = (event.volume + 16) % (MAX_VOLUME + 1);
                    }
                } else if (currentSegment == 2) {
                    // Adjust effect
                    if (event.type == EVENT_TYPE_NORMAL) {
                        if (state.up.justPressed) {
                            event.effect.type = static_cast<effect_type_t>((event.effect.type + 1) % EFFECT_TYPE_COUNT);
                        } else if (state.down.justPressed) {
                            event.effect.type = static_cast<effect_type_t>(
                                (event.effect.type - 1 + EFFECT_TYPE_COUNT) % EFFECT_TYPE_COUNT
                            );
                        }
                    }
                } else if (currentSegment == 3) {
                    // Adjust effect param
                    if (event.type == EVENT_TYPE_NORMAL) {
                        if (state.up.justPressed) {
                            event.effect.param = (event.effect.param + 1) % 256;
                        } else if (state.down.justPressed) {
                            event.effect.param = (event.effect.param - 1 + 256) % 256;
                        }
                        if (state.left.justPressed) {
                            event.effect.param = (event.effect.param - 16 + 256) % 256;
                        } else if (state.right.justPressed) {
                            event.effect.param = (event.effect.param + 16) % 256;
                        }
                    }
                }
                if (state.c.justPressed) {
                    event.type = static_cast<event_type_t>((event.type + 1) % EVENT_TYPE_COUNT);
                }
                pattern.setChannelEvent(currentChannel, cursorY, event);
            }
            if (state.b.justPressed) {
                // Play single event
                for (int channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
                    event_t event = pattern.getChannelEvent(channelIndex, cursorY);
                    if (channelIndex == currentChannel && event.type == EVENT_TYPE_NORMAL) {
                        mixer.start(
                            channelIndex,
                            pattern.getChannelWaveform(channelIndex),
                            event.note.toFrequency(),
                            event.volume > 0 ? ((float)event.volume) / MAX_VOLUME : 1.0,
                            event.effect
                        );
                    } else {
                        mixer.start(channelIndex, WAVEFORM_SILENCE, 0.0, 1.0);
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
                        mixer.start(
                            channelIndex,
                            waveform,
                            event.note.toFrequency(),
                            event.volume > 0 ? ((float)event.volume) / MAX_VOLUME : 1.0,
                            event.effect
                        );
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
                    cursorX = (cursorX - 1 + CHANNEL_COUNT * 4) % (CHANNEL_COUNT * 4);
                    currentChannel = cursorX / 4;
                    currentSegment = cursorX % 4;
                } else if (state.right.justPressed) {
                    cursorX = (cursorX + 1) % (CHANNEL_COUNT * 4);
                    currentChannel = cursorX / 4;
                    currentSegment = cursorX % 4;
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

int LilTrackerApp::printText(
    const char* text, int16_t x, int16_t y, lilka::Alignment hAlign, lilka::Alignment vAlign, bool editing,
    bool focused, bool dimmed
) {
    uint16_t textColor;
    int32_t bgColor;
    if (focused) {
        if (editing) {
            textColor = lilka::colors::Black;
            bgColor = lilka::colors::Green;
        } else {
            textColor = lilka::colors::Black;
            bgColor = lilka::colors::Uranian_blue;
        }
    } else {
        if (!dimmed) {
            textColor = lilka::colors::White;
        } else {
            textColor = lilka::colors::Uranian_blue;
        }
        bgColor = textColor;
    }
    canvas->setTextColor(textColor, bgColor);
    return canvas->drawTextAligned(text, x, y, hAlign, vAlign);
}

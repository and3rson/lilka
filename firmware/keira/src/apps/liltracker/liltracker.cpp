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

typedef enum {
    ACTIVE_BLOCK_CONTROLS,
    ACTIVE_BLOCK_EVENT_EDITING,
    ACTIVE_BLOCK_COUNT,
} active_block_t;

LilTrackerApp::LilTrackerApp() :
    App("LilTracker", 0, 0, lilka::display.width(), lilka::display.height()), mixer(), sequencer(&mixer) {
    this->setFlags(APP_FLAG_FULLSCREEN);
    this->setCore(1);
    this->setStackSize(16384);
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
    event_t drums[] = {
        {N_C0, 0, EVENT_TYPE_STOP},
        {N_C0, 0, EVENT_TYPE_STOP},
        {N_E6, 80, EVENT_TYPE_NORMAL},
        {N_C0, 0, EVENT_TYPE_STOP},
        //
        {N_C0, 0, EVENT_TYPE_STOP},
        {N_C0, 0, EVENT_TYPE_STOP},
        {N_E6, 80, EVENT_TYPE_NORMAL},
        {N_C0, 0, EVENT_TYPE_STOP},
        //
        {N_C0, 0, EVENT_TYPE_STOP},
        {N_C0, 0, EVENT_TYPE_STOP},
        {N_E6, 80, EVENT_TYPE_NORMAL},
        {N_C0, 0, EVENT_TYPE_STOP},
        //
        {N_C0, 0, EVENT_TYPE_STOP},
        {N_C0, 0, EVENT_TYPE_STOP},
        {N_E6, 80, EVENT_TYPE_NORMAL},
        {N_C0, 0, EVENT_TYPE_STOP},
        //
        {N_C0, 0, EVENT_TYPE_STOP},
        {N_C0, 0, EVENT_TYPE_STOP},
        {N_E6, 80, EVENT_TYPE_NORMAL},
        {N_C0, 0, EVENT_TYPE_STOP},
        //
        {N_C0, 0, EVENT_TYPE_STOP},
        {N_C0, 0, EVENT_TYPE_STOP},
        {N_E6, 80, EVENT_TYPE_NORMAL},
        {N_C0, 0, EVENT_TYPE_STOP},
        //
        {N_C0, 0, EVENT_TYPE_STOP},
        {N_C0, 0, EVENT_TYPE_STOP},
        {N_E6, 80, EVENT_TYPE_NORMAL},
        {N_C0, 0, EVENT_TYPE_STOP},
        //
        {N_C0, 0, EVENT_TYPE_STOP},
        {N_C0, 0, EVENT_TYPE_STOP},
        {N_E6, 80, EVENT_TYPE_NORMAL},
        {N_C0, 0, EVENT_TYPE_STOP},
        //
    };

    Track track;

    {
        page_t* page;

        page = track.getPage(0);
        page->patternIndices[0] = 0;
        page->patternIndices[1] = 2;
        page->patternIndices[2] = 0;

        page = track.getPage(1);
        page->patternIndices[0] = 0;
        page->patternIndices[1] = 2;
        page->patternIndices[2] = 1;

        page = track.getPage(2);
        page->patternIndices[0] = 2;
        page->patternIndices[1] = 2;
        page->patternIndices[2] = 1;

        page = track.getPage(3);
        page->patternIndices[0] = 2;
        page->patternIndices[1] = 2;
        page->patternIndices[2] = 1;

        page = track.getPage(4);
        page->patternIndices[0] = 3;
        page->patternIndices[1] = 2;
        page->patternIndices[2] = 1;

        page = track.getPage(5);
        page->patternIndices[0] = 3;
        page->patternIndices[1] = 2;
        page->patternIndices[2] = 1;

        page = track.getPage(6);
        page->patternIndices[0] = 3;
        page->patternIndices[1] = 2;
        page->patternIndices[2] = 0;

        page = track.getPage(7);
        page->patternIndices[0] = 0;
        page->patternIndices[1] = 2;
        page->patternIndices[2] = 0;

        Pattern* pattern;

        pattern = track.getPattern(1);
        pattern->setChannelEvents(2, drums);
        pattern->setChannelWaveform(2, WAVEFORM_NOISE);

        pattern = track.getPattern(2);
        pattern->setChannelEvents(0, channel0);
        pattern->setChannelWaveform(0, WAVEFORM_SQUARE);
        pattern->setChannelEvents(1, channel1);
        pattern->setChannelWaveform(1, WAVEFORM_SAWTOOTH);
        pattern->setChannelEvents(2, channel2);
        // pattern.setChannelWaveform(2, WAVEFORM_TRIANGLE);
        pattern->setChannelWaveform(2, WAVEFORM_SINE);

        pattern = track.getPattern(3);
        pattern->setChannelEvents(0, channel2);
        pattern->setChannelWaveform(0, WAVEFORM_SINE);
    }

    int pageIndex = 0;

    int8_t activeBlock = ACTIVE_BLOCK_EVENT_EDITING;
    int scoreCursorX = 0;
    int scoreCursorY = 0;
    int currentChannel = 0;
    int currentSegment = 0;

    bool isEditing = false;
    bool isPreviewing = false;

    int controlCursorX = 0;
    int controlCursorY = 0;

    char str[32];

    while (1) {
        seq_state_t seqState = sequencer.getSeqState();

        if (seqState.playing) {
            scoreCursorY = seqState.eventIndex;
            pageIndex = seqState.pageIndex;
        }

        page_t* page = track.getPage(pageIndex);

        canvas->fillScreen(lilka::colors::Black);

        // // Draw mixer buffer
        // int16_t buffer[MIXER_BUFFER_SIZE];
        // mixer.readBuffer(buffer);
        //
        // int16_t prevX, prevY;
        // for (int i = 0; i < MIXER_BUFFER_SIZE; i++) {
        //     int x = i * lilka::display.width() / MIXER_BUFFER_SIZE;
        //     int y = SCORE_HEADER_TOP / 2 + ((float)buffer[i] / 32768 / mixer.getMasterVolume()) * SCORE_HEADER_TOP / 2;
        //     if (i > 0) {
        //         canvas->drawLine(prevX, prevY, x, y, lilka::colors::White);
        //     }
        //     prevX = x;
        //     prevY = y;
        // }

        // Draw channel buffers
        if (seqState.playing || isPreviewing) {
            for (int channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
                int16_t buffer[MIXER_BUFFER_SIZE];
                mixer.readBuffer(buffer, channelIndex);
                int16_t prevX, prevY;
                int16_t minX = SCORE_COUNTER_WIDTH + channelIndex * SCORE_EVENT_WIDTH;
                int16_t width = SCORE_EVENT_WIDTH;

                for (int i = 0; i < MIXER_BUFFER_SIZE; i += 4) {
                    int x = minX + i * width / MIXER_BUFFER_SIZE;
                    int index = i / 2; // Make samples wider for nicer display
                    int y = SCORE_HEADER_TOP / 2 +
                            ((float)buffer[index] / 32768 / mixer.getMasterVolume()) * SCORE_HEADER_TOP / 2;
                    if (i > 0) {
                        canvas->drawLine(prevX, prevY, x, y, lilka::colors::White);
                    }
                    prevX = x;
                    prevY = y;
                }
            }
        } else {
            // Draw current page info and pattern indices
            sprintf(str, "Page %02X", pageIndex);
            bool isPageSelectorFocused = activeBlock == ACTIVE_BLOCK_CONTROLS && controlCursorY == 0;
            printText(
                str,
                lilka::display.width() / 2,
                SCORE_HEADER_TOP / 2,
                lilka::ALIGN_CENTER,
                lilka::ALIGN_CENTER,
                isPageSelectorFocused && isEditing,
                isPageSelectorFocused,
                false
            );
        }

        canvas->setFont(FONT);
        canvas->setTextBound(0, 0, canvas->width(), canvas->height());
        canvas->setTextColor(lilka::colors::White, lilka::colors::White);
        canvas->drawTextAligned(
            "##", SCORE_COUNTER_WIDTH / 2, SCORE_HEADER_TOP, lilka::ALIGN_CENTER, lilka::ALIGN_START
        );

        // Draw channel waveform names
        for (int channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
            Pattern* pattern = track.getPattern(page->patternIndices[channelIndex]);
            bool isChannelWaveformFocused =
                activeBlock == ACTIVE_BLOCK_CONTROLS && controlCursorY == 1 && controlCursorX == channelIndex;
            sprintf(
                str,
                "%02X: %-4s",
                page->patternIndices[channelIndex],
                waveform_names[pattern->getChannelWaveform(channelIndex)]
            );
            printText(
                str,
                SCORE_COUNTER_WIDTH + channelIndex * SCORE_EVENT_WIDTH,
                SCORE_HEADER_TOP,
                lilka::ALIGN_START,
                lilka::ALIGN_START,
                isChannelWaveformFocused && isEditing,
                isChannelWaveformFocused,
                false
            );
            // canvas->drawTextAligned(
            //     waveform_names[pattern->getChannelWaveform(channelIndex)],
            //     SCORE_COUNTER_WIDTH + channelIndex * SCORE_EVENT_WIDTH,
            //     SCORE_HEADER_TOP,
            //     lilka::ALIGN_START,
            //     lilka::ALIGN_START
            // );
        }

        for (int eventIndex = 0; eventIndex < CHANNEL_SIZE; eventIndex++) {
            int scoreRowIndex = eventIndex - scoreCursorY + SCORE_MIDDLE_ROW_INDEX - 1;
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
            if (eventIndex == scoreCursorY && activeBlock == ACTIVE_BLOCK_EVENT_EDITING) {
                canvas->drawRect(0, y, canvas->width(), SCORE_ITEM_HEIGHT, lilka::colors::Blue);
            }
            canvas->setTextColor(lilka::colors::White, lilka::colors::White);
            sprintf(str, "%02X", eventIndex);
            canvas->drawTextAligned(
                str, SCORE_COUNTER_WIDTH / 2, y + SCORE_ITEM_HEIGHT / 2, lilka::ALIGN_CENTER, lilka::ALIGN_CENTER
            );
            for (int channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
                Pattern* pattern = track.getPattern(page->patternIndices[channelIndex]);
                event_t event = pattern->getChannelEvent(channelIndex, eventIndex);
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
                bool eventFocused = activeBlock == ACTIVE_BLOCK_EVENT_EDITING && channelIndex == currentChannel &&
                                    scoreCursorY == eventIndex;
                bool isDimmed = event.type != EVENT_TYPE_NORMAL;
                // Note
                xOffset += printText(
                    str,
                    xOffset,
                    y + SCORE_ITEM_HEIGHT / 2,
                    lilka::ALIGN_START,
                    lilka::ALIGN_CENTER,
                    eventFocused && isEditing,
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
                    eventFocused && isEditing,
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
                    eventFocused && isEditing,
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
                    eventFocused && isEditing,
                    eventFocused && currentSegment == 3,
                    isDimmed
                );
                xOffset += 4;
                (void)xOffset;
            }
        }
        lilka::State state = lilka::controller.getState();

        if (activeBlock == ACTIVE_BLOCK_CONTROLS) {
            if (isEditing) {
                if (state.a.justPressed) {
                    // Exit edit mode
                    isEditing = false;
                }
                if (state.up.justPressed || state.down.justPressed || state.left.justPressed ||
                    state.right.justPressed) {
                    if (controlCursorY == 0) {
                        // Select page
                        if (state.up.justPressed || state.left.justPressed) {
                            pageIndex = (pageIndex - 1 + track.getPageCount()) % track.getPageCount();
                        } else if (state.down.justPressed || state.right.justPressed) {
                            pageIndex = (pageIndex + 1) % track.getPageCount();
                        }
                    } else if (controlCursorY == 1) {
                        // Select waveform for pattern's channel
                        Pattern* pattern = track.getPattern(page->patternIndices[controlCursorX]);
                        if (state.left.justPressed) {
                            // Previous waveform
                            int8_t newWaveform =
                                (pattern->getChannelWaveform(controlCursorX) - 1 + WAVEFORM_COUNT) % WAVEFORM_COUNT;
                            pattern->setChannelWaveform(controlCursorX, static_cast<waveform_t>(newWaveform));
                        } else if (state.right.justPressed) {
                            // Next waveform
                            int8_t newWaveform = (pattern->getChannelWaveform(controlCursorX) + 1) % WAVEFORM_COUNT;
                            pattern->setChannelWaveform(controlCursorX, static_cast<waveform_t>(newWaveform));
                        } else if (state.up.justPressed) {
                            // Previous pattern
                            page->patternIndices[controlCursorX] =
                                (page->patternIndices[controlCursorX] - 1 + track.getPatternCount()) %
                                track.getPatternCount();
                        } else if (state.down.justPressed) {
                            // Next pattern
                            page->patternIndices[controlCursorX] =
                                (page->patternIndices[controlCursorX] + 1) % track.getPatternCount();
                        }
                    }
                }
            } else {
                if (state.a.justPressed) {
                    // Enter edit mode
                    isEditing = true;
                }
                if (state.up.justPressed) {
                    controlCursorY = (controlCursorY - 1 + 2) % 2;
                } else if (state.down.justPressed) {
                    controlCursorY = (controlCursorY + 1) % 2;
                } else if (state.left.justPressed) {
                    controlCursorX = (controlCursorX - 1 + CHANNEL_COUNT) % CHANNEL_COUNT;
                } else if (state.right.justPressed) {
                    controlCursorX = (controlCursorX + 1) % CHANNEL_COUNT;
                }
                if (state.select.justPressed) {
                    activeBlock = (activeBlock + 1) % ACTIVE_BLOCK_COUNT;
                }
            }
        } else if (activeBlock == ACTIVE_BLOCK_EVENT_EDITING) {
            if (isEditing) {
                // Edit mode
                if (state.a.justPressed) {
                    // Exit edit mode
                    mixer.stop();
                    isPreviewing = false;
                    isEditing = false;
                }
                if (state.up.justPressed || state.down.justPressed || state.left.justPressed ||
                    state.right.justPressed || state.c.justPressed) {
                    // Adjust note
                    Pattern* pattern = track.getPattern(page->patternIndices[currentChannel]);
                    event_t event = pattern->getChannelEvent(currentChannel, scoreCursorY);
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
                                event.effect.type =
                                    static_cast<effect_type_t>((event.effect.type + 1) % EFFECT_TYPE_COUNT);
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
                    pattern->setChannelEvent(currentChannel, scoreCursorY, event);
                    if (isPreviewing) {
                        // Update preview
                        startPreview(&track, page, currentChannel, scoreCursorY);
                    }
                }
                if (state.b.justPressed) {
                    // Play single event
                    startPreview(&track, page, currentChannel, scoreCursorY);
                    isPreviewing = true;
                } else if (state.b.justReleased) {
                    // Stop playing single event
                    mixer.stop();
                    isPreviewing = false;
                }
            } else {
                // Normal mode
                if (seqState.playing) {
                    // Is playing
                    scoreCursorY = seqState.eventIndex;
                    if (state.start.justPressed) {
                        // Stop playing
                        sequencer.stop();
                    }
                } else {
                    // Not playing
                    if (state.b.justPressed) {
                        // Play all events from this row
                        startPreview(&track, page, -1, scoreCursorY);
                        isPreviewing = true;
                    } else if (state.b.justReleased) {
                        // Stop playing all events from this row
                        mixer.stop();
                        isPreviewing = false;
                    }

                    if (state.a.justPressed) {
                        // Enter edit mode
                        mixer.stop();
                        isEditing = true;
                        isPreviewing = false;
                    }

                    if (state.up.justPressed) {
                        scoreCursorY = (scoreCursorY - 1 + CHANNEL_SIZE) % CHANNEL_SIZE;
                    } else if (state.down.justPressed) {
                        scoreCursorY = (scoreCursorY + 1) % CHANNEL_SIZE;
                    } else if (state.left.justPressed) {
                        scoreCursorX = (scoreCursorX - 1 + CHANNEL_COUNT * 4) % (CHANNEL_COUNT * 4);
                        currentChannel = scoreCursorX / 4;
                        currentSegment = scoreCursorX % 4;
                    } else if (state.right.justPressed) {
                        scoreCursorX = (scoreCursorX + 1) % (CHANNEL_COUNT * 4);
                        currentChannel = scoreCursorX / 4;
                        currentSegment = scoreCursorX % 4;
                    }

                    if (state.start.justPressed) {
                        // Start playing
                        sequencer.play(&track, pageIndex, true);
                    }
                    if (state.a.justPressed) {
                        // Enter edit mode
                        isEditing = true;
                    }
                    if (state.select.justPressed) {
                        activeBlock = (activeBlock + 1) % ACTIVE_BLOCK_COUNT;
                    }
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

void LilTrackerApp::startPreview(
    Track* track, page_t* page, int32_t requestedChannelIndex, int32_t requestedEventIndex
) {
    for (int channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
        Pattern* pattern = track->getPattern(page->patternIndices[channelIndex]);
        event_t event = pattern->getChannelEvent(channelIndex, requestedEventIndex);
        bool shouldPlayThisChannel = requestedChannelIndex == -1 || requestedChannelIndex == channelIndex;
        if (shouldPlayThisChannel && event.type == EVENT_TYPE_NORMAL) {
            mixer.start(
                channelIndex,
                pattern->getChannelWaveform(channelIndex),
                event.note.toFrequency(),
                event.volume > 0 ? ((float)event.volume) / MAX_VOLUME : 1.0,
                event.effect
            );
        } else {
            mixer.start(channelIndex, WAVEFORM_SILENCE, 0.0, 1.0);
        }
    }
}

#include <Preferences.h>
#include "tamagotchi.h"
#include "tamalib.h"
#include "bitmaps.h"
#include "cpu.h"
#include "savestate.h"

static uint8_t matrix_buffer[LCD_HEIGHT][LCD_WIDTH] = {0};
static uint8_t icon_buffer[ICON_NUM] = {0};
static uint16_t current_freq = 0;

TamagotchiApp* TamagotchiApp::instance = nullptr;

TamagotchiApp::TamagotchiApp() : App("Tamagotchi") {
    TamagotchiApp::instance = this;
}

void TamagotchiApp::drawTriangle(uint8_t x, uint8_t y) {
    // display.drawLine(x,y,x+6,y);
    canvas->fillTriangle(x, y, x + 6, y, x + 3, y + 3, lilka::colors::White);
    // canvas->drawLine(x + 1, y + 1, x + 5 * 2, y + 1, lilka::colors::White);
    // canvas->drawLine(x + 2 * 2, y + 2 * 2, x + 4 * 2, y + 2, lilka::colors::White);
    // canvas->drawLine(x + 3 * 2, y + 3 * 2, x + 3 * 2, y + 3, lilka::colors::White);
}

void hal_log(log_level_t level, char* buf, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, buf);
    vsnprintf(buffer, sizeof(buffer), buf, args);
    va_end(args);

    lilka::serial.log("[tamagotchi] %s", buffer);
}

void TamagotchiApp::drawTamaSelection(uint8_t y) {
    uint8_t i;
    for (i = 0; i < 7; i++) {
        if (icon_buffer[i]) drawTriangle(i * 16 * 2 + 23, y);
        canvas->drawXBitmap(i * 16 * 2 + 18, y + 6, bitmaps + i * 18 * 4, 16 * 2, 9 * 2, lilka::colors::White);
    }
    if (icon_buffer[7]) {
        drawTriangle(7 * 16 * 2 + 23, y);
        canvas->drawXBitmap(7 * 16 * 2 + 18, y + 6, bitmaps + 7 * 18 * 4, 16 * 2, 9 * 2, lilka::colors::White);
    }
}

void TamagotchiApp::run() {
    hal_t hal = {
        .halt = []() -> void {},
        .log = hal_log,
        .sleep_until = [](uint32_t timestamp) -> void {},
        .get_timestamp = []() -> timestamp_t { return micros(); },
        .update_screen = []() -> void {
            lilka::Canvas* canvas = instance->canvas;
            canvas->fillScreen(lilka::colors::Black);
            for (uint16_t y = 0; y < LCD_HEIGHT; y++) {
                // if (j != 5) instance->drawTamaRow(j, j + j + j, 2);
                // if (j == 5) {
                //     instance->drawTamaRow(j, j + j + j, 1);
                //     instance->drawTamaRow(j, j + j + j + 1, 1);
                // }
                for (uint16_t x = 0; x < LCD_WIDTH; x++) {
                    if (matrix_buffer[y][x]) {
                        canvas->fillRect(x * 8, y * 8 + 24, 7, 7, lilka::colors::White);
                    }
                }
            }
            instance->drawTamaSelection(176);
            instance->queueDraw();
        },
        .set_lcd_matrix = [](uint8_t x, uint8_t y, uint8_t value) -> void { matrix_buffer[y][x] = value; },
        .set_lcd_icon = [](uint8_t icon_id, uint8_t value) -> void { icon_buffer[icon_id] = value; },
        .set_frequency = [](uint32_t frequency) -> void { current_freq = frequency; },
        .play_frequency = [](bool_t en) -> void {
            if (en) {
                lilka::buzzer.play(current_freq);
            } else {
                lilka::buzzer.stop();
            }
        },
        // .handler = &hal_handler,
        .handler = []() -> int {
            lilka::State state = lilka::controller.getState();
            hw_set_button(BTN_LEFT, state.left.pressed ? BTN_STATE_PRESSED : BTN_STATE_RELEASED);
            hw_set_button(BTN_MIDDLE, state.a.pressed ? BTN_STATE_PRESSED : BTN_STATE_RELEASED);
            hw_set_button(BTN_RIGHT, state.right.pressed ? BTN_STATE_PRESSED : BTN_STATE_RELEASED);
            return 0;
        },
    };

    tamalib_register_hal(&hal);
    tamalib_set_framerate(5);
    tamalib_init(1000000);

    loadState();

    int64_t select_press_started = 0;

    while (lilka::controller.getState().any.pressed) {
    }

    while (1) {
        lilka::State state = lilka::controller.getState();
        if (state.b.pressed) {
            // Save state & exit
            saveState();
            break;
        }

        if (state.select.justPressed) {
            select_press_started = millis();
        } else if (state.select.justReleased) {
            select_press_started = 0;
        }

        if (select_press_started && millis() - select_press_started > 1000) {
            // Reset state
            resetState();
            break;
        }

        tamalib_mainloop_step_by_step();
    }

    lilka::buzzer.stop();
}

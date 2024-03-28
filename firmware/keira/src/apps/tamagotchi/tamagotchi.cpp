#include "tamagotchi.h"
#include "tamalib.h"
#include "bitmaps.h"

static uint8_t matrix_buffer[LCD_HEIGHT][LCD_WIDTH] = {0};
static uint8_t icon_buffer[ICON_NUM] = {0};
static uint16_t current_freq = 0;

TamagotchiApp* TamagotchiApp::instance = nullptr;

TamagotchiApp::TamagotchiApp() : App("Tamagotchi") {
    TamagotchiApp::instance = this;
}

void TamagotchiApp::drawTriangle(uint8_t x, uint8_t y) {
    // display.drawLine(x,y,x+6,y);
    canvas->drawLine(x + 1, y + 1, x + 5, y + 1, lilka::colors::White);
    canvas->drawLine(x + 2, y + 2, x + 4, y + 2, lilka::colors::White);
    canvas->drawLine(x + 3, y + 3, x + 3, y + 3, lilka::colors::White);
}

// void TamagotchiApp::drawTamaRow(uint8_t tamaLCD_y, uint8_t ActualLCD_y, uint8_t thick) {
//     uint8_t i;
//     for (i = 0; i < LCD_WIDTH; i++) {
//         uint8_t mask = 0b10000000;
//         mask = mask >> (i % 8);
//         if ((matrix_buffer[tamaLCD_y][i / 8] & mask) != 0) {
//             canvas->drawRect(i + i + i + 16, ActualLCD_y, 2, thick, lilka::colors::White);
//         }
//     }
// }

void hal_log(log_level_t level, char* buf, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, buf);
    vsnprintf(buffer, sizeof(buffer), buf, args);
    va_end(args);

    lilka::serial_log("[tamagotchi] %s", buffer);
}

void TamagotchiApp::drawTamaSelection(uint8_t y) {
    uint8_t i;
    for (i = 0; i < 7; i++) {
        if (icon_buffer[i]) drawTriangle(i * 16 + 5, y);
        canvas->drawXBitmap(i * 16 + 4, y + 6, bitmaps + i * 18, 16, 9, lilka::colors::White);
    }
    if (icon_buffer[7]) {
        drawTriangle(7 * 16 + 5, y);
        canvas->drawXBitmap(7 * 16 + 4, y + 6, bitmaps + 7 * 18, 16, 9, lilka::colors::White);
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
                    if (matrix_buffer[y][x / 8] & (0x80 >> (x % 8))) {
                        canvas->drawPixel(x, y, lilka::colors::White);
                    }
                }
            }
            instance->drawTamaSelection(49);
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
    tamalib_set_framerate(3);
    tamalib_init(1000000);

    while (1) {
        if (lilka::controller.getState().b.pressed) {
            // TODO - save state?
            return;
        }

        tamalib_mainloop_step_by_step();
    }
}

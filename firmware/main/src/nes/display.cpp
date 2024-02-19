extern "C" {
#include <nes/nes.h>
}

#include <Arduino_GFX_Library.h>

#include "hw_config.h"
#include "lilka/display.h"

static int16_t w, h, frame_x, frame_y, frame_x_offset, frame_width, frame_height, frame_line_pixels;
extern int16_t bg_color;
extern uint16_t myPalette[];

static int64_t last_render = 0;
static int64_t last_frame_duration = 0;

extern void display_begin() {}

extern "C" void display_init() {
    w = lilka::display.width();
    h = lilka::display.height();
    nofrendo_log_printf("display w: %d, h: %d\n", w, h);
    if (w < 480) // assume only 240x240 or 320x240
    {
        if (w > NES_SCREEN_WIDTH) {
            frame_x = (w - NES_SCREEN_WIDTH) / 2;
            frame_x_offset = 0;
            frame_width = NES_SCREEN_WIDTH;
            frame_height = NES_SCREEN_HEIGHT;
            frame_line_pixels = frame_width;
        } else {
            frame_x = 0;
            frame_x_offset = (NES_SCREEN_WIDTH - w) / 2;
            frame_width = w;
            frame_height = NES_SCREEN_HEIGHT;
            frame_line_pixels = frame_width;
        }
        frame_y = (lilka::display.height() - NES_SCREEN_HEIGHT) / 2;
    } else // assume 480x320
    {
        frame_x = 0;
        frame_y = 0;
        frame_x_offset = 8;
        frame_width = w;
        frame_height = h;
        frame_line_pixels = frame_width / 2;
    }
}

extern "C" void display_write_frame(const uint8_t *data[]) {
    last_frame_duration = micros() - last_render;
    last_render = micros();

    lilka::display.startWrite();
    lilka::display.writeAddrWindow(frame_x, frame_y, frame_width, frame_height);
    // nofrendo_log_printf("Start\n");
    for (int32_t i = 0; i < NES_SCREEN_HEIGHT; i++) {
        lilka::display.writeIndexedPixels((uint8_t *)(data[i] + frame_x_offset), myPalette, frame_line_pixels);
    }
    lilka::display.endWrite();
    if (last_frame_duration > 0) {
        lilka::display.fillRect(80, LILKA_DISPLAY_HEIGHT - 20, 80, 20, lilka::display.color565(0, 0, 0));
        lilka::display.setCursor(80, LILKA_DISPLAY_HEIGHT - 4);
        lilka::display.setTextSize(1);
        lilka::display.setTextColor(lilka::display.color565(128, 128, 128));
        lilka::display.print("FPS: ");
        lilka::display.print(1000000 / last_frame_duration);
    }
}

extern "C" void display_clear() {
    // Arduino_TFT *gfx = lilka_display_get();
    lilka::display.fillScreen(lilka::display.color565(0, 0, 0));
}

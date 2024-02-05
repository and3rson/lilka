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
    Arduino_TFT *gfx = lilka_display_get();
    w = gfx->width();
    h = gfx->height();
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
        frame_y = (gfx->height() - NES_SCREEN_HEIGHT) / 2;
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
    // nofrendo_log_printf("display_write_frame\n");
    // gfx->fillScreen(gfx->color565(0, 0, 255));
    // gfx->drawPixel(120, 140, gfx->color565(255, 255, 255));
    Arduino_TFT *gfx = lilka_display_get();

    last_frame_duration = micros() - last_render;
    last_render = micros();

    // gfx->drawLine(0, 0, 280, 240, gfx->color565(255, 0, 0));
    // bus = new Arduino_ESP32SPI(3 /* DC */, 21 /* CS */, 0 /* SCK */, 1 /* MOSI */, -1);
    gfx->startWrite();
    if (w < 480) {
        // for (int32_t y = 0; y < frame_height; y++)
        // {
        //     for (int32_t x = 0; x < frame_line_pixels; x++)
        //     {
        //         // uint8_t index = (data[y][x + frame_x_offset]);
        //         // gfx->writeColor(myPalette[index], 1);
        //         // gfx->writePixel(x + frame_x, y + frame_y, myPalette[index]);
        //         nofrendo_log_printf("x: %d, y: %d\n", x, y);
        //         gfx->writePixel(x + 32, y + 32, rand() % 65536);
        //     }
        // }
        gfx->writeAddrWindow(frame_x, frame_y, frame_width, frame_height);
        // nofrendo_log_printf("Start\n");
        for (int32_t i = 0; i < NES_SCREEN_HEIGHT; i++) {
            // nofrendo_log_printf("line %d of %d: \n", i, NES_SCREEN_HEIGHT);
            gfx->writeIndexedPixels((uint8_t *)(data[i] + frame_x_offset), myPalette, frame_line_pixels);
        }
        // nofrendo_log_printf("End\n");
    }
    // gfx->endWrite();
    gfx->endWrite();
    if (last_frame_duration > 0) {
        gfx->fillRect(80, LILKA_DISPLAY_HEIGHT - 20, 80, 20, gfx->color565(0, 0, 0));
        // gfx->fillRect(0, 0, LILKA_DISPLAY_WIDTH, LILKA_DISPLAY_HEIGHT, gfx->color565(255, 0, 0));
        gfx->setCursor(80, LILKA_DISPLAY_HEIGHT - 4);
        gfx->setTextSize(1);
        gfx->setTextColor(gfx->color565(128, 128, 128));
        gfx->print("FPS: ");
        gfx->print(1000000 / last_frame_duration);
    }
}

extern "C" void display_clear() {
    Arduino_TFT *gfx = lilka_display_get();
    gfx->fillScreen(gfx->color565(0, 0, 0));
}

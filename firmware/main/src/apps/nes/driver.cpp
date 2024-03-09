#include "driver.h"

NesApp *Driver::app;
int16_t Driver::w, Driver::h, Driver::frame_x, Driver::frame_y, Driver::frame_x_offset, Driver::frame_width,
    Driver::frame_height, Driver::frame_line_pixels;
int64_t Driver::last_render = 0;
int64_t Driver::last_frame_duration = 0;

void Driver::setNesApp(NesApp *app) {
    Driver::app = app;
}

int Driver::init(int width, int height) {
    w = app->canvas->width();
    h = app->canvas->height();

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
        frame_y = (app->canvas->height() - NES_SCREEN_HEIGHT) / 2;
    } else // assume 480x320
    {
        frame_x = 0;
        frame_y = 0;
        frame_x_offset = 8;
        frame_width = w;
        frame_height = h;
        frame_line_pixels = frame_width / 2;
    }

    return 0;
}

void Driver::shutdown() {}

int Driver::setMode(int width, int height) {
    return 0;
}

void Driver::setPalette(rgb_t *pal) {
    uint16 c;

    int i;

    for (i = 0; i < 256; i++) {
        c = (pal[i].b >> 3) + ((pal[i].g >> 2) << 5) + ((pal[i].r >> 3) << 11);
        //myPalette[i]=(c>>8)|((c&0xff)<<8);
        nesPalette[i] = c;
    }
}

void Driver::clear(uint8 color) {
    app->canvas->fillScreen(0);
}

bitmap_t *Driver::lockWrite() {
    bitmap = bmp_createhw((uint8 *)fb, NES_SCREEN_WIDTH, NES_SCREEN_HEIGHT, NES_SCREEN_WIDTH * 2);
    return bitmap;
}

void Driver::freeFrite(int numDirties, rect_t *dirtyRects) {
    bmp_destroy(&bitmap);
}

void Driver::customBlit(bitmap_t *bmp, int numDirties, rect_t *dirtyRects) {
    last_frame_duration = micros() - last_render;
    last_render = micros();

    for (int y = 0; y < frame_height; y++) {
        uint8_t *line = bmp->line[y];
        for (int x = 0; x < frame_width; x++) {
            uint8_t index = line[x];
            uint16_t color = nesPalette[index];
            app->canvas->drawPixel(x + frame_x, y + frame_y, color);
        }
    }

    if (last_frame_duration > 0) {
        app->canvas->fillRect(80, LILKA_DISPLAY_HEIGHT - 20, 80, 20, app->canvas->color565(0, 0, 0));
        app->canvas->setCursor(80, LILKA_DISPLAY_HEIGHT - 4);
        app->canvas->setTextSize(1);
        app->canvas->setTextColor(app->canvas->color565(128, 128, 128));
        app->canvas->print("FPS: ");
        app->canvas->print(1000000 / last_frame_duration);
    }

    app->queueDraw();
}

char Driver::fb[1];
bitmap_t *Driver::bitmap;
uint16 Driver::nesPalette[256];
viddriver_t Driver::driver = {
    "Lilka", /* name */
    Driver::init, /* init */
    Driver::shutdown, /* shutdown */
    Driver::setMode, /* set_mode */
    Driver::setPalette, /* set_palette */
    Driver::clear, /* clear */
    Driver::lockWrite, /* lock_write */
    Driver::freeFrite, /* free_write */
    Driver::customBlit, /* custom_blit */
    false /* invalidate flag */
};

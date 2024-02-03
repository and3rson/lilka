#include "display.h"

#include "splash.h"

Arduino_DataBus* _bus = 0;
Arduino_ST7789* _gfx = 0;

Arduino_TFT* lilka_display_begin() {
    _bus = new Arduino_ESP32SPI(LILKA_DISPLAY_DC, LILKA_DISPLAY_CS, LILKA_DISPLAY_SCK, LILKA_DISPLAY_MOSI, -1);
    _gfx = new Arduino_ST7789(_bus, LILKA_DISPLAY_RST, LILKA_DISPLAY_ROTATION, true, LILKA_DISPLAY_WIDTH, LILKA_DISPLAY_HEIGHT);
    _gfx->begin();
    _gfx->fillScreen(_gfx->color565(0, 0, 0));
    uint16_t row[LILKA_DISPLAY_WIDTH];
    for (int i = 0; i <= 4; i++) {
        _gfx->startWrite();
        _gfx->writeAddrWindow(0, 0, 240, 280);
        for (int y = 0; y < LILKA_DISPLAY_HEIGHT; y++) {
            for (int x = 0; x < LILKA_DISPLAY_WIDTH; x++) {
                uint16_t color = splash[y * LILKA_DISPLAY_WIDTH + x];
                uint16_t r = ((color >> 11) & 0x1F) << 3;
                uint16_t g = ((color >> 5) & 0x3F) << 2;
                uint16_t b = (color & 0x1F) << 3;
                row[x] = _gfx->color565(r * i / 4, g * i / 4, b * i / 4);
            }
            _gfx->writePixels(row, LILKA_DISPLAY_WIDTH);
        }
        _gfx->endWrite();
    }
    delay(800);
    for (int i = 4; i >= 0; i--) {
        _gfx->startWrite();
        _gfx->writeAddrWindow(0, 0, 240, 280);
        for (int y = 0; y < LILKA_DISPLAY_HEIGHT; y++) {
            for (int x = 0; x < LILKA_DISPLAY_WIDTH; x++) {
                uint16_t color = splash[y * LILKA_DISPLAY_WIDTH + x];
                uint16_t r = ((color >> 11) & 0x1F) << 3;
                uint16_t g = ((color >> 5) & 0x3F) << 2;
                uint16_t b = (color & 0x1F) << 3;
                row[x] = _gfx->color565(r * i / 4, g * i / 4, b * i / 4);
            }
            _gfx->writePixels(row, LILKA_DISPLAY_WIDTH);
        }
        _gfx->endWrite();
    }
    return _gfx;
}

Arduino_TFT* lilka_display_get() {
    if (_gfx == 0) {
        lilka_display_begin();
    }
    return _gfx;
}

// Arduino_Canvas* lilka_display_create_canvas() {
//     Serial.println("Warning: Canvas is buggy. Do not use this function unless you know what you're doing.");
//     if (_gfx == 0) {
//         lilka_display_begin();
//     }
//     Arduino_Canvas* canvas = new Arduino_Canvas(LILKA_DISPLAY_WIDTH, LILKA_DISPLAY_HEIGHT, _gfx);
//     canvas->begin();
//     return canvas;
// }

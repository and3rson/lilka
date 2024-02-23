#include "display.h"

#include "splash.h"

#include "spi.h"
#include "serial.h"

namespace lilka {

#if LILKA_VERSION == 1
Arduino_ESP32SPI displayBus(LILKA_DISPLAY_DC, LILKA_DISPLAY_CS, LILKA_SPI_SCK, LILKA_SPI_MOSI);
#else
Arduino_ESP32SPI displayBus(
    LILKA_DISPLAY_DC, LILKA_DISPLAY_CS, LILKA_SPI_SCK, LILKA_SPI_MOSI, LILKA_SPI_MISO, SPI1_NUM
);
#endif

Display::Display()
    : Arduino_ST7789(
          &displayBus, LILKA_DISPLAY_RST, LILKA_DISPLAY_ROTATION, true, LILKA_DISPLAY_WIDTH, LILKA_DISPLAY_HEIGHT, 0, 20
      ) {}

void Display::begin() {
    serial_log("initializing display");
#ifdef LILKA_BREADBOARD
    Arduino_ST7789::begin(40000000);
#else
    Arduino_ST7789::begin(80000000);
#endif
    drawArc();
    setFont(u8g2_font_10x20_t_cyrillic);
    setUTF8Print(true);
    uint16_t row[LILKA_DISPLAY_WIDTH];
    for (int i = 0; i <= 4; i++) {
        startWrite();
        writeAddrWindow(0, 0, 240, 280);
        for (int y = 0; y < LILKA_DISPLAY_HEIGHT; y++) {
            for (int x = 0; x < LILKA_DISPLAY_WIDTH; x++) {
                uint16_t color = splash[y * LILKA_DISPLAY_WIDTH + x];
                uint16_t r = ((color >> 11) & 0x1F) << 3;
                uint16_t g = ((color >> 5) & 0x3F) << 2;
                uint16_t b = (color & 0x1F) << 3;
                row[x] = color565(r * i / 4, g * i / 4, b * i / 4);
            }
            writePixels(row, LILKA_DISPLAY_WIDTH);
        }
        endWrite();
    }
    delay(800);
    for (int i = 4; i >= 0; i--) {
        startWrite();
        writeAddrWindow(0, 0, 240, 280);
        for (int y = 0; y < LILKA_DISPLAY_HEIGHT; y++) {
            for (int x = 0; x < LILKA_DISPLAY_WIDTH; x++) {
                uint16_t color = splash[y * LILKA_DISPLAY_WIDTH + x];
                uint16_t r = ((color >> 11) & 0x1F) << 3;
                uint16_t g = ((color >> 5) & 0x3F) << 2;
                uint16_t b = (color & 0x1F) << 3;
                row[x] = color565(r * i / 4, g * i / 4, b * i / 4);
            }
            writePixels(row, LILKA_DISPLAY_WIDTH);
        }
        endWrite();
    }
    serial_log("display ok");
}

void Display::renderCanvas(Canvas &canvas) {
    draw16bitRGBBitmap(0, 0, canvas.getFramebuffer(), LILKA_DISPLAY_WIDTH, LILKA_DISPLAY_HEIGHT);
}

Canvas::Canvas() : Arduino_Canvas(LILKA_DISPLAY_WIDTH, LILKA_DISPLAY_HEIGHT, NULL) {
    setFont(u8g2_font_10x20_t_cyrillic);
    setUTF8Print(true);
}

Display display;

} // namespace lilka

#include "display.h"

#include "splash.h"

#include "serial.h"

namespace lilka {

LilkaBus::LilkaBus(int8_t dc, int8_t cs, int8_t sck, int8_t mosi, int8_t miso) : Arduino_ESP32SPI(dc, cs, sck, mosi, miso) {}

void LilkaBus::beginWrite() {
#if CONFIG_IDF_TARGET_ESP32
#    error "ESP32 is not supported"
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
#    error "TODO"
#else
    SPIClass *spi = &SPI;
#endif

    // https://github.com/moononournation/Arduino_GFX/issues/433
    // https://github.com/espressif/arduino-esp32/issues/9221
    spi->beginTransaction(SPISettings(SPI_DEFAULT_FREQ, SPI_MSBFIRST, SPI_MODE3));
    spi->write(0xFF);
    spi->endTransaction();

    Arduino_ESP32SPI::beginWrite();
}

LilkaBus lilkaBus(LILKA_DISPLAY_DC, LILKA_DISPLAY_CS, LILKA_SPI_SCK, LILKA_SPI_MOSI, LILKA_SPI_MISO);

Display::Display() : Arduino_ST7789(&lilkaBus, LILKA_DISPLAY_RST, LILKA_DISPLAY_ROTATION, true, LILKA_DISPLAY_WIDTH, LILKA_DISPLAY_HEIGHT, 0, 20) {}

void Display::begin() {
    serial_log("initializing display");
    Arduino_ST7789::begin();
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

Display display;

} // namespace lilka

#ifndef LILKA_DISPLAY_H
#define LILKA_DISPLAY_H

#include "config.h"

#include <Arduino_GFX_Library.h>
#include <U8g2lib.h>

namespace lilka {

class LilkaBus : public Arduino_ESP32SPI {
    // https://github.com/moononournation/Arduino_GFX/issues/433
    // https://github.com/espressif/arduino-esp32/issues/9221
public:
    LilkaBus(int8_t dc, int8_t cs, int8_t sck, int8_t mosi, int8_t miso);
    void beginWrite();
};

class Display : public Arduino_ST7789 {
public:
    Display();
    void begin();
};

extern Display display;

} // namespace lilka

#endif // LILKA_DISPLAY_H

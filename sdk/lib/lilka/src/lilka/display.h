#ifndef LILKA_DISPLAY_H
#define LILKA_DISPLAY_H

#include "config.h"

#include <Arduino_GFX_Library.h>
#include <U8g2lib.h>

namespace lilka {

class Canvas;

class Display : public Arduino_ST7789 {
public:
    Display();
    void begin();
    void renderCanvas(Canvas &canvas);
};

class Canvas : public Arduino_Canvas {
public:
    Canvas();
};

extern Display display;

} // namespace lilka

#endif // LILKA_DISPLAY_H

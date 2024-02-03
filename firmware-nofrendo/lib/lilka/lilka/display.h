#ifndef LILKA_DISPLAY_H
#define LILKA_DISPLAY_H

#include "config.h"

#include <Arduino_GFX_Library.h>

extern Arduino_DataBus* _bus;
extern Arduino_ST7789* _gfx;

Arduino_TFT* lilka_display_begin();
Arduino_TFT* lilka_display_get();
// Arduino_Canvas* lilka_display_create_canvas();

#endif // LILKA_DISPLAY_H

#ifndef LILKA_H
#define LILKA_H

#include "lilka/board.h"
#include "lilka/serial.h"
#include "lilka/spi.h"
#include "lilka/controller.h"
#include "lilka/display.h"
#include "lilka/fileutils.h"
#include "lilka/battery.h"
#include "lilka/buzzer.h"
#include "lilka/ui.h"
#include "lilka/multiboot.h"
#include "lilka/sys.h"
#include "lilka/resources.h"
#include "lilka/fmath.h"
#include "lilka/audio.h"
#include "lilka/ble_server.h"

namespace lilka {
/// Ініціалізація Лілки
///
/// Ініціалізує всі підсистеми Лілки - дисплей, кнопки, файлову систему, SD-карту, батарею, п'єзо-динамік і т.д.
///
/// Рекомендується викликати цю функцію один раз на початку програми в вашій функції ``setup()``.
void begin();
} // namespace lilka

#endif // LILKA_H

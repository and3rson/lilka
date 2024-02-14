#ifndef LILKA_UI_H
#define LILKA_UI_H

#include <Arduino.h>

#include "icons/icon.h"

namespace lilka {

int ui_menu(String title, String menu[], int menu_size, int cursor = 0, const menu_icon_t *icons[] = 0);
void ui_alert(String title, String message);

} // namespace lilka

#endif // LILKA_UI_H

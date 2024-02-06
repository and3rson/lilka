#ifndef LILKA_UI_H
#define LILKA_UI_H

#include <Arduino.h>

namespace lilka {

int ui_menu(String title, String menu[], int menu_size, int cursor);
void ui_alert(String title, String message);

} // namespace lilka

#endif // LILKA_UI_H

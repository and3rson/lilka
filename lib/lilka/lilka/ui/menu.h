#ifndef LILKA_MENU_H
#define LILKA_MENU_H

#include <Arduino.h>

namespace lilka {

int ui_menu(String title, String menu[], int menu_size, int cursor);

} // namespace lilka

#endif // LILKA_MENU_H

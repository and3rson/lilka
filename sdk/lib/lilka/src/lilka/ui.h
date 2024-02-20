#ifndef LILKA_UI_H
#define LILKA_UI_H

#include <Arduino.h>

#include "icons/icon.h"

namespace lilka {

/// Відобразити меню.
/// Повертає індекс вибраного пункту меню.
///
/// @param title Заголовок меню.
/// @param menu Масив пунктів меню.
/// @param menu_size Розмір масиву пунктів меню.
/// @param cursor Початковий курсор. За замовчуванням встановлено в 0.
/// @param icons Масив іконок для кожного пункту меню. Якщо значення елементу (або весь масив) буде рівним 0, то іконка не відображається.
int ui_menu(String title, String menu[], int menu_size, int cursor = 0, const menu_icon_t *icons[] = 0);
/// Відобразити підтвердження.
///
/// @param title Заголовок підтвердження.
/// @param message Повідомлення підтвердження.
void ui_alert(String title, String message);

} // namespace lilka

#endif // LILKA_UI_H

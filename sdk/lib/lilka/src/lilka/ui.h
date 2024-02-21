#ifndef LILKA_UI_H
#define LILKA_UI_H

#include <Arduino.h>

#include "icons/icon.h"

namespace lilka {

/// Відобразити меню.
///
/// Блокує виконання програми, доки користувач не обере пункт меню та не натисне кнопку A.
/// Повертає індекс вибраного пункту меню.
///
/// @param title Заголовок меню.
/// @param menu Масив пунктів меню.
/// @param menu_size Розмір масиву пунктів меню.
/// @param cursor Початковий курсор. За замовчуванням встановлено в 0.
/// @param icons Масив іконок для кожного пункту меню. Якщо значення елементу (або весь масив) буде рівним 0, то іконка не відображається.
/// @return Індекс вибраного пункту меню.
///
/// Приклад використання:
///
/// @code
/// #include <lilka.h>
///
/// void setup() {
///     lilka::begin();
/// }
///
/// void loop() {
///     String items[] = {"Смерть русні", "Ядерка на червону площу", "Повернення Криму"};
///     int selected = lilka::ui_menu("Оберіть щось", items, 3);
///     lilka::ui_alert(
///         "Гарний вибір!",
///         "Ви обрали " + menu[selected] + "\nДавайте оберемо ще щось!"
///     );
/// }
/// @endcode

int ui_menu(String title, String menu[], int menu_size, int cursor = 0, const menu_icon_t *icons[] = 0);
/// Відобразити сповіщення.
///
/// Блокує виконання програми, доки користувач не натисне кнопку A.
///
/// @param title Заголовок підтвердження.
/// @param message Повідомлення підтвердження.
///
/// Приклад використання:
///
/// @code
/// #include <lilka.h>
///
/// void setup() {
///     lilka::begin();
/// }
///
/// void loop() {
///     lilka::ui_alert("Увага", "Повітряна тривога в москві, загроза балістичних ракет!");
/// }
/// @endcode
void ui_alert(String title, String message);

} // namespace lilka

#endif // LILKA_UI_H

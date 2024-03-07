#ifndef LILKA_UI_H
#define LILKA_UI_H

#include <Arduino.h>
#include <vector>
#include "Arduino_GFX.h"
#include "display.h"

typedef uint16_t const menu_icon_t[576]; // 24x24px icon

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
/// @param colors Масив кольорів для кожного пункту меню (в форматі RGB 5-6-5). Якщо значення елементу (або весь масив) буде рівним 0, то використовується стандартний колір.
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

// int ui_menu(
//     Canvas *canvas, String title, String menu[], int menu_size, int cursor = 0, const menu_icon_t *icons[] = 0,
//     const uint16_t colors[] = 0
// );
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
// void ui_alert(Canvas *canvas, String title, String message);

typedef struct {
    String title;
    const menu_icon_t *icon;
    uint16_t color;
} MenuItem;

class Menu {
public:
    Menu(String title);
    void addItem(String title, const menu_icon_t *icon = 0, uint16_t color = 0);
    void setCursor(int16_t cursor);
    void update();
    void draw(Arduino_GFX *canvas);
    int16_t getSelectedIndex();

private:
    int16_t cursor;
    int16_t scroll;
    String title;
    std::vector<MenuItem> items;
    int16_t selectedIndex;
};

class Alert {
public:
    Alert(String title, String message);
    void update();
    void draw(Arduino_GFX*canvas);
    bool isDone();

private:
    String title;
    String message;
    bool done;
};

} // namespace lilka

#endif // LILKA_UI_H

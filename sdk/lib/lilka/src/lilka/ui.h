#ifndef LILKA_UI_H
#define LILKA_UI_H

#include <Arduino.h>
#include <vector>
#include "Arduino_GFX.h"
#include "display.h"
#include "controller.h"

typedef uint16_t const menu_icon_t[576]; // 24x24px icon

namespace lilka {

typedef struct {
    String title;
    const menu_icon_t* icon;
    uint16_t color;
} MenuItem;

/// Клас для відображення меню.
///
/// Дозволяє відобразити меню з пунктами, які можна вибирати за допомогою стрілок вгору/вниз та підтвердити вибір кнопкою A.
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
///     lilka::Menu dreams("Оберіть щось");
///     dreams.addItem("Смерть русні");
///     dreams.addItem("Ядерка на червону площу");
///     dreams.addItem("Повернення Криму");
///     int index = -1;
///     while (index == -1) {
///         dreams.update();
///         dreams.draw(&lilka::display);
///         index = dreams.getSelectedIndex();
///     }
///     Serial.println("Ви обрали пункт номер " + String(index));
/// }
/// @endcode
class Menu {
public:
    /// Конструктор класу.
    ///
    /// @param title Заголовок меню.
    explicit Menu(String title);
    /// Додати пункт до меню.
    /// @param title Заголовок пункту.
    /// @param icon Іконка пункту (масив з ``uint16_t`` розміром 576 елементів, який представляє 24x24px зображення). За замовчуванням ``0`` (відсутня іконка).
    /// @param color Колір пункту. За замовчуванням ``0`` (стандартний колір).
    void addItem(String title, const menu_icon_t* icon = 0, uint16_t color = 0);
    /// Встановити курсор на пункт меню.
    /// @param cursor Індекс пункту меню.
    void setCursor(int16_t cursor);
    /// Оновити стан меню.
    ///
    /// Цю функцію потрібно викликати, щоб меню опрацювало вхідні дані від користувача та оновило свій стан.
    void update();
    /// Намалювати меню на Display або Canvas.
    ///
    /// @param canvas Вказівник на Display або Canvas, на якому потрібно намалювати меню.
    ///
    /// Приклад використання:
    /// @code
    /// // ...
    /// menu.draw(&lilka::display); // намалювати меню на Display
    /// menu.draw(&canvas); // намалювати меню на Canvas
    /// // ...
    /// @endcode
    void draw(Arduino_GFX* canvas);
    /// Перевірити, чи обрано пункт меню.
    ///
    /// Якщо пункт обрано (користувач натиснув кнопку "A"), повертається ``true``, інакше ``false``. Після виклику цієї функції пункт перестає бути обраним.
    bool isFinished();
    /// Отримати індекс обраного пункту меню.
    int16_t getCursor();

private:
    int16_t cursor;
    int16_t scroll;
    String title;
    std::vector<MenuItem> items;
    bool done;
};

/// Клас для відображення сповіщення.
///
/// Дозволяє відобразити сповіщення, яке користувач може закрити кнопкою A.
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
///     lilka::Alert warning("Увага", "Повітряна тривога в москві, загроза балістичних ракет!");
///     warning.draw(&lilka::display);
///     while (!warning.isFinished()) {
///         warning.update();
///     }
/// }
/// @endcode
class Alert {
public:
    /// Конструктор класу.
    ///
    /// @param title Заголовок сповіщення.
    /// @param message Повідомлення сповіщення.
    Alert(String title, String message);
    /// Змінити заголовок сповіщення.
    /// @param title Новий заголовок сповіщення.
    void setTitle(String title);
    /// Змінити повідомлення сповіщення.
    /// @param message Нове повідомлення сповіщення.
    void setMessage(String message);
    /// Оновити стан сповіщення.
    ///
    /// Цю функцію потрібно викликати, щоб сповіщення опрацювало вхідні дані від користувача та оновило свій стан.
    void update();
    /// Намалювати сповіщення на Display або Canvas.
    ///
    /// @param canvas Вказівник на Display або Canvas, на якому потрібно намалювати сповіщення.
    ///
    /// Приклад використання:
    ///
    /// @code
    /// // ...
    /// alert.draw(&lilka::display); // намалювати сповіщення на Display
    /// alert.draw(&canvas); // намалювати сповіщення на Canvas
    /// // ...
    /// @endcode
    void draw(Arduino_GFX* canvas);
    /// Перевірити, чи користувач закрив сповіщення.
    ///
    /// Якщо сповіщення закрито (користувач натиснув кнопку "A" або "Start"), повертається ``true``, інакше ``false``.
    bool isFinished();
    /// Отримати кнопку, якою користувач закрив сповіщення.
    ///
    /// Якщо сповіщення не закрито, результат буде невизначеним. Рекомендується використовувати цю функцію тільки після того, як ``isFinished()`` поверне ``true``.
    Button getButton();

private:
    String title;
    String message;
    bool done;
    Button button;
};

/// Клас для відображення індикатора виконання.
///
/// Дозволяє відобразити індикатор виконання, який можна оновлювати та змінювати його повідомлення.
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
///     lilka::ProgressDialog progress("Почекайте", "Завантаження даних...");
///     for (int i = 0; i <= 100; i++) {
///         progress.setProgress(i);
///         progress.draw(&lilka::display);
///         delay(100);
///     }
///     progress.setMessage("Дані завантажено!");
///     delay(1000);
/// }
/// @endcode
class ProgressDialog {
public:
    /// Конструктор класу.
    ///
    /// @param title Заголовок індикатора виконання.
    /// @param message Повідомлення індикатора виконання.
    ProgressDialog(String title, String message);
    /// Встановити прогрес виконання.
    ///
    /// @param progress Прогрес виконання від 0 до 100.
    void setProgress(int16_t progress);
    /// Встановити повідомлення.
    /// @param message Повідомлення.
    void setMessage(String message);
    /// Намалювати індикатор виконання на Display або Canvas.
    /// @param canvas Вказівник на Display або Canvas, на якому потрібно намалювати індикатор виконання.
    ///
    /// Приклад використання:
    ///
    /// @code
    /// // ...
    /// progress.draw(&lilka::display); // намалювати індикатор виконання на Display
    /// progress.draw(&canvas); // намалювати індикатор виконання на Canvas
    /// // ...
    /// @endcode
    void draw(Arduino_GFX* canvas);

private:
    String title;
    String message;
    int16_t progress;
};

/// Клас для відображення діалогового вікна введення.
///
/// Малює вікно введення та екранну клавіатуру, дозволяє вводити текст та підтверджувати введення.
class InputDialog {
public:
    explicit InputDialog(String title);
    void setMasked(bool masked);
    void setValue(String value);
    void update();
    void draw(Arduino_GFX* canvas);
    bool isFinished();
    String getValue();

private:
    void resetBlink();

    String title;
    bool masked;
    String value;
    bool done;

    int16_t layer;
    int16_t cx;
    int16_t cy;
    int64_t lastBlink;
    bool blinkPhase;
};

} // namespace lilka

#endif // LILKA_UI_H

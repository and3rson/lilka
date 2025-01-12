#ifndef LILKA_UI_H
#define LILKA_UI_H

#include <Arduino.h>
#include <vector>
#include "Arduino_GFX.h"
#include "display.h"
#include "controller.h"

#define LILKA_UI_UPDATE_DELAY_MS 10

constexpr uint16_t menu_icon_width = 24;
constexpr uint16_t menu_icon_height = 24;
typedef uint16_t const menu_icon_t[menu_icon_width * menu_icon_height]; // 24x24px icon (576*2 bytes)

namespace lilka {

typedef void (*PMenuItemCallback)(void*);
typedef struct {
    String title;
    const menu_icon_t* icon;
    uint16_t color;
    String postfix;
    PMenuItemCallback callback;
    void* callbackData; // place ptr to Menu inside callback Data if u want to track menu.getButton()
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
///     while (!dreams.isFinished()) {
///         dreams.update();
///         dreams.draw(&lilka::display);
///     }
///     int index = dreams.getCursor();
///     lilka::MenuItem item;
///     dreams.getItem(index, &item);
///     Serial.println(String("Ви обрали пункт ") + item.title);
/// }
/// @endcode
class Menu {
public:
    /// Конструктор класу.
    ///
    /// @param title Заголовок меню.
    explicit Menu(String title = "Меню");

    ~Menu();

    /// Встановити новий заголовок меню
    ///
    /// @param title Заголовок меню.
    void setTitle(String title);

    /// Додати пункт до меню.
    ///
    /// @param title Заголовок пункту.
    /// @param icon Іконка пункту (масив з `uint16_t` розміром 576 елементів, який представляє 24x24px зображення). За замовчуванням `0` (відсутня іконка).
    /// @param color Колір пункту. За замовчуванням `0` (стандартний колір).
    /// @param postfix Текст, який додається після заголовка пункту і вирівнюється до правого краю меню.
    /// @param callback Вказівник на функцію яку буде викликано при закритті меню.
    /// @param callbackData Дані які буде передано в callback функцію
    void addItem(
        String title, const menu_icon_t* icon = 0, uint16_t color = 0, String postfix = "",
        PMenuItemCallback callback = NULL, void* callbackData = NULL
    );

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
    /// Якщо пункт обрано, тобто користувач натиснув кнопку "A" (або іншу кнопку, яка була додана за допомогою `addActivationButton()`), повертається `true`, інакше `false`.
    bool isFinished();

    /// Змінити пункт меню
    /// @param index Індекс пункту.
    /// @param title Заголовок пункту.
    /// @param icon Іконка пункту (масив з `uint16_t` розміром 576 елементів, який представляє 24x24px зображення). За замовчуванням `0` (відсутня іконка).
    /// @param color Колір пункту. За замовчуванням `0` (стандартний колір).
    /// @param postfix Текст, який додається після заголовка пункту і вирівнюється до правого краю меню.
    ///
    /// Повертає значення true, якщо пункт було змінено
    bool setItem(int16_t index, String title, const menu_icon_t* icon = 0, uint16_t color = 0, String postfix = "");
    /// Отримати пункт меню
    /// @param index Індекс пункту.
    /// @param menuItem Вказівник на lilka::MenuItem куди буде скопійовано пункт
    /// Повертає true у разі успіху
    bool getItem(int16_t index, MenuItem* menuItem);
    /// Отримати індекс обраного пункту меню.
    int16_t getCursor();
    /// Очистити меню і зробити його доступним для повторного використання
    void clearItems();
    /// Отримати кількість пунктів меню.
    /// @return Кількість пунктів меню.
    int16_t getItemCount();
    /// Дозволити вибір пункту меню за допомогою інших кнопок.
    ///
    /// За замовчуванням вибір пункту можливий тільки за допомогою кнопки "A". Після виклику цієї функції можна вибирати пункт за допомогою додаткових кнопок.
    /// @see getButton
    void addActivationButton(Button activationButton);
    /// Прибрати кнопку активації меню
    ///
    /// @see getButton
    void removeActivationButton(Button activationButton);
    /// Отримати кнопку, якою користувач обрав пункт меню.
    ///
    /// Якщо пункт не обрано, результат буде невизначеним. Рекомендується використовувати цю функцію тільки після того, як isFinished() поверне `true`.
    Button getButton();

private:
    int16_t cursor;
    int16_t scroll;
    String title;
    std::vector<MenuItem> items;
    bool done;
    Image* iconImage;
    Canvas* iconCanvas;
    int64_t lastCursorMove;
    int64_t firstRender;
    std::vector<Button> activationButtons;
    Button button;
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
    /// Якщо сповіщення закрито, тобто користувач натиснув кнопку "A" (або іншу кнопку, яка була додана за допомогою `addActivationButton()`), повертається `true`, інакше `false`.
    bool isFinished();
    /// Дозволити закриття сповіщення за допомогою інших кнопок.
    ///
    /// За замовчуванням закриття сповіщення можливе тільки за допомогою кнопки "A". Після виклику цієї функції можна закривати сповіщення за допомогою додаткових кнопок.
    /// @see getButton
    void addActivationButton(Button activationButton);
    /// Отримати кнопку, якою користувач закрив сповіщення.
    ///
    /// Якщо сповіщення не закрито, результат буде невизначеним. Рекомендується використовувати цю функцію тільки після того, як isFinished() поверне `true`.
    Button getButton();

private:
    String title;
    String message;
    bool done;
    Button button;
    std::vector<Button> activationButtons;
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
///
/// Приклад використання:
///
/// @code
/// InputDialog dialog("Введіть пароль");
/// dialog.setMasked(true);
/// dialog.setValue("1234");
/// while (!dialog.isFinished()) {
///     dialog.update();
///     dialog.draw(&lilka::display);
/// }
/// String password = dialog.getValue();
/// @endcode
class InputDialog {
public:
    /// Конструктор класу.
    ///
    /// @param title Заголовок діалогового вікна введення.
    explicit InputDialog(String title);
    /// Встановити маскування введеного тексту. Якщо встановлено `true`, введений текст буде відображатися як зірочки.
    /// @param masked Чи маскувати введений текст.
    void setMasked(bool masked);
    /// Встановити початкове значення введеного тексту.
    /// @param value Текст.
    void setValue(String value);
    /// Оновити стан діалогового вікна введення.
    ///
    /// Цю функцію потрібно викликати, щоб діалогове вікно введення опрацювало вхідні дані від користувача та оновило свій стан.
    void update();
    /// Намалювати діалогове вікно введення на Display або Canvas.
    ///
    /// @param canvas Вказівник на Display або Canvas, на якому потрібно намалювати діалогове вікно введення.
    void draw(Arduino_GFX* canvas);
    /// Перевірити, чи користувач завершив введення тексту.
    ///
    /// Якщо введення завершено, тобто користувач натиснув кнопку "START", повертається `true`, інакше `false`.
    bool isFinished();
    String getValue();

private:
    void resetBlink();
    String unicodeToString(int16_t unicode);
    void removeLastChar();

    String title;
    bool masked;
    String value;
    bool done;

    int16_t layer;
    int16_t language;
    int16_t cx;
    int16_t cy;
    int64_t lastBlink;
    bool blinkPhase;
};

} // namespace lilka

#endif // LILKA_UI_H

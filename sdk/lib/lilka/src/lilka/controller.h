#ifndef LILKA_CONTROLLER_H
#define LILKA_CONTROLLER_H

#include "config.h"

#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

namespace lilka {

typedef enum {
    UP = 0,
    DOWN,
    LEFT,
    RIGHT,
    A,
    B,
    C,
    D,
    SELECT,
    START,
    ANY,
    COUNT,
} Button;

#define LILKA_DEBOUNCE_TIME 10 // 10ms

/// Містить стан кнопки, який був вимінярий в певний момент часу.
typedef struct {
    /// `true`, якщо кнопка була в натиснутому стані в момент виклику `lilka::controller.getState()`.
    bool pressed;
    /// `true`, якщо кнопка була вперше натиснута в момент виклику `lilka::controller.getState()` (до цього була відпущена).
    bool justPressed;
    /// `true`, якщо кнопка була вперше відпущена в момент виклику `lilka::controller.getState()` (до цього була натиснута).
    bool justReleased;
    uint64_t time;
    uint64_t nextRepeatTime;
    uint32_t repeatRate;
    uint32_t repeatDelay;
} ButtonState;

/// Містить стани всіх кнопок, які були виміряні в певний момент часу.
typedef struct {
    ButtonState up;
    ButtonState down;
    ButtonState left;
    ButtonState right;
    ButtonState a;
    ButtonState b;
    ButtonState c;
    ButtonState d;
    ButtonState select;
    ButtonState start;
    /// Спеціальний стан, який містить стани "будь-якої" кнопки.
    ButtonState any;
} State;

// Так, так, тут колись був union... Але doxygen + breathe не люблять union, які містять анонімні структури. :(
typedef ButtonState _StateButtons[Button::COUNT];

/// Клас для роботи з контролером.
///
/// Використовується для вимірювання стану кнопок.
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
///     while (1) {
///         lilka::State state = lilka::controller.getState();
///         if (state.up.justPressed) {
///             Serial.println("Ви щойно натиснули кнопку 'Вгору'");
///         } else if (state.up.justReleased) {
///             Serial.println("Ви щойно відпустили кнопку 'Вгору'");
///         }
///     }
/// }
/// @endcode
class Controller {
public:
    Controller();
    /// Почати вимірювання стану кнопок.
    /// \warning Цей метод викликається автоматично при виклику `lilka::begin()`.
    void begin();
    /// Прочитати стан кнопок та скинути прапорці `justPressed` та `justReleased`.
    State getState();
    /// Прочитати стан кнопок, не скидаючи прапорців `justPressed` та `justReleased`.
    State peekState();
    void resetState();
    /// Встановити глобальний обробник подій, який буде викликатися при натисненні або відпусканні будь-якої кнопки.
    void setGlobalHandler(void (*handler)(Button, bool));
    /// Встановити обробник подій для певної кнопки, який буде викликатися при натисненні або відпусканні цієї кнопки.
    void setHandler(Button button, void (*handler)(bool));
    /// Видалити всі обробники подій.
    void clearHandlers();
    /// Налаштувати автоматичне повторення натискання кнопки.
    ///
    /// Після виклику цього методу кнопка буде автоматично натискатися з певною затримкою та частотою.
    ///
    /// Щоб вимкнути автоматичне повторення натискання кнопки, викличте цей метод з параметрами `delay = 0` та `rate = 0`.
    ///
    /// \param button Кнопка, для якої налаштовується автоматичне повторення натискання.
    /// \param rate Частота автоматичного повторення натискання (кількість натискань на секунду).
    /// \param delay Затримка перед початком автоматичного повторення натискання (в мілісекундах).
    /// \code
    /// // Натискання кнопки "Вгору" буде повторюватись з частотою 5 натискань на секунду після початкової затримки 500 мс:
    /// lilka::controller.setAutoRepeat(lilka::Button::UP, 5, 500);
    /// \endcode
    void setAutoRepeat(Button button, uint32_t rate, uint32_t delay);

private:
    // Input task FreeRTOS semaphore
    SemaphoreHandle_t semaphore;
    void inputTask();
    State state;
    int8_t pins[Button::COUNT] = {
        LILKA_GPIO_UP,
        LILKA_GPIO_DOWN,
        LILKA_GPIO_LEFT,
        LILKA_GPIO_RIGHT,
        LILKA_GPIO_A,
        LILKA_GPIO_B,
        LILKA_GPIO_C,
        LILKA_GPIO_D,
        LILKA_GPIO_SELECT,
        LILKA_GPIO_START,
        -1,
    };
    void (*handlers[Button::COUNT])(bool);
    void (*globalHandler)(Button, bool);
    int8_t _seq[10] = {UP, UP, DOWN, DOWN, LEFT, RIGHT, LEFT, RIGHT, B, A};
    int8_t _seqIndex = 0;
};

/// Екземпляр класу `Controller`, який можна використовувати для вимірювання стану кнопок.
/// Вам не потрібно інстанціювати `Controller` вручну.
extern Controller controller;

} // namespace lilka

#endif // LILKA_CONTROLLER_H

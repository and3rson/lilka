#ifndef LILKA_DISPLAY_H
#define LILKA_DISPLAY_H

#include "config.h"

#include <Arduino_GFX_Library.h>
#include <U8g2lib.h>

namespace lilka {

class Canvas;

/// Клас для роботи з дисплеєм.
///
/// Використовується для відображення графічних об'єктів.
///
/// Приклад використання:
///
/// @code
/// #include <lilka.h>
///
/// void setup() {
///     lilka.begin();
/// }
///
/// void loop() {
///    lilka::display.fillScreen(lilka::display.color565(255, 0, 0)); // Заповнити екран червоним кольором
///    lilka::display.setCursor(32, 32);
///    lilka::display.setTextColor(lilka::display.color565(0, 255, 0)); // Зелений текст
///    lilka::display.print("Привіт, Лілка!");
/// }
/// @endcode
class Display : public Arduino_ST7789 {
public:
    Display();
    /// Почати роботу з дисплеєм.
    /// \warning Цей метод викликається автоматично при виклику `lilka::begin()`.
    void begin();
    /// Відобразити буфер на екрані (див. `lilka::Canvas`).
    void renderCanvas(Canvas &canvas);
};

/// Клас для роботи з графічним буфером.
///
/// При частому перемальовуванні екрану без використання буфера, може спостерігатися мерехтіння.
/// Наприклад, якщо використовувати метод `fillScreen` для очищення екрану перед кожним викликом `print`,
/// то текст буде мерехтіти.
///
/// Щоб уникнути цього, можна використовувати буфер. Цей клас дозволяє малювати графічні об'єкти на буфері,
/// а потім відобразити його на екрані за допомогою методу `lilka::display.renderCanvas`.
///
/// Такий підхід дозволяє зменшити мерехтіння, але збільшує використання пам'яті. Він називається "буферизація".
///
/// Приклад використання:
///
/// @code
/// #include <lilka.h>
///
/// void setup() {
///     lilka.begin();
/// }
///
/// void loop() {
///     lilka::Canvas canvas;
///     int y = 100;
///     while (1) {
///         canvas.fillScreen(lilka::display.color565(0, 0, 0)); // Заповнити буфер чорним кольором
///         canvas.setCursor(32, 0);
///         canvas.setTextColor(lilka::display.color565(0, 0, 0)); // Білий текст
///         canvas.print("Привіт, Лілка!");
///         lilka::display.renderCanvas(canvas); // Відобразити буфер на екрані - жодного мерехтіння!
///         y++;
///         if (y > 200) {
///             y = 100;
///         }
///     }
/// }
/// @endcode
class Canvas : public Arduino_Canvas {
public:
    Canvas();
};

extern Display display;

} // namespace lilka

#endif // LILKA_DISPLAY_H

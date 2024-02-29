#ifndef LILKA_DISPLAY_H
#define LILKA_DISPLAY_H

#include "config.h"

#include <Arduino_GFX_Library.h>
#include <U8g2lib.h>

namespace lilka {

// Рекомендовані шрифти для використання з дисплеєм.
#define FONT_4x6 u8g2_font_4x6_t_cyrillic
#define FONT_5x7 u8g2_font_5x7_t_cyrillic
#define FONT_5x8 u8g2_font_5x8_t_cyrillic
#define FONT_6x12 u8g2_font_6x12_t_cyrillic
#define FONT_6x13 u8g2_font_6x13_t_cyrillic
#define FONT_7x13 u8g2_font_7x13_t_cyrillic
#define FONT_8x13 u8g2_font_8x13_t_cyrillic
#define FONT_9x15 u8g2_font_9x15_t_cyrillic
#define FONT_10x20 u8g2_font_10x20_t_cyrillic

class Canvas;

/// Клас для роботи з дисплеєм.
///
/// Використовується для відображення графічних об'єктів.
///
/// Цей клас є підкласом `Arduino_GFX` з бібліотеки `Arduino_GFX_Library`.
/// Детальніше про доступні методи можна дізнатися в документації бібліотеки ``Arduino_GFX_Library`` -
/// https://github.com/moononournation/Arduino_GFX.
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
#ifdef DOXYGEN
    // `Arduino_GFX_Library` має купу гарних методів, але вони погано документовані.
    // Ця секція - лише для документації цих методів. Вона буде прочитана інструментом `doxygen` при генерації
    // документації.

    /// Перетворити RGB колір в 16-бітний формат.
    /// @param r Компонента R.
    /// @param g Компонента G.
    /// @param b Компонента B.
    ///
    /// Оскільки дисплей підтримує лише 16-бітні кольори, цей метод дозволяє перетворити 24-бітний колір в 16-бітний.
    uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
    /// Встановити шрифт.
    ///
    /// @param font Вказівник на шрифт.
    ///
    /// Шрифт можна вибрати зі списку рекомендованих шрифтів:
    ///
    /// - `FONT_4x6`
    /// - `FONT_5x7`
    /// - `FONT_5x8`
    /// - `FONT_6x12`
    /// - `FONT_6x13`
    /// - `FONT_7x13`
    /// - `FONT_8x13`
    /// - `FONT_9x15`
    /// - `FONT_10x20`
    ///
    /// Наприклад:
    ///
    /// @code
    /// lilka::display.setFont(FONT_6x12);
    /// lilka::display.setCursor(0, 32);
    /// lilka::display.print("Привіт, Лілка!");
    /// @endcode
    void setFont(const uint8_t *font);
    /// Встановити курсор.
    ///
    /// @param x Координата X.
    /// @param y Координата Y.
    void setCursor(int16_t x, int16_t y);
    /// Встановити масштаб тексту.
    ///
    /// @param size Масштаб.
    ///
    /// Якщо цей параметр дорівнює 1, текст виводиться в масштабі 1:1. Якщо 2, то кожен піксель тексту буде займати 2x2
    /// пікселі на екрані, і так далі.
    void setTextSize(uint8_t size);
    /// Встановити колір тексту.
    void setTextColor(uint16_t color);
    /// Встановити колір тексту та фону.
    void setTextColor(uint16_t color, uint16_t background);
    /// Відобразити текст.
    /// @param ... Текст.
    ///
    /// Наприклад:
    /// @code
    /// lilka::display.setCursor(0, 32);
    /// lilka::display.setTextColor(lilka::display.color565(0, 0, 0)); // Білий текст
    /// lilka::display.print("Привіт,j ");
    /// lilka::display.print(String("Лілка!\n"));
    /// lilka::display.print(42);
    /// @endcode
    ///
    /// @see setCursor, setTextColor, setTextSize, setFont
    void print(...);
    /// Заповнити екран кольором.
    void fillScreen(uint16_t color);
    /// Встановити колір пікселя.
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    /// Намалювати лінію.
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
    /// Намалювати прямокутник.
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    /// Намалювати заповнений прямокутник.
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    /// Намалювати коло.
    void drawCircle(int16_t x, int16_t y, int16_t r, uint16_t color);
    /// Намалювати заповнене коло.
    void fillCircle(int16_t x, int16_t y, int16_t r, uint16_t color);
    /// Намалювати трикутник.
    void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
    /// Намалювати заповнений трикутник.
    void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
    /// Намалювати еліпс.
    /// @param x Координата X центру еліпса.
    /// @param y Координата Y центру еліпса.
    /// @param rx Радіус по X.
    /// @param ry Радіус по Y.
    /// @param color Колір.
    void drawEllipse(int16_t x, int16_t y, int16_t rx, int16_t ry, uint16_t color);
    /// Намалювати заповнений еліпс.
    /// @see drawEllipse
    void fillEllipse(int16_t x, int16_t y, int16_t rx, int16_t ry, uint16_t color);
    /// Намалювати дугу.
    /// @param x Координата X центру дуги.
    /// @param y Координата Y центру дуги.
    /// @param r1 Зовнішній радіус дуги.
    /// @param r2 Внутрішній радіус дуги.
    /// @param start Початковий кут (в градусах).
    /// @param end Кінцевий кут (в градусах).
    /// @param color Колір.
    void drawArc(int16_t x, int16_t y, int16_t r1, int16_t r2, int16_t start, int16_t end, uint16_t color);
    /// Намалювати заповнену дугу.
    /// @see drawArc
    void fillArc(int16_t x, int16_t y, int16_t r1, int16_t r2, int16_t start, int16_t end, uint16_t color);

    /// Намалювати зображення.
    /// @param x Координата X лівого верхнього кута зображення.
    /// @param y Координата Y лівого верхнього кута зображення.
    /// @param bitmap Вказівник на зображення.
    /// @param w Ширина зображення.
    /// @param h Висота зображення.
    ///
    /// Приклад використання:
    /// @code
    /// lilka::Bitmap *bitmap = lilka::resources.loadBitmap("image.bmp");
    /// lilka::display.drawBitmap(0, 0, bitmap->pixels, bitmap->width, bitmap->height);
    /// @endcode
    void draw16bitRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h);
    /// @see draw16bitRGBBitmap
    void draw16bitRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h);

    /// Намалювати зображення з прозорістю.
    /// @param x Координата X лівого верхнього кута зображення.
    /// @param y Координата Y лівого верхнього кута зображення.
    /// @param bitmap Вказівник на зображення.
    /// @param transparent_color Колір, який буде вважатися прозорим.
    /// @param w Ширина зображення.
    /// @param h Висота зображення.
    ///
    /// Приклад використання:
    /// @code
    /// // Завантажити зображення з файлу "image.bmp", використовуючи білий колір як прозорий.
    /// lilka::Bitmap *bitmap = lilka::resources.loadBitmap("image.bmp", lilka::display.color565(255, 255, 255));
    /// lilka::display.draw16bitRGBBitmapWithTranColor(
    ///     0, 0, bitmap->pixels, bitmap->transparentColor, bitmap->width, bitmap->height
    /// );
    /// @endcode
    void draw16bitRGBBitmapWithTranColor(
        int16_t x, int16_t y, uint16_t *bitmap, uint16_t transparent_color, int16_t w, int16_t h
    );
#endif
    /// @see draw16bitRGBBitmapWithTranColor
    void draw16bitRGBBitmapWithTranColor(
        int16_t x, int16_t y, const uint16_t bitmap[], uint16_t transparent_color, int16_t w, int16_t h
    );
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
/// Цей клас, як і `Display`, є підкласом `Arduino_GFX` з бібліотеки `Arduino_GFX_Library`.
/// Це означає, що майже всі методи, які доступні в `Display`, також доступні в `Canvas`.
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
    void draw16bitRGBBitmapWithTranColor(
        int16_t x, int16_t y, const uint16_t bitmap[], uint16_t transparent_color, int16_t w, int16_t h
    );
};

/// Екземпляр класу `Display`, який можна використовувати для роботи з дисплеєм.
/// Вам не потрібно інстанціювати `Display` вручну.
extern Display display;

} // namespace lilka

#endif // LILKA_DISPLAY_H

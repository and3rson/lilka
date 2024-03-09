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
class Image;

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
    /// Встановити зображення, яке буде відображатися при запуску.
    ///
    /// За замовчуванням відображається вітальний екран Лілки.
    ///
    /// @note Якщо викликати цей метод, то вітальний екран буде відображатись навіть якщо `LILKA_NO_SPLASH` встановлено в `true`.
    ///
    /// Його потрібно викликати перед викликом `lilka::begin()` або не викликати взагалі.
    /// @param splash Масив 16-бітних кольорів (5-6-5) з розміром 280*240.
    void setSplash(const uint16_t *splash);
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
#endif

    /// Намалювати зображення.
    /// @param image Вказівник на зображення (об'єкт класу `lilka::Image`).
    /// @param x Координата X лівого верхнього кута зображення.
    /// @param y Координата Y лівого верхнього кута зображення.
    ///
    /// Приклад використання:
    ///
    /// @code
    /// lilka::Image *image = lilka::resources.loadImage("image.bmp");
    /// if (!image) {
    ///     Serial.println("Failed to load image");
    ///     return;
    /// }
    /// lilka::display.drawImage(image, 32, 64);
    /// // Звільнюємо пам'ять
    /// delete image;
    /// @endcode
    void drawImage(Image *image, int16_t x, int16_t y);

#ifdef DOXYGEN
    /// Намалювати зображення з масиву 16-бітних точок.
    /// @param x Координата X лівого верхнього кута зображення.
    /// @param y Координата Y лівого верхнього кута зображення.
    /// @param bitmap Масив 16-бітних кольорів.
    /// @param w Ширина зображення.
    /// @param h Висота зображення.
    ///
    /// Приклад використання:
    /// @code
    /// lilka::Image *image = lilka::resources.loadImage("image.bmp");
    /// lilka::display.drawBitmap(0, 0, image->pixels, image->width, image->height);
    /// @endcode
    void draw16bitRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h);
    /// @see draw16bitRGBBitmap
    void draw16bitRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h);

    /// Намалювати зображення з масиву 16-бітних точок і вказати колір, який буде вважатися прозорим.
    /// @param x Координата X лівого верхнього кута зображення.
    /// @param y Координата Y лівого верхнього кута зображення.
    /// @param bitmap Масив 16-бітних кольорів.
    /// @param transparent_color Колір, який буде вважатися прозорим.
    /// @param w Ширина зображення.
    /// @param h Висота зображення.
    ///
    /// Приклад використання:
    /// @code
    /// // Завантажити зображення з файлу "image.bmp", використовуючи білий колір як прозорий.
    /// lilka::Image *image = lilka::resources.loadImage("image.bmp", lilka::display.color565(255, 255, 255));
    /// lilka::display.draw16bitRGBBitmapWithTranColor(
    ///     0, 0, image->pixels, image->transparentColor, image->width, image->height
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
    void renderCanvas(Canvas *canvas);

private:
    const uint16_t *splash;
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
///         lilka::display.renderCanvas(&canvas); // Відобразити буфер на екрані - жодного мерехтіння!
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
    Canvas(uint16_t w, uint16_t h);
    Canvas(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    /// Намалювати зображення.
    /// @see Display::drawImage
    void drawImage(Image *image, int16_t x, int16_t y);
    void draw16bitRGBBitmapWithTranColor(
        int16_t x, int16_t y, const uint16_t bitmap[], uint16_t transparent_color, int16_t w, int16_t h
    );
    void drawCanvas(Canvas *canvas);
    int16_t x();
    int16_t y();
};

/// Зображення
///
/// Містить розміри, прозорий колір та пікселі зображення (в 16-бітному форматі, 5-6-5).
/// Пікселі зберігаються в рядку зліва направо, зверху вниз.
///
/// @note Основна відмінність Image від поняття "bitmap" погялає в тому, що Image містить масив пікселів, розміри зображення і прозорий колір, в той час як "bitmap" - це просто масив пікселів.
class Image {
public:
    Image(uint32_t width, uint32_t height, int32_t transparentColor = -1);
    ~Image();
    /// Обернути зображення на заданий кут (в градусах) і записати результат в `dest`.
    ///
    /// @param angle Кут обертання в градусах.
    /// @param dest Вказівник на Image, в яке буде записано обернуте зображення.
    /// @param blankColor 16-бітний колір (5-6-5), який буде використаний для заповнення пікселів, які виходять за межі зображення.
    /// @warning `dest` повинен бути ініціалізований заздалегідь.
    ///
    /// Приклад:
    ///
    /// @code
    /// lilka::Image *image = lilka::resources.loadImage("image.bmp");
    /// if (!image) {
    ///    Serial.println("Failed to load image");
    ///    return;
    /// }
    /// lilka::Image *rotatedImage = new lilka::Image(image->width, image->height);
    /// // Повертаємо на 30 градусів, заповнюючи пікселі, які виходять за межі зображення, білим кольором:
    /// image->rotate(30, rotatedImage, lilka::display.color565(255, 255, 255));
    /// // Звільнюємо пам'ять
    /// delete image;
    /// delete rotatedImage;
    /// @endcode
    void rotate(int16_t angle, Image *dest, int32_t blankColor);
    /// Віддзеркалити зображення по горизонталі і записати результат в `dest`.
    void flipX(Image *dest);
    /// Віддзеркалити зображення по вертикалі і записати результат в `dest`.
    void flipY(Image *dest);
    uint32_t width;
    uint32_t height;
    /// 16-бітний колір (5-6-5), який буде прозорим. За замовчуванням -1 (прозорість відсутня).
    int32_t transparentColor;
    uint16_t *pixels;
};

/// Екземпляр класу `Display`, який можна використовувати для роботи з дисплеєм.
/// Вам не потрібно інстанціювати `Display` вручну.
extern Display display;

} // namespace lilka

#endif // LILKA_DISPLAY_H

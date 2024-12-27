#ifndef LILKA_DISPLAY_H
#define LILKA_DISPLAY_H

#include "config.h"
#include "colors565.h"
#include <Arduino_GFX_Library.h>
#include <U8g2lib.h>

namespace lilka {

// Рекомендовані шрифти для використання з дисплеєм.
#define FONT_4x6          u8g2_font_4x6_t_cyrillic
#define FONT_5x7          u8g2_font_5x7_t_cyrillic
#define FONT_5x8          u8g2_font_5x8_t_cyrillic
#define FONT_6x12         u8g2_font_6x12_t_cyrillic
#define FONT_6x13         u8g2_font_6x13_t_cyrillic
#define FONT_7x13         u8g2_font_7x13_t_cyrillic
#define FONT_8x13         u8g2_font_8x13_t_cyrillic
#define FONT_8x13_MONO    u8g2_font_8x13_mf
#define FONT_9x15         u8g2_font_9x15_t_cyrillic
#define FONT_9x15_SYMBOLS u8g2_font_9x15_t_symbols
#define FONT_10x20        u8g2_font_10x20_t_cyrillic
#define FONT_10x20_MONO   u8g2_font_10x20_mf

class Canvas;
class Image;
class Transform;
typedef struct int_vector_t {
    int32_t x;
    int32_t y;
} int_vector_t;

typedef enum {
    ALIGN_START,
    ALIGN_CENTER,
    ALIGN_END,
} Alignment;

/// Цей клас описує спільні методи для класів `Display` та `Canvas`, оскільки вони обидва є підкласами `GFX`.
template <typename T>
class GFX {
public:
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
    /// - `FONT_8x13_MONO`
    /// - `FONT_9x15`
    /// - `FONT_10x20`
    /// - `FONT_10x20_MONO`
    ///
    /// Також можна використати будь-який інший шрифт з бібліотеки `U8g2`: https://github.com/olikraus/u8g2/wiki/fntlistallplain
    ///
    /// @code
    /// lilka::display.setFont(FONT_6x12);
    /// lilka::display.setCursor(0, 32);
    /// lilka::display.print("Привіт, Лілка!");
    /// @endcode
    void setFont(const uint8_t* font);
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
    /// @code
    /// lilka::display.setCursor(0, 32);
    /// lilka::display.setTextColor(lilka::colors::Black); // Чорний текст
    /// lilka::display.print("Привіт, ");
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
    /// Намалювати зображення з масиву 16-бітних точок.
    /// @param x Координата X лівого верхнього кута зображення.
    /// @param y Координата Y лівого верхнього кута зображення.
    /// @param bitmap Масив 16-бітних кольорів.
    /// @param w Ширина зображення.
    /// @param h Висота зображення.
    ///
    /// @code
    /// lilka::Image *image = lilka::resources.loadImage("image.bmp");
    /// lilka::display.drawBitmap(0, 0, image->pixels, image->width, image->height);
    /// @endcode
    void draw16bitRGBBitmap(int16_t x, int16_t y, uint16_t* bitmap, int16_t w, int16_t h);
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
    /// @code
    /// // Завантажити зображення з файлу "image.bmp", використовуючи білий колір як прозорий.
    /// lilka::Image *image = lilka::resources.loadImage("image.bmp", lilka::colors::White);
    /// lilka::display.draw16bitRGBBitmapWithTranColor(
    ///     0, 0, image->pixels, image->transparentColor, image->width, image->height
    /// );
    /// @endcode
    void draw16bitRGBBitmapWithTranColor(
        int16_t x, int16_t y, uint16_t* bitmap, uint16_t transparent_color, int16_t w, int16_t h
    );
#endif
    /// Відобразити буфер на екрані (див. `lilka::Canvas`).
    void drawCanvas(Canvas* canvas);

    /// Намалювати зображення.
    /// @param image Вказівник на зображення (об'єкт класу `lilka::Image`).
    /// @param x Координата X осі зображення.
    /// @param y Координата Y осі зображення.
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
    void drawImage(Image* image, int16_t x, int16_t y);
    /// Намалювати зображення з афінними перетвореннями.
    /// @param image Вказівник на зображення (об'єкт класу `lilka::Image`).
    /// @param x Координата X осі зображення.
    /// @param y Координата Y осі зображення.
    /// @param transform Об'єкт класу `lilka::Transform`, який містить матрицю перетворення.
    /// @note Зверніть увагу, що перетворення - це повільніше, ніж звичайне малювання зображення, оскільки обчислює координати пікселів "на льоту". Використовуйте його лише тоді, коли не можете заздалегідь створити обернені копії зображеня за допомогою методів `lilka::Image::rotate`, `lilka::Image::flipX` та `lilka::Image::flipY`.
    /// @see lilka::Transform
    void drawImageTransformed(Image* image, int16_t x, int16_t y, Transform transform);
    int drawTextAligned(const char* text, int16_t x, int16_t y, Alignment hAlign, Alignment vAlign);
    void getTextBoundsAligned(
        const char* text, int16_t x, int16_t y, Alignment hAlign, Alignment vAlign, int16_t* x1, int16_t* y1,
        uint16_t* w, uint16_t* h
    );
};

/// @see GFX
///
/// Клас для роботи з дисплеєм.
///
/// Використовується для відображення графічних об'єктів.
///
/// Цей клас наслідує `Arduino_GFX` з бібліотеки `Arduino_GFX_Library`, а також клас `GFX`.
///
/// Детальніше про доступні методи можна дізнатися в документації бібліотеки `Arduino_GFX_Library` -
/// https://github.com/moononournation/Arduino_GFX.
///
/// @code
/// #include <lilka.h>
///
/// void setup() {
///     lilka.begin();
/// }
///
/// void loop() {
///    lilka::display.fillScreen(lilka::colors::Red); // Заповнити екран червоним кольором
///    lilka::display.setCursor(32, 32);
///    lilka::display.setTextColor(lilka::colors::Green); // Зелений текст
///    lilka::display.print("Привіт, Лілка!");
/// }
/// @endcode
class Display : public Arduino_ST7789, public GFX<Display> {
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
    /// @param splash Масив 16-бітних кольорів (5-6-5) з розміром 280*240 (або масив байтів, закодованих алгоритмом RLE, з довжиною rleLength).
    /// @param rleLength Якщо використовується RLE-кодування, цей аргумент вказує довжину масиву splash. Зображення повинне бути згенероване за допомогою утиліти `sdk/tools/image2code` з прапорцем `--rle`.
    void setSplash(const void* splash, uint32_t rleLength = 0);
    /// Перетворити HSV колір в 16-бітний формат.
    ///
    /// @param hue Тон (0-360).
    /// @param sat Насиченість (0-100).
    /// @param val Яскравість (0-100).
    /// @return 16-бітний колір.
    uint16_t color565hsv(uint16_t hue, uint8_t sat, uint8_t val);
    void draw16bitRGBBitmapWithTranColor(
        int16_t x, int16_t y, const uint16_t bitmap[], uint16_t transparent_color, int16_t w, int16_t h
    );
    uint8_t* getFont();
    void drawCanvasInterlaced(Canvas* canvas, bool odd);

private:
    const void* splash;
    uint32_t rleLength;
};

/// @see GFX
///
/// Клас для роботи з графічним буфером. Він наслідує клас `Arduino_Canvas` з бібліотеки `Arduino_GFX_Library`, а також клас `GFX`.
///
/// При частому перемальовуванні екрану без використання буфера може спостерігатися мерехтіння.
/// Наприклад, якщо використовувати метод `fillScreen` для очищення екрану перед кожним викликом `print`,
/// то текст буде мерехтіти.
///
/// Щоб уникнути цього, можна використовувати графічний буфер. Цей клас дозволяє малювати графічні об'єкти на буфері,
/// а потім відобразити його на екрані за допомогою методу `lilka::display.drawCanvas`. Фактично, цей клас і є графічним буфером.
///
/// Такий підхід дозволяє зменшити мерехтіння, але збільшує використання пам'яті. Він називається "буферизація",
/// оскільки ми спершу малюємо на буфері, а тоді відображаємо буфер на екрані.
///
/// Цей клас, як і `Display`, є підкласом `Arduino_GFX` з бібліотеки `Arduino_GFX_Library`.
/// Це означає, що майже всі методи, які доступні в `Display`, також доступні в `Canvas`.
///
/// @code
/// #include <lilka.h>
///
/// void setup() {
///     lilka.begin();
/// }
///
/// void loop() {
///     lilka::Canvas canvas; // Створити новий Canvas зі стандартним розміром (розмір дисплею)
///     int y = 100;
///     while (1) {
///         canvas.fillScreen(lilka::colors::Black); // Заповнити буфер чорним кольором
///         canvas.setCursor(32, y);
///         canvas.setTextColor(lilka::colors::Black); // Чорний текст
///         canvas.print("Привіт, Лілка!");
///         lilka::display.drawCanvas(&canvas); // Відобразити буфер на екрані - жодного мерехтіння!
///         y++;
///         if (y > 200) {
///             y = 100;
///         }
///     }
/// }
/// @endcode
class Canvas : public Arduino_Canvas, public GFX<Canvas> {
public:
    /// Створити буфер зі стандартним розміром (який дорівнює розміру дисплею).
    Canvas();
    /// Створити буфер з заданими розмірами.
    /// @param w Ширина буфера.
    /// @param h Висота буфера.
    Canvas(uint16_t w, uint16_t h);
    /// Створити буфер з заданими розмірами та позицією.
    /// @param x Координата X лівого верхнього кута буфера.
    /// @param y Координата Y лівого верхнього кута буфера.
    /// @param w Ширина буфера.
    /// @param h Висота буфера.
    Canvas(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    int16_t x();
    int16_t y();
    void draw16bitRGBBitmapWithTranColor(
        int16_t x, int16_t y, const uint16_t bitmap[], uint16_t transparent_color, int16_t w, int16_t h
    );
    uint8_t* getFont();
};

// Dirty (and painfully slow!) workaround to calculate text width (since getTextBounds clips result to canvas width)
// Will be superseded by https://github.com/moononournation/Arduino_GFX/pull/460
int16_t getTextWidth(const uint8_t* font, const char* text);

/// Зображення
///
/// Містить розміри, прозорий колір та пікселі зображення (в 16-бітному форматі, 5-6-5).
/// Пікселі зберігаються в рядку зліва направо, зверху вниз.
///
/// Вісь зображення (pivot) - це точка, яка вказує на центр зображення. Це дозволяє вам встановити точку, відносно якої буде відображатися зображення, а також навколо якої буде відбуватися перетворення зображення.
///
/// @note Основна відмінність Image від поняття "bitmap" погялає в тому, що Image містить масив пікселів, розміри зображення і прозорий колір, в той час як "bitmap" - це просто масив пікселів.
class Image {
public:
    /// Створити зображення з заданими розмірами та прозорим кольором.
    ///
    /// Якщо `transparentColor` встановлено в `-1`, то прозорість відсутня.
    ///
    /// @param width Ширина зображення.
    /// @param height Висота зображення.
    /// @param transparentColor 16-бітний колір (5-6-5), який буде вважатися прозорим. За замовчуванням -1 (прозорість відсутня).
    /// @param pivotX Координата X центральної осі зображення. За замовчуванням 0.
    /// @param pivotY Координата Y центральної осі зображення. За замовчуванням 0.
    Image(uint32_t width, uint32_t height, int32_t transparentColor = -1, int16_t pivotX = 0, int16_t pivotY = 0);
    ~Image();
    /// Створити зображення з масиву 16-бітних точок, стисненого алгоритмом RLE.
    static Image* newFromRLE(
        const uint8_t* data, uint32_t length, uint32_t width, uint32_t height, int32_t transparentColor = -1,
        int16_t pivotX = 0, int16_t pivotY = 0
    );
    /// Обернути зображення на заданий кут (в градусах) і записати результат в `dest`.
    ///
    /// Цей метод, а також методи `flipX` та `flipY`, зручно використовувати для створення обернених та віддзеркалених копій зображення, якщо ви заздалегідь знаєте, які варіанти зображення вам знадобляться.
    ///
    /// Замість них можна використовувати клас `lilka::Transform` та його методи, які дозволяють виконувати та комбінувати складніші перетворення "на льоту", але такі перетворення є повільнішими.
    ///
    /// @param angle Кут обертання в градусах.
    /// @param dest Вказівник на Image, в яке буде записано обернуте зображення.
    /// @param blankColor 16-бітний колір (5-6-5), який буде використаний для заповнення пікселів, які виходять за межі зображення.
    /// @warning `dest` повинен бути ініціалізований заздалегідь.
    ///
    /// @code
    /// lilka::Image *image = lilka::resources.loadImage("image.bmp");
    /// if (!image) {
    ///    Serial.println("Failed to load image");
    ///    return;
    /// }
    /// lilka::Image *rotatedImage = new lilka::Image(image->width, image->height);
    /// // Повертаємо на 30 градусів, заповнюючи пікселі, які виходять за межі зображення, білим кольором:
    /// image->rotate(30, rotatedImage, lilka::colors::White);
    /// // Звільнюємо пам'ять
    /// delete image;
    /// delete rotatedImage;
    /// @endcode
    /// @see Display::drawImageTransformed, Canvas::drawImageTransformed, Transform
    void rotate(int16_t angle, Image* dest, int32_t blankColor);
    /// Віддзеркалити зображення по горизонталі і записати результат в `dest`.
    void flipX(Image* dest);
    /// Віддзеркалити зображення по вертикалі і записати результат в `dest`.
    void flipY(Image* dest);
    /// Ширина зображення.
    uint32_t width;
    /// Висота зображення.
    uint32_t height;
    /// 16-бітний колір (5-6-5), який буде прозорим. За замовчуванням -1 (прозорість відсутня).
    int32_t transparentColor;
    /// Координата X центральної осі зображення.
    int16_t pivotX;
    /// Координата Y центральної осі зображення.
    int16_t pivotY;
    /// Масив пікселів зображення. Ініціалізується в конструкторі автоматично.
    uint16_t* pixels;
};

/// Клас для роботи з афінними перетвореннями.
///
/// Афінні перетворення - це перетворення, які зберігають паралельність ліній.
/// Вони включають в себе обертання, масштабування та віддзеркалення.
///
/// Перетворення - це всього лиш матриця 2x2. Застосування перетворення до вектора - це множення цього вектора на матрицю перетворення. Магія!
///
/// @code
/// // Цей код обертає зображення на 30 градусів і тоді віддзеркалює його по горизонталі
/// lilka::Transform transform = lilka::Transform().rotate(30).flipX();
/// lilka::display.drawImageTransformed(image, 32, 64, transform);
/// @endcode
class Transform {
public:
    /// Створити об'єкт класу `lilka::Transform`.
    Transform();
    Transform(const Transform& other);
    Transform& operator=(const Transform& other);
    /// Обернути навколо центру на кут `angle` (в градусах).
    ///
    /// Оскільки на екрані вісь Y вказує вниз, обертання буде здійснено за годинниковою стрілкою.
    /// @param angle Кут обертання в градусах.
    /// @return Нове перетворення.
    Transform rotate(int16_t angle);
    /// Масштабувати по X та Y.
    ///
    /// Щоб віддзеркалити зображення, використайте від'ємні значення (наприклад, -1).
    /// @param scaleX Масштаб по X.
    /// @param scaleY Масштаб по Y.
    /// @return Нове перетворення.
    Transform scale(float scaleX, float scaleY);

    /// Помножити це перетворення на інше.
    ///
    /// @note Зверніть увагу: при комбінації перетворень порядок важливий, і він є оберненим до порядку множення матриць! В результаті цього, перетворення other буде виконано **перед** поточним перетворенням. Тобто якщо в вас є деякі перетворення `A` та `B`, то перетворення `A.multiply(B)` утворить нове перетворення, в якому спочатку виконається перетворення `B`, а потім `A`.
    /// @note Для уникнення плутанини рекомендуємо використовувати більш високорівневі методи, такі як `rotate` та `scale`.
    /// @param other Інше перетворення.
    /// @return Перетворення, яке є результатом застосування цього перетворення після `other` перетворення.
    /// @code
    /// lilka::Transform rotate30 = lilka::Transform().rotate(30); // обернути на 30 градусів
    /// lilka::Transform scale2x = lilka::Transform().scale(2, 2); // збільшити в 2 рази
    /// lilka::Transform scaleThenRotate = rotate30.multiply(scale2x); // результат - це перетворення "спочатку збільшити, а потім обернути", а не навпаки!
    /// @endcode
    Transform multiply(Transform other);

    /// Інвертувати перетворення.
    /// @note Інвертне перетворення - це таке перетворення, яке скасує це перетворення, тобто є зворотнім до цього.
    /// @return Інвертоване перетворення.
    Transform inverse();

    /// Перетворити вектор, використовуючи це перетворення.
    /// @param vector Вхідний вектор.
    /// @return Результат перетворення.
    int_vector_t transform(int_vector_t vector);

    // Матриця перетворення
    float matrix[2][2]; // [рядок][стовпець]
};

class RLEDecoder {
public:
    RLEDecoder(const uint8_t* data, uint32_t length);
    uint16_t next();

private:
    const uint8_t* data;
    uint32_t length;
    uint32_t pos;
    uint8_t count;
    uint16_t current;
};

/// Екземпляр класу `Display`, який можна використовувати для роботи з дисплеєм.
/// Вам не потрібно інстанціювати `Display` вручну.
extern Display display;

} // namespace lilka

#endif // LILKA_DISPLAY_H

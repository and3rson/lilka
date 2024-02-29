#ifndef LILKA_RESOURCES_H
#define LILKA_RESOURCES_H

#include <Arduino.h>

namespace lilka {

/// Зображення
///
/// Містить розміри, прозорий колір та пікселі зображення (в 16-бітному форматі, 5-6-5).
/// Пікселі зберігаються в рядку зліва направо, зверху вниз.
class Bitmap {
public:
    Bitmap(uint32_t width, uint32_t height, int32_t transparentColor = -1);
    ~Bitmap();
    /// Повертає зображення, яке отримане обертанням поточного зображення на заданий кут (в градусах), і записує його в `dest`.
    ///
    /// @param angle Кут обертання в градусах.
    /// @param dest Вказівник на зображення, в яке буде записано обернуте зображення.
    /// @param blankColor 16-бітний колір (5-6-5), який буде використаний для заповнення пікселів, які виходять за межі зображення.
    /// @warning `dest` повинен бути ініціалізований заздалегідь.
    ///
    /// Приклад:
    ///
    /// @code
    /// lilka::Bitmap *bitmap = lilka::resources.loadBitmap("image.bmp");
    /// if (!bitmap) {
    ///    Serial.println("Failed to load image");
    ///    return;
    /// }
    /// lilka::Bitmap *rotatedBitmap = new lilka::Bitmap(bitmap->width, bitmap->height);
    /// // Повертаємо на 30 градусів, заповнюючи пікселі, які виходять за межі зображення, білим кольором:
    /// bitmap->rotate(30, rotatedBitmap, lilka::display.color565(255, 255, 255));
    /// @endcode
    void rotate(int16_t angle, Bitmap *dest, int32_t blankColor);
    uint32_t width;
    uint32_t height;
    /// 16-бітний колір (5-6-5), який буде прозорим. За замовчуванням -1 (прозорість відсутня).
    int32_t transparentColor;
    uint16_t *pixels;
};

/// Клас для роботи з ресурсами - зображенням, файлами даних тощо.
class Resources {
public:
    /// Завантажити зображення в форматі BMP з файлу.
    ///
    /// \param filename Шлях до файлу.
    /// \param transparentColor 16-бітний колір (5-6-5), який буде прозорим. За замовчуванням -1 (прозорість відсутня).
    /// \return Вказівник на зображення.
    ///
    /// \warning Пам'ять для зображення виділяється динамічно. Після використання зображення, його потрібно видалити за допомогою `delete`.
    ///
    /// Приклад:
    ///
    /// \code
    /// lilka::Bitmap *bitmap = lilka::resources.loadBitmap("image.bmp", lilka::display.color565(255, 255, 0)); //
    /// Жовтий колір буде прозорим if (!bitmap) {
    ///     Serial.println("Failed to load image");
    ///     return;
    /// }
    /// // Відобразити зображення на екрані
    /// lilka::display.draw16bitRGBBitmapWithTranColor(50, 100, bitmap->pixels, bitmap->transparentColor, bitmap->width,
    /// bitmap->height);
    /// // Звільнити пам'ять
    /// delete bitmap;
    /// \endcode
    Bitmap *loadBitmap(String filename, int32_t transparentColor = -1);
    /// Прочитати вміст файлу.
    ///
    /// TODO: Update sdcard/filesystem stuff
    ///
    /// \param filename Шлях до файлу.
    /// \return 0, якщо читання успішне; -1, якщо файл не знайдено
    /// \warning Не використовуйте цей метод для читання великих файлів, оскільки весь вміст файлу зберігається в
    /// пам'яті. Для великих файлів використовуйте методи `sdcard` та `filesystem`.
    int readFile(String filename, String &fileContent);
    /// Записати вміст файлу.
    /// \param filename Шлях до файлу.
    /// \param fileContent Вміст файлу.
    /// \return 0, якщо запис успішний; -1, якщо запис не вдався
    int writeFile(String filename, String fileContent);
};

/// Екземпляр класу `Resources`, який можна використовувати для завантаження ресурсів.
/// Вам не потрібно інстанціювати `Resources` вручну.
extern Resources resources;

} // namespace lilka

#endif // LILKA_RESOURCES_H

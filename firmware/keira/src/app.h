#ifndef MAIN_APP_H
#define MAIN_APP_H

#include <Arduino.h>
#include <lilka.h>

typedef enum {
    APP_FLAG_NONE = 0,
    APP_FLAG_FULLSCREEN = 1,
} AppFlags;

/// Клас, що представляє додаток для Кіри.
///
/// Додатки запускаються за допомогою синглтону AppManager.
///
/// Додаток має визначити принаймні метод run(), який буде викликатися в окремій задачі FreeRTOS.
///
/// При завершенні додатку, AppManager зупиняє задачу та видаляє об'єкт додатку.
///
/// Приклад запуску додатку:
///
/// @code
/// #include <appmanager.h>
/// #include <myapp.h>
///
/// // ...
///
/// AppManager::getInstance()->runApp(new MyApp());
/// @endcode
class App {
    friend class AppManager;

public:
    App(const char* name);
    App(const char* name, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    virtual ~App();
    /// Повідомити ОС, що додаток завершив малювання кадру.
    ///
    /// Цей метод слід викликати після малювання кожного кадру.
    ///
    /// Технічно, цей метод міняє місцями передній буфер (canvas) та задній буфер (backCanvas).
    ///
    /// Якщо цей метод викликається в той час, коли ОС вже малює попередній кадр, то він призведе до нетривалого блокування додатку.
    /// Іншими словами, якщо ваш додаток малює кадри швидше, ніж ОС здатна їх відображати, то цей метод буде час від часу заповільнювати ваш додаток.
    /// Це не проблема, але варто про це пам'ятати.
    void queueDraw();
    const char* getName();

    eTaskState getState();

    /// Вказівник на передній буфер для малювання.
    ///
    /// Додаток повинен використовувати цей буфер для малювання всієї графіки.
    ///
    /// Після малювання, буфер потрібно відобразити на екрані за допомогою методу queueDraw().
    lilka::Canvas* canvas;

protected:
    void start();
    void suspend();
    void resume();
    void stop();
    void setCore(int appCore);
    /// Встановити прапорці додатку.
    ///
    /// Наприклад, якщо додаток має відображатися на весь екран, то слід викликати setFlags(APP_FLAG_FULLSCREEN).
    /// @param flags
    void setFlags(AppFlags flags);
    AppFlags getFlags();
    /// Встановити розмір стеку задачі додатку.
    ///
    /// За замовчуванням, розмір стеку задачі дорівнює 8192 байт. Проте деякі додатки можуть вимагати більший розмір стеку.
    void setStackSize(uint32_t stackSize);

    bool needsRedraw();
    void markClean();
    void forceRedraw();

    void acquireBackCanvas();
    void releaseBackCanvas();

    lilka::Canvas* backCanvas;

private:
    static void _run(void* data);
    /// Основний код додатку.
    ///
    /// Цей метод викликається в окремій задачі FreeRTOS.
    ///
    /// Додаток завершується, коли цей метод завершується або робить return.
    virtual void run() = 0;
    /// Цей метод викликається операційною системою, коли вона збирається зупинити ваш додаток.
    virtual void onSuspend() {
    }
    /// Цей метод викликається операційною системою, коли вона збирається відновити роботу вашого додатку.
    virtual void onResume() {
    }
    /// Цей метод викликається операційною системою, коли вона збирається зупинити ваш додаток.
    virtual void onStop() {
    }

    const char* name;
    SemaphoreHandle_t backCanvasMutex;
    bool isDrawQueued;
    int appCore;
    AppFlags flags;
    TaskHandle_t taskHandle;
    uint32_t stackSize;
};

#endif // MAIN_APP_H

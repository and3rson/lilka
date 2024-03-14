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
    void start();
    void suspend();
    void resume();
    void stop();
    static void _run(void* data);
    void queueDraw();
    const char* getName();

    eTaskState getState();

    lilka::Canvas* canvas;

protected:
    void setFlags(AppFlags flags);
    AppFlags getFlags();

    bool needsRedraw();
    void markClean();
    void forceRedraw();

    void acquireBackCanvas();
    void releaseBackCanvas();

    lilka::Canvas* backCanvas;

private:
    /// Основний код програми.
    ///
    /// Цей метод викликається в окремій задачі FreeRTOS.
    ///
    /// Програма завершується, коли цей метод завершується або робить return.
    virtual void run() = 0;
    virtual void onSuspend() {
    }
    virtual void onResume() {
    }
    virtual void onStop() {
    }

    const char* name;
    uint16_t x, y, w, h;
    SemaphoreHandle_t backCanvasMutex;
    bool isDrawQueued;
    AppFlags flags;
    TaskHandle_t taskHandle;
};

#endif // MAIN_APP_H

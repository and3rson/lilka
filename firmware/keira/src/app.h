#ifndef MAIN_APP_H
#define MAIN_APP_H

#include <Arduino.h>
#include <lilka.h>

typedef enum {
    APP_FLAG_NONE = 0,
    APP_FLAG_FULLSCREEN = 1,
} AppFlags;

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
    virtual void run() = 0;

    const char* name;
    uint16_t x, y, w, h;
    SemaphoreHandle_t backCanvasMutex;
    bool isDrawQueued;
    AppFlags flags;
    TaskHandle_t taskHandle;
};

#endif // MAIN_APP_H

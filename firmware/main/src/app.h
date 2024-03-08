#ifndef MAIN_APP_H
#define MAIN_APP_H

#include <Arduino.h>
#include <lilka.h>

class App {
public:
    App(const char *name);
    App(const char *name, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    virtual ~App();
    void start();
    void suspend();
    void resume();
    void stop();
    static void _run(void *data);
    virtual void run() = 0;
    void queueDraw();
    bool needsRedraw();
    void markClean();
    const char *getName();

    void acquireBackCanvas();
    void releaseBackCanvas();

    eTaskState getState();

    lilka::Canvas *canvas;
    lilka::Canvas *backCanvas;

private:
    const char *name;
    uint16_t x, y, w, h;
    SemaphoreHandle_t backCanvasMutex;
    bool dirty;
    TaskHandle_t taskHandle;
};

#endif // MAIN_APP_H

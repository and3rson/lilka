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
    virtual void run() = 0;
    void acquireCanvas();
    void releaseCanvas();
    lilka::Canvas *canvas;
    bool needsRedraw();
    void markClean();
    TaskHandle_t taskHandle;
    const char *getName();

private:
    const char *name;
    uint16_t x, y, w, h;
    SemaphoreHandle_t mutex;
    bool dirty;
};

#endif // MAIN_APP_H

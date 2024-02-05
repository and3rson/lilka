#ifndef LILKA_CONTROLLER_H
#define LILKA_CONTROLLER_H

#include "config.h"

#include <stdint.h>

namespace lilka {

typedef enum {
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3,
    A = 4,
    B = 5,
    SELECT = 6,
    START = 7,
    COUNT = 8,
} Button;

#define LILKA_DEBOUNCE_TIME 10 // 10ms

typedef struct {
    bool pressed;
    uint64_t time;
} state_t;

typedef struct {
    int up;
    int down;
    int left;
    int right;
    int a;
    int b;
    int select;
    int start;
} Input;

class Controller {
   public:
    Controller();
    void begin();
    Input state();
    void setHandler(Button button, void (*handler)(bool));
    void clearHandlers();

    static void on_up();
    static void on_down();
    static void on_left();
    static void on_right();
    static void on_a();
    static void on_b();
    static void on_select();
    static void on_start();

   private:
    void handle_interrupt(int stateIndex);
    static Controller *_instance;
    state_t states[Button::COUNT];
    int8_t pins[Button::COUNT] = {
        LILKA_GPIO_UP, LILKA_GPIO_DOWN, LILKA_GPIO_LEFT, LILKA_GPIO_RIGHT, LILKA_GPIO_A, LILKA_GPIO_B, LILKA_GPIO_SELECT, LILKA_GPIO_START,
    };
    void (*handlers[Button::COUNT])(bool);
};

extern Controller controller;

} // namespace lilka

#endif // LILKA_CONTROLLER_H

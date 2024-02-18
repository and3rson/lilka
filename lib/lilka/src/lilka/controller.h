#ifndef LILKA_CONTROLLER_H
#define LILKA_CONTROLLER_H

#include "config.h"

#include <stdint.h>

namespace lilka {

typedef enum {
    UP = 0,
    DOWN,
    LEFT,
    RIGHT,
    A,
    B,
    C,
    D,
    SELECT,
    START,
    COUNT,
} Button;

#define LILKA_DEBOUNCE_TIME 10 // 10ms

typedef struct {
    bool pressed;
    bool justPressed;
    bool justReleased;
    uint64_t time;
} ButtonState;

typedef union {
    struct {
        ButtonState up;
        ButtonState down;
        ButtonState left;
        ButtonState right;
        ButtonState a;
        ButtonState b;
        ButtonState c;
        ButtonState d;
        ButtonState select;
        ButtonState start;
    };
    ButtonState buttons[Button::COUNT];
} State;

class Controller {
public:
    Controller();
    void begin();
    void resetState();
    State getState();
    void setGlobalHandler(void (*handler)(Button, bool));
    void setHandler(Button button, void (*handler)(bool));
    void clearHandlers();

    static void on_up();
    static void on_down();
    static void on_left();
    static void on_right();
    static void on_a();
    static void on_b();
    static void on_c();
    static void on_d();
    static void on_select();
    static void on_start();

private:
    void handle_interrupt(int stateIndex);
    static Controller *_instance;
    State state;
    int8_t pins[Button::COUNT] = {
        LILKA_GPIO_UP, LILKA_GPIO_DOWN, LILKA_GPIO_LEFT, LILKA_GPIO_RIGHT, LILKA_GPIO_A, LILKA_GPIO_B, LILKA_GPIO_C, LILKA_GPIO_D, LILKA_GPIO_SELECT, LILKA_GPIO_START,
    };
    void (*handlers[Button::COUNT])(bool);
    void (*globalHandler)(Button, bool);
    int8_t _seq[10] = {UP, UP, DOWN, DOWN, LEFT, RIGHT, LEFT, RIGHT, B, A};
    int8_t _seqIndex = 0;
};

extern Controller controller;

} // namespace lilka

#endif // LILKA_CONTROLLER_H

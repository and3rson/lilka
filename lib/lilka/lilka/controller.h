#ifndef LILKA_CONTROLLER_H
#define LILKA_CONTROLLER_H

#include "config.h"

#include <stdint.h>

#define LILKA_STATE_UP 0
#define LILKA_STATE_DOWN 1
#define LILKA_STATE_LEFT 2
#define LILKA_STATE_RIGHT 3
#define LILKA_STATE_A 4
#define LILKA_STATE_B 5
#define LILKA_STATE_SELECT 6
#define LILKA_STATE_START 7
#define LILKA_STATE_COUNT 8

#define LILKA_DEBOUNCE_TIME 10 // 10ms

namespace lilka {

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
} input_t;

extern state_t states[];
extern int8_t pins[];

class Controller {
   public:
    Controller();
    void begin();
    input_t state();
};

extern Controller controller;

} // namespace lilka

#endif // LILKA_CONTROLLER_H

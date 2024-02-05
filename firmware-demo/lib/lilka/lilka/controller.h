#ifndef LILKA_CONTROLLER_H
#define LILKA_CONTROLLER_H

#include "config.h"

namespace lilka {

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

class Controller {
   public:
    Controller();
    void begin();
    input_t read();
};

extern Controller controller;

} // namespace lilka

#endif // LILKA_CONTROLLER_H

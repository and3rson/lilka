#ifndef LILY_MJSRUNNER_H
#define LILY_MJSRUNNER_H

#include <Arduino.h>
#include "lilka/display.h"
#include "mjs.h"

namespace lilka {

int mjs_run(Canvas *canvas, String path);

} // namespace lilka

#endif // LILY_MJSRUNNER_H

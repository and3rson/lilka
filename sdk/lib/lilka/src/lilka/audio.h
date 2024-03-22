#pragma once

#include <I2S.h>
#include "lilka/config.h"

namespace lilka {

class Audio {
public:
    Audio();
    void begin();
};

extern Audio audio;

// extern I2SClass i2s;

} // namespace lilka

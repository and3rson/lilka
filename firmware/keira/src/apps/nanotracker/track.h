#pragma once

#include <vector>

#include "pattern.h"

class Track {
public:
    void getPattern(int32_t index);

private:
    std::vector<Pattern*> patterns;
};

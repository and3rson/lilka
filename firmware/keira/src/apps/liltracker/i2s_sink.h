#include "sink.h"

class I2SSink : public ISink {
public:
    void start();
    size_t write(const int16_t* data, size_t size);
    void stop();
};

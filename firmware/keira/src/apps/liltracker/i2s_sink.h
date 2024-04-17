#include "sink.h"

class I2SSink : public Sink {
public:
    I2SSink();
    ~I2SSink();
    void start() override;
    size_t write(const int16_t* data, size_t size) override;
    void stop() override;
};

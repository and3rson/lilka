#include "sink.h"

class NullSink : public Sink {
public:
    void start() override;
    size_t write(const int16_t* data, size_t size) override;
    void stop() override;
};

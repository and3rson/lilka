#include "null_sink.h"

void NullSink::start() {
    // Do nothing
}

size_t NullSink::write(const int16_t* data, size_t size) {
    return size;
}

void NullSink::stop() {
    // Do nothing
}

#include "analyzer.h"

AudioOutputAnalyzer::AudioOutputAnalyzer(AudioOutput* sink, int16_t bufferDivisor) :
    sink(sink), bufferDivisor(bufferDivisor) {
    memset(buffer, 0, sizeof(buffer));
}

AudioOutputAnalyzer::~AudioOutputAnalyzer() {
}

bool AudioOutputAnalyzer::SetRate(int hz) {
    return sink->SetRate(hz);
}

bool AudioOutputAnalyzer::SetBitsPerSample(int bits) {
    return sink->SetBitsPerSample(bits);
}

bool AudioOutputAnalyzer::SetChannels(int chan) {
    return sink->SetChannels(chan);
}

bool AudioOutputAnalyzer::SetGain(float f) {
    return sink->SetGain(f);
}

bool AudioOutputAnalyzer::begin() {
    return sink->begin();
}

bool AudioOutputAnalyzer::ConsumeSample(int16_t sample[2]) {
    bool res = sink->ConsumeSample(sample);
    if (res) {
        buffer[bufferPos++ / bufferDivisor] = (sample[0] + sample[1]) / 2;
        if (bufferPos / bufferDivisor >= ANALYZER_BUFFER_SIZE) {
            bufferPos = 0;
        }
    }
    return res;
}

int16_t AudioOutputAnalyzer::readBuffer(int16_t* dest) {
    memcpy(dest, buffer, ANALYZER_BUFFER_SIZE * sizeof(int16_t));
    return ANALYZER_BUFFER_SIZE;
}

int16_t AudioOutputAnalyzer::getBufferHead() {
    return bufferPos / bufferDivisor;
}

bool AudioOutputAnalyzer::stop() {
    return sink->stop();
}

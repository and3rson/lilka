#include <lilka.h>

#include "sink.h"

typedef struct {
    char chunkID[4]; // "RIFF"
    unsigned int chunkSize; // size of the entire file minus 8 bytes
    char format[4]; // "WAVE"
} riff_header_t;

typedef struct {
    char subchunk1ID[4]; // "fmt "
    unsigned int subchunk1Size; // should be 16 for PCM
    unsigned short audioFormat; // PCM = 1
    unsigned short numChannels; // 1 for mono, 2 for stereo
    unsigned int sampleRate; // sampling rate (Hz)
    unsigned int byteRate; // = SampleRate * NumChannels * BitsPerSample/8
    unsigned short blockAlign; // = NumChannels * BitsPerSample/8
    unsigned short bitsPerSample; // 16 for PCM
} fmt_subchunk_t;

typedef struct {
    char subchunk2ID[4]; // "data"
    unsigned int subchunk2Size; // num samples * num channels * bits per sample/8
} data_subchunk_t;

class WAVSink : public Sink {
public:
    explicit WAVSink(String filename);
    ~WAVSink();
    void start() override;
    size_t write(const int16_t* data, size_t size) override;
    void stop() override;

private:
    String filename;
    File file;
    uint8_t* buffer;
    size_t bufferPos;
    size_t numSamples;

    riff_header_t riffHeader;
    fmt_subchunk_t fmtSubchunk;
    data_subchunk_t dataSubchunk;
};

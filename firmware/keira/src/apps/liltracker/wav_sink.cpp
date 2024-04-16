#include "wav_sink.h"
#include "synth.h"

WAVSink::WAVSink(String filename) :
    filename(filename), bufferPos(0), numSamples(0), riffHeader(), fmtSubchunk(), dataSubchunk() {
    buffer = new uint8_t[65536];
}

WAVSink::~WAVSink() {
    delete[] buffer;
}

void WAVSink::start() {
    bufferPos = 0;
    numSamples = 0;

    file = SD.open(filename, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }

    // RIFF header
    memcpy(riffHeader.chunkID, "RIFF", 4); // "RIFF"
    // We don't know the size yet, so we'll write 0 for now:
    riffHeader.chunkSize = 0; // size of the entire file minus 8 bytes
    memcpy(riffHeader.format, "WAVE", 4); // "WAVE"

    // FMT subchunk
    memcpy(fmtSubchunk.subchunk1ID, "fmt ", 4); // "fmt "
    fmtSubchunk.subchunk1Size = 16; // should be 16 for PCM
    fmtSubchunk.audioFormat = 1; // PCM = 1
    fmtSubchunk.numChannels = 1; // 1 for mono, 2 for stereo
    fmtSubchunk.sampleRate = SYNTH_SAMPLE_RATE; // sampling rate (Hz)
    fmtSubchunk.byteRate = SYNTH_SAMPLE_RATE * 1 * 16 / 8; // = SampleRate * NumChannels * BitsPerSample / 8
    fmtSubchunk.blockAlign = 1 * 16 / 8; // = NumChannels * BitsPerSample / 8
    fmtSubchunk.bitsPerSample = 16; // 16 bits per sample

    // Data subchunk
    memcpy(dataSubchunk.subchunk2ID, "data", 4); // "data"
    // We don't know the size yet, so we'll write 0 for now:
    dataSubchunk.subchunk2Size = 0; // num samples * num channels * bits per sample / 8

    // Write the header
    file.write(reinterpret_cast<uint8_t*>(&riffHeader), sizeof(riffHeader));
    file.write(reinterpret_cast<uint8_t*>(&fmtSubchunk), sizeof(fmtSubchunk));
    file.write(reinterpret_cast<uint8_t*>(&dataSubchunk), sizeof(dataSubchunk));
}

size_t WAVSink::write(const int16_t* data, size_t size) {
    int32_t bFree = 65536 - bufferPos;
    int32_t bToStore = size * 2;
    if (bToStore > bFree) {
        bToStore = bFree;
    }
    memcpy(buffer + bufferPos, data, bToStore);
    bufferPos += bToStore;
    if (bufferPos == 65536) {
        file.write(buffer, 65536);
        bufferPos = 0;
    }
    numSamples += bToStore / 2;
    return bToStore / 2;
}

void WAVSink::stop() {
    // Update the header with the correct sizes
    riffHeader.chunkSize = file.position() - 8;
    dataSubchunk.subchunk2Size = numSamples * 1 * 16 / 8;

    // Write the header again
    file.seek(0);
    file.write(reinterpret_cast<uint8_t*>(&riffHeader), sizeof(riffHeader));
    file.write(reinterpret_cast<uint8_t*>(&fmtSubchunk), sizeof(fmtSubchunk));
    file.write(reinterpret_cast<uint8_t*>(&dataSubchunk), sizeof(dataSubchunk));

    // Close the file
    file.close();
}

#include <AudioOutput.h>

#define ANALYZER_BUFFER_SIZE 256

class AudioOutputAnalyzer : public AudioOutput {
public:
    // bufferDivisor - кількість семплів на один запис в буфер.
    // Значення "4" означає, що кожен 4-й семпл буде записаний в буфер.
    // Це дасть красивіший графік, але зменшить частоту оновлення
    // (але цього не буде помітно, оскільки швидкість наповнення буфера в рази вища за частоту малювання графіка).
    // Значення 1 відображає кожен семпл, але графік буде більш хаотичним (зате виглядає круто!)
    // Значення 32 (та вище) створить графік, який "пливе" по екрану, але не відображає деталей.
    explicit AudioOutputAnalyzer(AudioOutput* sink, int16_t bufferDivisor = 2);
    virtual ~AudioOutputAnalyzer() override;
    virtual bool SetRate(int hz) override;
    virtual bool SetBitsPerSample(int bits) override;
    virtual bool SetChannels(int chan) override;
    virtual bool SetGain(float f) override;
    virtual bool begin() override;
    virtual bool ConsumeSample(int16_t sample[2]) override;
    virtual bool stop() override;
    int16_t readBuffer(int16_t* dest);
    int16_t getBufferHead();

protected:
    AudioOutput* sink;
    int16_t bufferPos = 0;
    int16_t bufferDivisor = 0;
    int16_t buffer[ANALYZER_BUFFER_SIZE];
};

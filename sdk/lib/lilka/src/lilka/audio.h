#pragma once

#include <I2S.h>

namespace lilka {

/// Клас для ініціалізації аудіо.
///
/// Цей клас лише встановлює піни для I2S і відтворює тестовий звук.
///
/// Для роботи з аудіо використовуйте клас I2S напряму: https://espressif-docs.readthedocs-hosted.com/projects/arduino-esp32/en/latest/api/i2s.html#sample-code
class Audio {
public:
    Audio();
    /// Налаштоувує піни для I2S і відтворює тестовий звук.
    /// \warning Цей метод викликається автоматично при виклику `lilka::begin()`.
    void begin();
    /// Налаштувує піни для I2S.
    /// Цей метод варто викликати перед викликом `i2s_driver_install()`.
    void initPins();
    /// Регулює гучність до рівня, збереженого в налаштуваннях
    /// Цей метод бажано викликати перед `i2s_write`.
    void adjustVolume(void* buffer, size_t size, int bitsPerSample);
    /// Повертає рівень гучності
    int getVolume();
    /// Встановлює рівень гучності
    void setVolume(int level);
    /// Перевіряє чи увімкнено звук вітання
    bool getStartupSoundEnabled();
    /// Вмикає чи вимикає звук вітання
    void setStartupSoundEnabled(bool enable);

private:
    /// Глобальна гучність
    uint32_t volumeLevel = 100;
    bool startupSound = true;
    void saveSettings();
};

extern Audio audio;

} // namespace lilka

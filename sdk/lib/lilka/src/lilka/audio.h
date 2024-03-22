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
};

extern Audio audio;

} // namespace lilka

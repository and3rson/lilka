#include <Arduino.h>

#include "buzzer.h"
#include "config.h"
#include "serial.h"

namespace lilka {

Buzzer::Buzzer() :
    buzzerMutex(xSemaphoreCreateMutex()),
    melodyTaskHandle(NULL),
    currentMelody(NULL),
    currentMelodyLength(0),
    currentMelodyTempo(0) {
}

void Buzzer::begin() {
    // TODO: Use ledc?
#if LILKA_VERSION < 2
    serial_err("Buzzer is not supported on this board");
    return;
#else
    pinMode(LILKA_BUZZER, OUTPUT);
#endif
}

void Buzzer::play(uint16_t frequency) {
#if LILKA_VERSION < 2
    serial_err("Buzzer is not supported on this board");
    return;
#else
    xSemaphoreTake(buzzerMutex, portMAX_DELAY);
    _stop();
    tone(LILKA_BUZZER, frequency);
    xSemaphoreGive(buzzerMutex);
#endif
}

void Buzzer::play(uint16_t frequency, uint32_t duration) {
#if LILKA_VERSION < 2
    serial_err("Buzzer is not supported on this board");
    return;
#else
    xSemaphoreTake(buzzerMutex, portMAX_DELAY);
    _stop();
    tone(LILKA_BUZZER, frequency, duration);
    xSemaphoreGive(buzzerMutex);
#endif
}

void Buzzer::playMelody(const Tone* melody, uint32_t length, uint32_t tempo) {
#if LILKA_VERSION < 2
    serial_err("Buzzer is not supported on this board");
#else
    xSemaphoreTake(buzzerMutex, portMAX_DELAY);
    _stop();
    currentMelody = static_cast<Tone*>(realloc(currentMelody, length * sizeof(Tone)));
    memcpy(currentMelody, melody, length * sizeof(Tone));
    currentMelodyLength = length;
    currentMelodyTempo = tempo;
    xTaskCreate(melodyTask, "melodyTask", 2048, this, 1, &melodyTaskHandle);
    xSemaphoreGive(buzzerMutex);
#endif
}

void Buzzer::melodyTask(void* arg) {
    Buzzer* buzzer = static_cast<Buzzer*>(arg);
    for (uint32_t i = 0; i < buzzer->currentMelodyLength; i++) {
        xSemaphoreTake(buzzer->buzzerMutex, portMAX_DELAY);
        Tone currentTone = buzzer->currentMelody[i];
        if (currentTone.size == 0) {
            taskYIELD();
            continue;
        }
        uint32_t duration = (60000 / buzzer->currentMelodyTempo) / abs(currentTone.size);
        if (currentTone.size < 0) {
            duration += duration / 2;
        }

        if (buzzer->currentMelody[i].frequency != 0) {
            tone(LILKA_BUZZER, buzzer->currentMelody[i].frequency);
        }
        xSemaphoreGive(buzzer->buzzerMutex);
        vTaskDelay(duration / portTICK_PERIOD_MS);
        if (i == buzzer->currentMelodyLength - 1) {
            noTone(LILKA_BUZZER);
        }
    }
    buzzer->stop();
}

void Buzzer::stop() {
#if LILKA_VERSION < 2
    serial_err("Buzzer is not supported on this board");
#else
    xSemaphoreTake(buzzerMutex, portMAX_DELAY);
    _stop();
    xSemaphoreGive(buzzerMutex);
#endif
}

void Buzzer::_stop() {
    if (melodyTaskHandle != NULL) {
        vTaskDelete(melodyTaskHandle);
        melodyTaskHandle = NULL;
    }
    noTone(LILKA_BUZZER);
}

void Buzzer::playDoom() {
    const Tone doom_e1m1[] = {
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_E3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_D3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_C3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_AS2, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_B2, 8},   {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_E3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_D3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_AS2, -2},  {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_E3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_D3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_AS2, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_B2, 8},    {NOTE_C3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_E3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_D3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_C3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_AS2, -2},  {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_E3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_D3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_C3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_AS2, 8},   {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_B2, 8},    {NOTE_C3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_E3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_D3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_C3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_AS2, -2},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_E3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_D3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_C3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_AS2, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_B2, 8},   {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_E3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_D3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_FS3, -16}, {NOTE_D3, -16},  {NOTE_B2, -16}, {NOTE_A3, -16},  {NOTE_FS3, -16}, {NOTE_B2, -16},
        {NOTE_D3, -16},  {NOTE_FS3, -16}, {NOTE_A3, -16}, {NOTE_FS3, -16}, {NOTE_D3, -16},  {NOTE_B2, -16},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_E3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_D3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_C3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_AS2, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_B2, 8},   {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_E3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_D3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_AS2, -2},  {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_E3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_D3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_AS2, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_B2, 8},    {NOTE_C3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_E3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_D3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_B3, -16},  {NOTE_G3, -16},
        {NOTE_E3, -16},  {NOTE_G3, -16},  {NOTE_B3, -16}, {NOTE_E4, -16},  {NOTE_G3, -16},  {NOTE_B3, -16},
        {NOTE_E4, -16},  {NOTE_B3, -16},  {NOTE_G4, -16}, {NOTE_B4, -16},  {NOTE_A2, 8},    {NOTE_A2, 8},
        {NOTE_A3, 8},    {NOTE_A2, 8},    {NOTE_A2, 8},   {NOTE_G3, 8},    {NOTE_A2, 8},    {NOTE_A2, 8},
        {NOTE_F3, 8},    {NOTE_A2, 8},    {NOTE_A2, 8},   {NOTE_DS3, 8},   {NOTE_A2, 8},    {NOTE_A2, 8},
        {NOTE_E3, 8},    {NOTE_F3, 8},    {NOTE_A2, 8},   {NOTE_A2, 8},    {NOTE_A3, 8},    {NOTE_A2, 8},
        {NOTE_A2, 8},    {NOTE_G3, 8},    {NOTE_A2, 8},   {NOTE_A2, 8},    {NOTE_F3, 8},    {NOTE_A2, 8},
        {NOTE_A2, 8},    {NOTE_DS3, -2},  {NOTE_A2, 8},   {NOTE_A2, 8},    {NOTE_A3, 8},    {NOTE_A2, 8},
        {NOTE_A2, 8},    {NOTE_G3, 8},    {NOTE_A2, 8},   {NOTE_A2, 8},    {NOTE_F3, 8},    {NOTE_A2, 8},
        {NOTE_A2, 8},    {NOTE_DS3, 8},   {NOTE_A2, 8},   {NOTE_A2, 8},    {NOTE_E3, 8},    {NOTE_F3, 8},
        {NOTE_A2, 8},    {NOTE_A2, 8},    {NOTE_A3, 8},   {NOTE_A2, 8},    {NOTE_A2, 8},    {NOTE_G3, 8},
        {NOTE_A2, 8},    {NOTE_A2, 8},    {NOTE_A3, -16}, {NOTE_F3, -16},  {NOTE_D3, -16},  {NOTE_A3, -16},
        {NOTE_F3, -16},  {NOTE_D3, -16},  {NOTE_C4, -16}, {NOTE_A3, -16},  {NOTE_F3, -16},  {NOTE_A3, -16},
        {NOTE_F3, -16},  {NOTE_D3, -16},  {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_E3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_D3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_C3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_AS2, 8},   {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_B2, 8},    {NOTE_C3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_E3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_D3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_C3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_AS2, -2},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_E3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_D3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_C3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_AS2, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_B2, 8},   {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_E3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_D3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_AS2, -2},  {NOTE_CS3, 8},   {NOTE_CS3, 8},
        {NOTE_CS4, 8},   {NOTE_CS3, 8},   {NOTE_CS3, 8},  {NOTE_B3, 8},    {NOTE_CS3, 8},   {NOTE_CS3, 8},
        {NOTE_A3, 8},    {NOTE_CS3, 8},   {NOTE_CS3, 8},  {NOTE_G3, 8},    {NOTE_CS3, 8},   {NOTE_CS3, 8},
        {NOTE_GS3, 8},   {NOTE_A3, 8},    {NOTE_B2, 8},   {NOTE_B2, 8},    {NOTE_B3, 8},    {NOTE_B2, 8},
        {NOTE_B2, 8},    {NOTE_A3, 8},    {NOTE_B2, 8},   {NOTE_B2, 8},    {NOTE_G3, 8},    {NOTE_B2, 8},
        {NOTE_B2, 8},    {NOTE_F3, -2},   {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_E3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_D3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_C3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_AS2, 8},   {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_B2, 8},    {NOTE_C3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_E3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_D3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_B3, -16}, {NOTE_G3, -16},  {NOTE_E3, -16},  {NOTE_G3, -16},
        {NOTE_B3, -16},  {NOTE_E4, -16},  {NOTE_G3, -16}, {NOTE_B3, -16},  {NOTE_E4, -16},  {NOTE_B3, -16},
        {NOTE_G4, -16},  {NOTE_B4, -16},  {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_E3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_D3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_C3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_AS2, 8},   {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_B2, 8},    {NOTE_C3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_E3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_D3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_C3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_AS2, -2},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_E3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_D3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_C3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_AS2, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_B2, 8},   {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_E3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_D3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_FS3, -16}, {NOTE_DS3, -16}, {NOTE_B2, -16}, {NOTE_FS3, -16}, {NOTE_DS3, -16}, {NOTE_B2, -16},
        {NOTE_G3, -16},  {NOTE_D3, -16},  {NOTE_B2, -16}, {NOTE_DS4, -16}, {NOTE_DS3, -16}, {NOTE_B2, -16},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_E3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_D3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_C3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_AS2, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_B2, 8},   {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_E3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_D3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_AS2, -2},  {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_E3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_D3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_AS2, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_B2, 8},    {NOTE_C3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_E3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_D3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_E4, -16},  {NOTE_B3, -16},
        {NOTE_G3, -16},  {NOTE_G4, -16},  {NOTE_E4, -16}, {NOTE_G3, -16},  {NOTE_B3, -16},  {NOTE_D4, -16},
        {NOTE_E4, -16},  {NOTE_G4, -16},  {NOTE_E4, -16}, {NOTE_G3, -16},  {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_E3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_D3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_AS2, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_B2, 8},    {NOTE_C3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_E3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_D3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_C3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_AS2, -2},  {NOTE_A2, 8},   {NOTE_A2, 8},    {NOTE_A3, 8},    {NOTE_A2, 8},
        {NOTE_A2, 8},    {NOTE_G3, 8},    {NOTE_A2, 8},   {NOTE_A2, 8},    {NOTE_F3, 8},    {NOTE_A2, 8},
        {NOTE_A2, 8},    {NOTE_DS3, 8},   {NOTE_A2, 8},   {NOTE_A2, 8},    {NOTE_E3, 8},    {NOTE_F3, 8},
        {NOTE_A2, 8},    {NOTE_A2, 8},    {NOTE_A3, 8},   {NOTE_A2, 8},    {NOTE_A2, 8},    {NOTE_G3, 8},
        {NOTE_A2, 8},    {NOTE_A2, 8},    {NOTE_A3, -16}, {NOTE_F3, -16},  {NOTE_D3, -16},  {NOTE_A3, -16},
        {NOTE_F3, -16},  {NOTE_D3, -16},  {NOTE_C4, -16}, {NOTE_A3, -16},  {NOTE_F3, -16},  {NOTE_A3, -16},
        {NOTE_F3, -16},  {NOTE_D3, -16},  {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_E3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_D3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_C3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_AS2, 8},   {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_B2, 8},    {NOTE_C3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_E3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_D3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_C3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_AS2, -2},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_E3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_D3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_C3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_AS2, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_B2, 8},   {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_E3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_D3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_AS2, -2},  {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_E3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_D3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_AS2, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_B2, 8},    {NOTE_C3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_E3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_D3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_C3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_AS2, -2},  {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_E3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_D3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_C3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_AS2, 8},   {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_B2, 8},    {NOTE_C3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_E3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_D3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_B3, -16}, {NOTE_G3, -16},  {NOTE_E3, -16},  {NOTE_B2, -16},
        {NOTE_E3, -16},  {NOTE_G3, -16},  {NOTE_C4, -16}, {NOTE_B3, -16},  {NOTE_G3, -16},  {NOTE_B3, -16},
        {NOTE_G3, -16},  {NOTE_E3, -16},
    };
    playMelody(
        doom_e1m1, sizeof(doom_e1m1) / sizeof(doom_e1m1[0]), 60
    ); // TODO - Should be 160... But note durations seem to be off
}

Buzzer buzzer;

} // namespace lilka

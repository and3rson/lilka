#include <Arduino.h>

#include "buzzer.h"
#include "config.h"
#include "serial.h"
#include "Preferences.h"

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

    Preferences prefs;
    prefs.begin("sound", true);
    startupBuzzer = prefs.getBool("startupBuzzer", true);
    prefs.end();

    _stop();
    pinMode(LILKA_BUZZER, OUTPUT);
#    ifndef LILKA_NO_BUZZER_HELLO
    const Tone helloTune[] = {{NOTE_C3, 8}, {NOTE_C4, 8}, {NOTE_C5, 8}, {NOTE_C7, 4}, {0, 8}, {NOTE_C6, 4}};
    if (startupBuzzer) playMelody(helloTune, sizeof(helloTune) / sizeof(Tone), 160);
#    endif
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
    // delete[] currentMelody;
    // currentMelody = new Tone[length];
    memcpy(currentMelody, melody, length * sizeof(Tone));
    currentMelodyLength = length;
    currentMelodyTempo = tempo;
    // Serial.println("Melody task starting, length: " + String(length) + ", tempo: " + String(tempo) + ", first note frequency: " + String(melody[0].frequency));
    if (xTaskCreate(melodyTask, "melodyTask", 2048, this, 1, &melodyTaskHandle) != pdPASS) {
        serial_err("Failed to create melody task (not enough memory?)");
    }
    xSemaphoreGive(buzzerMutex);
#endif
}

void Buzzer::melodyTask(void* arg) {
    Buzzer* buzzer = static_cast<Buzzer*>(arg);
#if LILKA_VERSION < 2
    serial_err("Buzzer is not supported on this board");
#else
    // Serial.println("Melody task started");
    for (uint32_t i = 0; i < buzzer->currentMelodyLength; i++) {
        // Serial.println("Playing note " + String(i) + " with frequency " + String(buzzer->currentMelody[i].frequency));
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
    }
#endif
    xSemaphoreTake(buzzer->buzzerMutex, portMAX_DELAY);
#if LILKA_VERSION >= 2
    noTone(LILKA_BUZZER);
#endif
    // Release the mutex & delete task.
    buzzer->melodyTaskHandle = NULL;
    xSemaphoreGive(buzzer->buzzerMutex);
    vTaskDelete(NULL);
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
#if LILKA_VERSION >= 2
    noTone(LILKA_BUZZER);
#endif
    TaskHandle_t handle = melodyTaskHandle;
    if (handle != NULL) {
        melodyTaskHandle = NULL;
        // This can be called from within the task,
        // so we postpone the deletion of the task till the end of the function to avoid a deadlock
        vTaskDelete(handle);
    }
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

bool Buzzer::getStartupBuzzerEnabled() {
    return startupBuzzer;
}

void Buzzer::setStartupBuzzerEnabled(bool enable) {
    startupBuzzer = enable;
    saveSettings();
}

void Buzzer::saveSettings() {
    Preferences prefs;
    prefs.begin("sound", false);
    prefs.putBool("startupBuzzer", startupBuzzer);
    prefs.end();
}

Buzzer buzzer;

} // namespace lilka

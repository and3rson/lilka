// #include <lilka.h>
// #include <esp_wifi.h>
// #include <esp_bt.h>
// // #include "WiFi.h"
// #include "mixer.h"
// #include "config.h"
// #include "utils/acquire.h"
//
// Mixer::Mixer() :
//     xMutex(xSemaphoreCreateMutex()),
//     xQueue(xQueueCreate(CHANNEL_COUNT * MIXER_COMMAND_COUNT, sizeof(mixer_command_t))),
//     masterVolume(0.25) {
//     constexpr uint8_t pinCount = 3;
//     uint8_t pins[pinCount] = {LILKA_I2S_BCLK, LILKA_I2S_LRCK, LILKA_I2S_DOUT};
//     uint8_t funcs[pinCount] = {I2S0O_BCK_OUT_IDX, I2S0O_WS_OUT_IDX, I2S0O_SD_OUT_IDX};
//     for (int i = 0; i < pinCount; i++) {
//         gpio_pad_select_gpio(pins[i]);
//         gpio_set_direction((gpio_num_t)pins[i], GPIO_MODE_OUTPUT);
//         gpio_matrix_out(pins[i], funcs[i], false, false);
//     }
//
//     esp_i2s::i2s_config_t cfg = {
//         .mode = (esp_i2s::i2s_mode_t)(esp_i2s::I2S_MODE_MASTER | esp_i2s::I2S_MODE_TX),
//         .sample_rate = SAMPLE_RATE,
//         .bits_per_sample = esp_i2s::I2S_BITS_PER_SAMPLE_16BIT,
//         .channel_format = esp_i2s::I2S_CHANNEL_FMT_ONLY_LEFT,
//         .communication_format =
//             (esp_i2s::i2s_comm_format_t)(esp_i2s::I2S_COMM_FORMAT_STAND_I2S | esp_i2s::I2S_COMM_FORMAT_STAND_MSB),
//         .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
//         .dma_buf_count = 4,
//         .dma_buf_len = MIXER_BUFFER_SIZE,
//         .use_apll = false,
//         .tx_desc_auto_clear = true,
//     };
//     if (esp_i2s::i2s_driver_install(esp_i2s::I2S_NUM_0, &cfg, 0, NULL) != ESP_OK) {
//         lilka::serial_err("Failed to install I2S driver");
//         return;
//     }
//     if (esp_i2s::i2s_zero_dma_buffer(esp_i2s::I2S_NUM_0) != ESP_OK) {
//         lilka::serial_err("Failed to zero I2S DMA buffer");
//         return;
//     }
//
//     // Free up the core 0 for the mixer task
//     // WiFi.mode(WIFI_OFF);
//     // esp_wifi_stop();
//     // esp_wifi_deinit();
//     // esp_bt_controller_disable();
//     // esp_bt_controller_deinit();
//
//     xTaskCreatePinnedToCore(
//         [](void* pvParameters) {
//             static_cast<Mixer*>(pvParameters)->mixerTask();
//             vTaskDelete(NULL);
//         },
//         "mixerTask",
//         8192,
//         this,
//         1,
//         nullptr,
//         0
//     );
//
//     xSemaphoreGive(xMutex);
// }
//
// Mixer::~Mixer() {
//     vQueueDelete(xQueue);
//     vSemaphoreDelete(xMutex);
// }
//
// void Mixer::sendCommand(const mixer_command_t command) {
//     xQueueSend(xQueue, &command, portMAX_DELAY);
// }
//
// void Mixer::start(uint8_t channelIndex, waveform_t waveform, float frequency, float volume, effect_t effect) {
//     mixer_command_t cmd;
//     cmd.channelIndex = channelIndex;
//     cmd.type = MIXER_COMMAND_SET_WAVEFORM;
//     cmd.waveform = waveform;
//     sendCommand(cmd);
//     cmd.type = MIXER_COMMAND_SET_FREQUENCY;
//     cmd.frequency = frequency;
//     sendCommand(cmd);
//     cmd.type = MIXER_COMMAND_SET_VOLUME;
//     cmd.volume = volume;
//     sendCommand(cmd);
//     cmd.type = MIXER_COMMAND_SET_EFFECT;
//     cmd.effect = effect;
//     sendCommand(cmd);
// }
//
// void Mixer::stop() {
//     // start(NULL, 0);
//     for (uint8_t channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
//         mixer_command_t request = {channelIndex, MIXER_COMMAND_SET_OFF};
//         xQueueSend(xQueue, &request, portMAX_DELAY);
//     }
// }
//
// void Mixer::reset() {
//     for (uint8_t channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
//         mixer_command_t cmd;
//         cmd.channelIndex = channelIndex;
//         cmd.type = MIXER_COMMAND_SET_WAVEFORM;
//         cmd.waveform = WAVEFORM_SQUARE;
//         sendCommand(cmd);
//         cmd.type = MIXER_COMMAND_SET_FREQUENCY;
//         cmd.frequency = 0.0f;
//         sendCommand(cmd);
//         cmd.type = MIXER_COMMAND_SET_VOLUME;
//         cmd.volume = 1.0f;
//         sendCommand(cmd);
//         cmd.type = MIXER_COMMAND_SET_EFFECT;
//         cmd.effect = {EFFECT_TYPE_NONE, 0};
//         sendCommand(cmd);
//     }
// }
//
// void Mixer::mixerTask() {
//     int16_t audioBuffer[MIXER_BUFFER_SIZE];
//     int16_t channel0AudioBuffer[MIXER_BUFFER_SIZE];
//     int16_t channel1AudioBuffer[MIXER_BUFFER_SIZE];
//     int16_t channel2AudioBuffer[MIXER_BUFFER_SIZE];
//     int16_t* channelAudioBuffers[CHANNEL_COUNT] = {channel0AudioBuffer, channel1AudioBuffer, channel2AudioBuffer};
//
//     while (1) { // TODO: make this stoppable
//         // TODO: Handle envelopes, effects and stuff in this loop
//         // Check if queue has a new pattern and event index to play
//         // start_request_t request;
//         mixer_command_t command;
//         while (xQueueReceive(xQueue, &command, 0) == pdTRUE) {
//             // channelStates[request.channelIndex] = {request.waveform, request.frequency, request.volume, request.effect};
//             if (command.type == MIXER_COMMAND_SET_WAVEFORM) {
//                 synth.setWaveform(command.channelIndex, command.waveform);
//             } else if (command.type == MIXER_COMMAND_SET_FREQUENCY) {
//                 synth.setFrequency(command.channelIndex, command.frequency);
//             } else if (command.type == MIXER_COMMAND_SET_VOLUME) {
//                 synth.setVolume(command.channelIndex, command.volume);
//             } else if (command.type == MIXER_COMMAND_SET_EFFECT) {
//                 synth.setEffect(command.channelIndex, command.effect);
//             } else if (command.type == MIXER_COMMAND_SET_OFF) {
//                 synth.setOff(command.channelIndex);
//             }
//         }
//         // Mix the channels
//         float _masterVolume;
//         {
//             Acquire lock(xMutex);
//             _masterVolume = masterVolume;
//         }
//         int64_t mixStart = millis();
//         synth.render(audioBuffer, channelAudioBuffers, MIXER_BUFFER_SIZE, _masterVolume);
//         int64_t mixEnd = millis();
//         // Check if time spent mixing is more than the duration of the buffer
//         // Duration of the buffer in microseconds: 1 / SAMPLE_RATE * 1000 * MIXER_BUFFER_SIZE
//         // For 256-sample buffer at 8 kHz, it is 32000 microseconds
//         if (mixEnd - mixStart > MIXER_BUFFER_DURATION_MS) {
//             lilka::serial_err(
//                 "Mixer buffer underrun! Spent %lld ms mixing, had %d ms", mixEnd - mixStart, MIXER_BUFFER_DURATION_MS
//             );
//         } else {
//             // lilka::serial_log("Mixer buffer mix took %lld ms", mixEnd - mixStart);
//         }
//
//         size_t bytesWritten = 0;
//         esp_i2s::i2s_write(esp_i2s::I2S_NUM_0, audioBuffer, MIXER_BUFFER_SIZE * 2, &bytesWritten, portMAX_DELAY);
//         {
//             Acquire lock(xMutex);
//             memcpy(audioBufferCopy, audioBuffer, sizeof(int16_t) * MIXER_BUFFER_SIZE);
//             for (uint8_t channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
//                 memcpy(
//                     channelAudioBuffersCopy[channelIndex],
//                     channelAudioBuffers[channelIndex],
//                     sizeof(int16_t) * MIXER_BUFFER_SIZE
//                 );
//             }
//         }
//         synth.advanceTime(bytesWritten / 2);
//         // taskYIELD();
//         vTaskDelay(1); // Needed to wait for UI to be able to acquire the mutex... Probably can go back to taskYIELD now
//     }
//
//     // TODO: Make this part reachable & deinit I2S
//     i2s_driver_uninstall(esp_i2s::I2S_NUM_0);
// }
//
// int16_t Mixer::readBuffer(int16_t* targetBuffer) {
//     Acquire lock(xMutex);
//     memcpy(targetBuffer, audioBufferCopy, sizeof(int16_t) * MIXER_BUFFER_SIZE);
//     return MIXER_BUFFER_SIZE;
// }
//
// int16_t Mixer::readBuffer(int16_t* targetBuffer, uint8_t channelIndex) {
//     Acquire lock(xMutex);
//     memcpy(targetBuffer, channelAudioBuffersCopy[channelIndex], sizeof(int16_t) * MIXER_BUFFER_SIZE);
//     return MIXER_BUFFER_SIZE;
// }
//
// void Mixer::setMasterVolume(float volume) {
//     Acquire lock(xMutex);
//     masterVolume = fmaxf(0.0f, fminf(1.0f, volume));
// }
//
// float Mixer::getMasterVolume() {
//     Acquire lock(xMutex);
//     return masterVolume;
// }

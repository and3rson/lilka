//
// ModPlayer – програвач MOD-файлів на базі бібліотеки ESP8266Audio для Lilka зі звуковим модулем PCM5102.
// Автори: Олексій "Alder" Деркач (https://github.com/alder) та Андрій "and3rson" Дунай (https://github.com/and3rson)
// Детальніше про формат та його історію — https://en.wikipedia.org/wiki/MOD_(file_format)
// Найбільший архів з музикою у форматі MOD – https://modarchive.org/
//

#include <AudioGeneratorMOD.h>
#include <AudioOutputI2S.h>
#include <AudioFileSourceSD.h>
#include <AudioFileSourceBuffer.h>
#include <AudioFileSourcePROGMEM.h>

#include "modplayer.h"

ModPlayerApp::ModPlayerApp(String path) :
    App("MODPlayer", 0, 0, lilka::display.width(), lilka::display.height()),
    playerCommandQueue(xQueueCreate(8, sizeof(PlayerCommand))) {
    setFlags(AppFlags::APP_FLAG_FULLSCREEN);
    // This app will run on core 1 since it's already more busy with drawing Keira stuff.
    // However, player task will run on core 0 since it's less busy. /AD
    setCore(1);
    // Get local file name (path minus mount point)
    fileName = lilka::fileutils.getLocalPathInfo(path).path;
    playerMutex = xSemaphoreCreateMutex();
    xSemaphoreGive(playerMutex);
}

void ModPlayerApp::run() {
    // Start the player task on core 0
    xTaskCreatePinnedToCore(
        [](void* arg) {
            ModPlayerApp* app = static_cast<ModPlayerApp*>(arg);
            app->playTask();
        },
        "MODPlayer",
        8192,
        this,
        1,
        nullptr,
        0
    );

    while (1) {
        mainWindow();
        xSemaphoreTake(playerMutex, portMAX_DELAY);
        PlayerTaskData info = playerTaskData;
        xSemaphoreGive(playerMutex);

        lilka::State state = lilka::controller.getState();
        if (state.a.justPressed) {
            PlayerCommand command = {.type = CMD_SET_PAUSED, .isPaused = !info.isPaused};
            xQueueSend(playerCommandQueue, &command, portMAX_DELAY);
        };
        if (state.b.justPressed) {
            // Exit app
            PlayerCommand command = {.type = CMD_STOP};
            xQueueSend(playerCommandQueue, &command, portMAX_DELAY);
            break;
        };
        if (state.up.justPressed) {
            PlayerCommand command = {.type = CMD_SET_GAIN, .gain = info.gain + 0.25f};
            xQueueSend(playerCommandQueue, &command, portMAX_DELAY);
        };
        if (state.down.justPressed) {
            PlayerCommand command = {.type = CMD_SET_GAIN, .gain = info.gain - 0.25f};
            xQueueSend(playerCommandQueue, &command, portMAX_DELAY);
        };
    };
}

void ModPlayerApp::mainWindow() {
    canvas->fillScreen(lilka::colors::Black);

    xSemaphoreTake(playerMutex, portMAX_DELAY);
    bool shouldDrawAnalyzer = !playerTaskData.isPaused && !playerTaskData.isFinished;
    xSemaphoreGive(playerMutex);
    if (shouldDrawAnalyzer) {
        int16_t analyzerBuffer[ANALYZER_BUFFER_SIZE];
        xSemaphoreTake(playerMutex, portMAX_DELAY);
        playerTaskData.analyzer->readBuffer(analyzerBuffer);
        int16_t head = playerTaskData.analyzer->getBufferHead();
        float gain = playerTaskData.gain;
        xSemaphoreGive(playerMutex);

        int16_t prevX, prevY;
        int16_t width = canvas->width();
        int16_t height = canvas->height();

        constexpr int16_t HUE_SPEED_DIV = 4;
        constexpr int16_t HUE_SCALE = 4;
        int16_t yCenter = height * 5 / 7;

        int64_t time = millis();

        for (int i = 0; i < ANALYZER_BUFFER_SIZE; i += 4) {
            int x = i * width / ANALYZER_BUFFER_SIZE;
            int index = (i + head) % ANALYZER_BUFFER_SIZE;
            float amplitude = static_cast<float>(analyzerBuffer[index]) / 32768 * gain;
            int y = yCenter + static_cast<int>(amplitude * height / 2);
            if (i > 0) {
                int16_t hue = (time / HUE_SPEED_DIV + i / HUE_SCALE) % 360;
                canvas->drawLine(prevX, prevY, x, y, lilka::display.color565hsv(hue, 100, 100));
            }
            prevX = x;
            prevY = y;
        }
    }

    xSemaphoreTake(playerMutex, portMAX_DELAY);
    // Copy playerTaskData to prevent blocking the mutex for too long
    PlayerTaskData info = this->playerTaskData;
    xSemaphoreGive(playerMutex);

    canvas->setFont(FONT_9x15);
    canvas->setTextBound(32, 32, canvas->width() - 64, canvas->height() - 32);
    canvas->setTextColor(lilka::colors::White);
    canvas->setCursor(32, 32 + 15);
    canvas->println("Програвач MOD");
    canvas->println("------------------------");
    canvas->println("A - Відтворення / пауза");
    canvas->setFont(FONT_9x15_SYMBOLS);
    canvas->print("↑ / ↓");
    canvas->setFont(FONT_9x15);
    canvas->println(" - Гучність");
    canvas->println("B - Вихід");
    canvas->println("------------------------");
    canvas->println("Гучність: " + String(info.gain));
    if (info.isFinished) canvas->println("Трек закінчився");

    lilka::Canvas titleCanvas(canvas->width(), 20);
    titleCanvas.fillScreen(lilka::colors::Black);
    titleCanvas.setFont(FONT_9x15);
    titleCanvas.setTextColor(lilka::display.color565hsv((millis() * 30) % 360, 100, 100));
    titleCanvas.drawTextAligned(
        fileName.c_str(), titleCanvas.width() / 2, titleCanvas.height() / 2, lilka::ALIGN_CENTER, lilka::ALIGN_CENTER
    );
    const uint16_t* titleCanvasFB = titleCanvas.getFramebuffer();
    uint16_t yOffset = canvas->height() - titleCanvas.height() - 8;
    float time = millis() / 1500.0f;
    for (int16_t x = 0; x < titleCanvas.width(); x++) {
        int16_t yShift = sinf(time + x / 25.0f) * 4 + yOffset;
        for (int16_t y = 0; y < titleCanvas.height(); y++) {
            uint16_t color = titleCanvasFB[x + y * titleCanvas.width()];
            if (color) {
                canvas->drawPixel(x, y + yShift, color);
            }
        }
    }
    queueDraw();
}

void ModPlayerApp::playTask() {
    // Source/sink order:
    // modSource -> modBufferSource -> mod -> analyzer -> out

    // Create output
    AudioOutputI2S* out = new AudioOutputI2S();
    std::unique_ptr<AudioOutputI2S> outPtr(out); // Auto-delete on task return
    out->SetPinout(LILKA_I2S_BCLK, LILKA_I2S_LRCK, LILKA_I2S_DOUT);

    // Create output analyzer
    playerTaskData.analyzer = new AudioOutputAnalyzer(out);
    std::unique_ptr<AudioOutputAnalyzer> analyzerPtr(playerTaskData.analyzer);

    // Create source
    AudioFileSourceSD* modSource = new AudioFileSourceSD(fileName.c_str());
    std::unique_ptr<AudioFileSource> modSourcePtr(modSource);

    // Previously we directly fed modSource to mod, but it caused a lot of stuttering due to SPI bus contention between SD and display.
    // However, MOD files are small enough to fit in memory, so we can read the whole file into a buffer and then feed it to mod!
    // Why not use AudioFileSourceBuffer? Because it doesn't seem to work properly and does weird things, and its code is barely readable.
    // So we read the file manually and then use AudioFileSourcePROGMEM to feed it to mod. (In ESP32, PROGMEM is an outdated misnomer: it works with RAM too)
    // TODO: Задокументувати нюанс SPI bus contention між SD та дисплеєм. /AD
    uint8_t* modFileData = new uint8_t[modSource->getSize()];
    std::unique_ptr<uint8_t[]> modFileDataPtr(modFileData);
    // Read the whole file into buffer
    modSource->read(modFileData, modSource->getSize());
    // Create source buffer
    AudioFileSourcePROGMEM* modBufferSource = new AudioFileSourcePROGMEM(modFileData, modSource->getSize());
    std::unique_ptr<AudioFileSource> modBufferSourcePtr(modBufferSource);

    // Create MOD player
    AudioGeneratorMOD* mod = new AudioGeneratorMOD();
    std::unique_ptr<AudioGeneratorMOD> modPtr(mod);
    mod->begin(modBufferSource, playerTaskData.analyzer);

    xSemaphoreTake(playerMutex, portMAX_DELAY);
    playerTaskData.isPaused = false;
    playerTaskData.isFinished = false;
    playerTaskData.gain = 1.0f;
    xSemaphoreGive(playerMutex);

    while (1) {
        // Check for new command
        PlayerCommand command;
        if (xQueueReceive(playerCommandQueue, &command, 0) == pdTRUE) {
            switch (command.type) {
                case CMD_SET_PAUSED:
                    xSemaphoreTake(playerMutex, portMAX_DELAY);
                    playerTaskData.isPaused = command.isPaused;
                    xSemaphoreGive(playerMutex);
                    break;
                case CMD_SET_GAIN:
                    xSemaphoreTake(playerMutex, portMAX_DELAY);
                    playerTaskData.gain = command.gain;
                    xSemaphoreGive(playerMutex);
                    if (playerTaskData.gain < 0) playerTaskData.gain = 0;
                    if (playerTaskData.gain > 4) playerTaskData.gain = 4;
                    out->SetGain(playerTaskData.gain);
                    break;
                case CMD_STOP:
                    xSemaphoreTake(playerMutex, portMAX_DELAY);
                    playerTaskData.isFinished = true;
                    xSemaphoreGive(playerMutex);
                    break;
            }
        }
        xSemaphoreTake(playerMutex, portMAX_DELAY);
        if (playerTaskData.isFinished) {
            mod->stop();
            xSemaphoreGive(playerMutex);
            break;
        }
        if (!playerTaskData.isPaused) {
            if (!mod->loop()) {
                mod->stop();
                playerTaskData.isFinished = true;
                xSemaphoreGive(playerMutex);
                break;
            }
        }
        xSemaphoreGive(playerMutex);
        taskYIELD(); // Give app a chance to acquire playerMutex
    }

    // Tasks must ALWAYS delete themselves before exiting, or we're get IllegalInstruction panic
    vTaskDelete(NULL);
    lilka::serial_log("Player task exited");
}

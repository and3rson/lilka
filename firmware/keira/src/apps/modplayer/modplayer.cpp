//
// ModPlayer – простий програвач MOD-файлів на базі бібліотеки ESP8266Audio для Lilka зі звуковим модулем PCM5102.
// Автор: Олексій "Alder" Деркач
// Github: https://github.com/alder
// Детальніше про формат та його історію — https://en.wikipedia.org/wiki/MOD_(file_format)
// Найбільший архів з музикою у форматі MOD – https://modarchive.org/
//
#include "modplayer.h"

ModPlayerApp::ModPlayerApp(String path) :
    App("MODPlayer", 0, 0, lilka::display.width(), lilka::display.height()), path(path) {
    setFlags(AppFlags::APP_FLAG_FULLSCREEN);
}

void ModPlayerApp::run() {
    modSource = new AudioFileSourceSD(path.substring(3).c_str());
    out = new AudioOutputI2S();
    //bclk = BCK = GPIO42, wclk = LRCK = GPIO1, dout = DIN = GPIO2
    out->SetPinout(42, 1, 2);
    mod = new AudioGeneratorMOD();
    mod->begin(modSource, out);

    float gain = 1.0;
    out->SetGain(gain);

    bool isPaused = false;
    bool isFinished = false;
    String fileName = path.substring(4);
    mainWindow(fileName, gain, isFinished);

    while (1) {
        if (mod->isRunning()) {
            if (!isPaused) {
                if (!mod->loop()) mod->stop();
            }
        } else {
            isFinished = true;
            mainWindow(fileName, gain, isFinished);
        };
        lilka::State state = lilka::controller.getState();
        if (state.a.justPressed) {
            isPaused = !isPaused;
        };
        if (state.b.justPressed) {
            break;
        };
        if (state.up.justPressed) {
            gain = gain + 0.25;
            if (gain > 4) gain = 4.0;
            out->SetGain(gain);
            mainWindow(fileName, gain, isFinished);
        };
        if (state.down.justPressed) {
            gain = gain - 0.25;
            if (gain < 0) gain = 0;
            out->SetGain(gain);
            mainWindow(fileName, gain, isFinished);
        };
    };
}

void ModPlayerApp::mainWindow(String fileName, float gain, bool isFinished) {
    canvas->fillScreen(lilka::colors::Black);
    canvas->setFont(FONT_9x15);
    canvas->setTextBound(32, 32, canvas->width() - 64, canvas->height() - 64);
    canvas->setTextColor(lilka::colors::White);
    canvas->setCursor(32, 32 + 15);
    canvas->println("Програвач MOD");
    canvas->println("------------------------");
    canvas->println("[A] - Play / Pause");
    canvas->println("[Up] / [Down] - Гучність");
    canvas->println("[B] - Вийти");
    canvas->println("------------------------");
    canvas->println("Файл: " + fileName);
    if (isFinished) canvas->println("Трек закінчився");
    canvas->setCursor(32, canvas->height() - 38);
    canvas->println("Гучність: " + String(gain));
    queueDraw();
}

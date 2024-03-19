#include "ftp_server.h"
#include "WiFi.h"

FTPServerApp::FTPServerApp() : App("FTP Server") {
}

void FTPServerApp::run() {
    char password[9];
    // Generate random 8-char password
    for (int i = 0; i < 8; i++) {
        if (random(0, 2) == 0) {
            password[i] = random(48, 57);
        } else {
            password[i] = random(97, 122);
        }
    }
    password[8] = '\0';

    ftpSrv.begin("lilka", password);

    canvas->fillScreen(0);
    canvas->setCursor(32, 32);
    canvas->setTextBound(16, 16, canvas->width() - 32, canvas->height() - 32);
    canvas->printf(
        "FTP Server запущений\nIP: %s\nПорт: %d\nНатисніть A для виходу",
        WiFi.localIP().toString().c_str(),
        FTP_CTRL_PORT
    );
    queueDraw();

    while (true) {
        lilka::State state = lilka::controller.getState();
        if (state.a.justPressed) {
            break;
        }
        ftpSrv.handleFTP();
        taskYIELD();
    }
}

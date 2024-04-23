#include <WiFi.h>
#include <Preferences.h>
#include <FtpServer.h>

#include "ftp_server.h"
#include "servicemanager.h"
#include "services/network.h"

FTPServerApp::FTPServerApp() : App("FTP Server") {
}

void FTPServerApp::run() {
    String password = getPassword();
    if (password.isEmpty()) {
        password = createPassword();
    }

    lilka::fileutils.initSD();

    NetworkService* networkService = ServiceManager::getInstance()->getService<NetworkService>("network");

    if (networkService->getNetworkState() != NETWORK_STATE_ONLINE) {
        canvas->fillScreen(0);
        canvas->setCursor(16, 16);
        canvas->setTextBound(16, 16, canvas->width() - 32, canvas->height() - 32);
        canvas->printf("WiFi не підключено\n"
                       "\n"
                       "Підключіться до мережі\n"
                       "та спробуйте ще раз\n"
                       "\n"
                       "Натисніть [A] для виходу");
        queueDraw();

        while (true) {
            lilka::State state = lilka::controller.getState();
            if (state.a.justPressed) {
                return;
            }
            taskYIELD();
        }
    }

    FtpServer ftpSrv;
    ftpSrv.begin("lilka", password.c_str());

    while (1) {
        canvas->fillScreen(0);
        canvas->setCursor(16, 16);
        canvas->setTextBound(16, 16, canvas->width() - 32, canvas->height() - 32);
        canvas->printf(
            "FTP сервер запущений\n"
            "\n"
            "IP: %s\n"
            "Порт: %d\n"
            "Користувач: %s\n"
            "Пароль: %s\n"
            "\n"
            "Натисніть [A] для виходу\n"
            "Натисніть [SELECT]\n"
            "для зміни паролю",
            WiFi.localIP().toString().c_str(),
            FTP_CMD_PORT,
            "lilka",
            password.c_str()
        );
        queueDraw();

        while (true) {
            lilka::State state = lilka::controller.getState();
            if (state.a.justPressed) {
                return;
            } else if (state.select.justPressed) {
                password = createPassword();
                ftpSrv.credentials("lilka", password.c_str());
                break;
            } else {
                ftpSrv.handleFTP();
                taskYIELD();
            }
        }
    }
}

String FTPServerApp::createPassword() {
    // Generate random 6-char password
    const uint8_t pwdLen = 6;
    static char password[pwdLen + 1];
    for (int i = 0; i < pwdLen; i++) {
        if (random(0, 2) == 0) {
            password[i] = random(48, 57);
        } else {
            password[i] = random(97, 122);
        }
    }
    password[pwdLen] = '\0';

    Preferences prefs;
    prefs.begin("keira", false);
    prefs.putString("password", password);
    prefs.end();

    return String(password);
}

String FTPServerApp::getPassword() {
    Preferences prefs;
    prefs.begin("keira", true);
    String password;
    if (!prefs.isKey("password")) {
        password = "";
    } else {
        password = prefs.getString("password", "");
    }
    prefs.end();

    return password;
}

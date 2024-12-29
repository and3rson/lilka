#include "ftp.h"
#include "Preferences.h"
#include "servicemanager.h"
#include "network.h"

FTPService::FTPService() : Service("ftp") {
    Preferences prefs;
    prefs.begin(FTP_SETTINGS, true);
    enabled = prefs.getBool("enabled", false);
    password = prefs.getString("password", "");
    prefs.end();
    if (password.isEmpty()) {
        createPassword();
    }

    networkService = ServiceManager::getInstance()->getService<NetworkService>("network");
    lilka::fileutils.initSD();
}

FTPService::~FTPService() {
    if (ftpServer) {
        delete ftpServer;
    }
}

void FTPService::run() {
    bool wasOnline = false;
    while (true) {
        if (!networkService) {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        bool isOnline = networkService->getNetworkState() == NetworkState::NETWORK_STATE_ONLINE;
        if ((enabled && isOnline) && !wasOnline) {
            ftpServer = new FtpServer();
            ftpServer->begin(getUser().c_str(), getPassword().c_str());
            wasOnline = true;
        } else if ((!enabled || !isOnline) && wasOnline) {
            ftpServer->end();
            delete ftpServer;
            ftpServer = nullptr;
            wasOnline = false;
        }

        if (enabled && isOnline) {
            ftpServer->handleFTP();
            taskYIELD();
        } else {
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
    }
}

bool FTPService::getEnabled() {
    return enabled;
}

void FTPService::setEnabled(bool enabled) {
    this->enabled = enabled;
    Preferences prefs;
    prefs.begin(FTP_SETTINGS, false);
    prefs.putBool("enabled", enabled);
    prefs.end();
}

String FTPService::getUser() {
    return user;
}

String FTPService::getPassword() {
    return password;
}

void FTPService::createPassword() {
    char pwd[FTP_PASSWORD_LENGTH + 1];
    for (int i = 0; i < FTP_PASSWORD_LENGTH; i++) {
        pwd[i] = random(0, 2) == 0 ? random(48, 57) : random(97, 122);
    }
    pwd[FTP_PASSWORD_LENGTH] = 0;

    Preferences prefs;
    prefs.begin(FTP_SETTINGS, false);
    prefs.putString("password", pwd);
    prefs.end();

    password = String(pwd);

    if (ftpServer) {
        ftpServer->credentials(user.c_str(), password.c_str());
    }
}

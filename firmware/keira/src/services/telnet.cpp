#include <EscapeCodes.h>

#include "telnet.h"
#include "servicemanager.h"
#include "network.h"

#define STR_HELPER(x) #x
#define STR(x)        STR_HELPER(x)

ESPTelnet telnet;
EscapeCodes ansi;

TelnetService::TelnetService() : Service("telnet") {
}

TelnetService::~TelnetService() {
}

typedef struct {
    const char* command;
    void (*function)(String);
} Command;

Command commands[] = {
    {
        "help",
        [](String args) {
            telnet.println("Доступні команди:");
            telnet.println("  help          - показати цей список команд");
            telnet.println("  reboot        - перезавантажити пристрій");
            telnet.println("  uptime        - показати час роботи пристрою");
            telnet.println("  free          - показати стан пам'яті");
            telnet.println("  ls [DIR]      - показати список файлів на SD-картці");
            telnet.println("  find [TEXT]   - знайти файли на SD-картці, які містять TEXT в назві");
            telnet.println("  exit          - розірвати з'єднання");
        },
    },
    {
        "reboot",
        [](String args) {
            telnet.println("Система перезавантажується...");
            telnet.disconnectClient();
            esp_restart();
        },
    },
    {
        "uptime",
        [](String args) { telnet.println("Uptime: " + String(millis() / 1000) + " seconds"); },
    },
    {
        "free",
        [](String args) {
            telnet.println("Heap: " + String(ESP.getFreeHeap()) + " / " + String(ESP.getHeapSize()) + " bytes free");
            telnet.println("PSRAM: " + String(ESP.getFreePsram()) + " / " + String(ESP.getPsramSize()) + " bytes free");
        },
    },
    {
        "ls",
        [](String args) {
            File dir = SD.open(args.isEmpty() ? "/" : ("/" + args));
            int count = 0;
            if (!dir) {
                telnet.println("Не вдалося відкрити директорію: " + args);
                return;
            }
            while (File file = dir.openNextFile()) {
                count++;
                file.isDirectory() ? telnet.print("DIR ") : telnet.print("FILE");
                telnet.print("  ");
                telnet.printf(
                    "%8s  ", !file.isDirectory() ? lilka::fileutils.getHumanFriendlySize(file.size()).c_str() : "-"
                );
                telnet.println(file.name());
                file.close();
            }
            dir.close();
            telnet.println("Знайдено " + String(count) + " файлів");
        },
    },
    {
        "find",
        [](String args) {
            std::vector<String> dirs;
            dirs.push_back("/");
            int count = 0;
            while (!dirs.empty()) {
                String dir = dirs.back();
                dirs.pop_back();
                File d = SD.open(dir);
                if (!d) {
                    telnet.println("Не вдалося відкрити директорію: " + dir);
                    continue;
                }
                while (File file = d.openNextFile()) {
                    String fullPath = (!dir.equals("/") ? dir : "") + "/" + file.name();
                    if (file.isDirectory()) {
                        dirs.push_back(fullPath);
                    } else if (args.isEmpty() || String(file.name()).indexOf(args) != -1) {
                        count++;
                        telnet.print("FILE  ");
                        telnet.printf("%8s  ", lilka::fileutils.getHumanFriendlySize(file.size()).c_str());
                        telnet.println(fullPath);
                    }
                    file.close();
                }
                d.close();
            }
            telnet.println("Знайдено " + String(count) + " файлів");
        },
    },
    {
        "exit",
        [](String args) { telnet.disconnectClient(); },
    },
};

void TelnetService::run() {
    NetworkService* network = ServiceManager::getInstance()->getService<NetworkService>("network");

    telnet.onConnectionAttempt([](String ip) {
        lilka::serial_log("TelnetService: %s attempting to connect", ip.c_str());
    });
    telnet.onConnect([](String ip) {
        lilka::serial_log("TelnetService: %s connected", ip.c_str());
        telnet.print(ansi.cls());
        telnet.print(ansi.home());
        telnet.println(ansi.setBG(ANSI_BLUE) + "      " + ansi.reset() + " Keira OS @ Lilka v" STR(LILKA_VERSION));
        telnet.println(ansi.setBG(ANSI_YELLOW) + "      " + ansi.reset() + " Слава Україні! ");
        telnet.println();
        telnet.print("> ");
    });
    telnet.onReconnect([](String ip) { lilka::serial_log("TelnetService: %s reconnected", ip.c_str()); });
    telnet.onDisconnect([](String ip) { lilka::serial_log("TelnetService: %s disconnected", ip.c_str()); });
    telnet.onInputReceived([](String input) {
        lilka::serial_log("TelnetService: received text: %s", input.c_str());
        input.trim();
        if (!input.isEmpty()) {
            int spaceIndex = input.indexOf(' ');
            if (spaceIndex == -1) {
                spaceIndex = input.length();
            }
            String command = input.substring(0, spaceIndex);
            command.toLowerCase();
            command.trim();
            String args = input.substring(spaceIndex + 1);
            args.trim();
            Command* cmd = &commands[0];
            for (int i = 0; i < sizeof(commands) / sizeof(Command); i++) {
                if (command.equals(commands[i].command)) {
                    cmd = &commands[i];
                    break;
                }
            }
            cmd->function(args);
        }
        telnet.print("> ");
    });

    bool wasOnline = false;
    while (1) {
        if (!network) {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        bool isOnline = network->getNetworkState() == NetworkState::NETWORK_STATE_ONLINE;
        if (!wasOnline && isOnline) {
            wasOnline = true;
            // Start telnet server
            telnet.begin(23);
        } else if (wasOnline && !isOnline) {
            wasOnline = false;
            // Stop telnet server
            telnet.stop();
        }
        if (isOnline) {
            telnet.loop();
        } else {
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
    }
}

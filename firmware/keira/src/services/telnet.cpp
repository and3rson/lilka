#include <EscapeCodes.h>

#include "telnet.h"
#include "Preferences.h"
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
    void (*function)(std::vector<String>);
} Command;

Command commands[] = {
    {
        "help",
        [](std::vector<String> args) {
            telnet.println("Доступні команди:");
            telnet.println("  help               - показати цей список команд");
            telnet.println("  reboot             - перезавантажити пристрій");
            telnet.println("  uptime             - показати час роботи пристрою");
            telnet.println("  free               - показати стан пам'яті");
            telnet.println("  ls [DIR]           - показати список файлів на SD-картці");
            telnet.println("  find [TEXT]        - знайти файли на SD-картці, які містять TEXT в назві");
            telnet.println("  nvs get [NS] [KEY] - отримати значення ключа з NVS");
            telnet.println("  nvs rm [NS] [KEY]  - видалити ключ з NVS");
            telnet.println("  nvs rm [NS]        - видалити всі ключі з namespace в NVS");
            telnet.println("  exit               - розірвати з'єднання");
        },
    },
    {
        "reboot",
        [](std::vector<String> args) {
            telnet.println("Система перезавантажується...");
            telnet.disconnectClient();
            esp_restart();
        },
    },
    {
        "uptime",
        [](std::vector<String> args) { telnet.println("Uptime: " + String(millis() / 1000) + " seconds"); },
    },
    {
        "free",
        [](std::vector<String> args) {
            telnet.println("Heap: " + String(ESP.getFreeHeap()) + " / " + String(ESP.getHeapSize()) + " bytes free");
            telnet.println("PSRAM: " + String(ESP.getFreePsram()) + " / " + String(ESP.getPsramSize()) + " bytes free");
        },
    },
    {
        "ls",
        [](std::vector<String> args) {
            String dirName = args.empty() ? "/" : ("/" + args[0]);
            File dir = SD.open(dirName);
            int count = 0;
            if (!dir) {
                telnet.println("Не вдалося відкрити директорію: " + dirName);
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
        [](std::vector<String> args) {
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
                    } else if (args.empty() || String(file.name()).indexOf(args[0]) != -1) {
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
        "nvs",
        [](std::vector<String> args) {
            if (args.size() == 0) {
                telnet.println("Помилка: не вказано підкоманду");
                return;
            }

            String subcommand = args[0];
            subcommand.toLowerCase();

            if (subcommand == "get") {
                if (args.size() != 3) {
                    telnet.println("Помилка: невірна кількість параметрів");
                    return;
                }
                String ns = args[1];
                String key = args[2];
                Preferences prefs;
                prefs.begin(ns.c_str(), true);
                if (!prefs.isKey(key.c_str())) {
                    telnet.println("Помилка: ключ не знайдено");
                } else {
                    PreferenceType type = prefs.getType(key.c_str());
                    String typeStr;
                    String valueStr;
                    // Ah sh1t, here we go again
                    switch (type) {
                        case PreferenceType::PT_I8: {
                            typeStr = "int8";
                            valueStr = String(prefs.getChar(key.c_str()));
                            break;
                        }
                        case PreferenceType::PT_U8: {
                            typeStr = "uint8";
                            valueStr = String(prefs.getUChar(key.c_str()));
                            break;
                        }
                        case PreferenceType::PT_I16: {
                            typeStr = "int16";
                            valueStr = String(prefs.getShort(key.c_str()));
                            break;
                        }
                        case PreferenceType::PT_U16: {
                            typeStr = "uint16";
                            valueStr = String(prefs.getUShort(key.c_str()));
                            break;
                        }
                        case PreferenceType::PT_I32: {
                            typeStr = "int32";
                            valueStr = String(prefs.getInt(key.c_str()));
                            break;
                        }
                        case PreferenceType::PT_U32: {
                            typeStr = "uint32";
                            valueStr = String(prefs.getUInt(key.c_str()));
                            break;
                        }
                        case PreferenceType::PT_I64: {
                            typeStr = "int64";
                            valueStr = String(prefs.getLong(key.c_str()));
                            break;
                        }
                        case PreferenceType::PT_U64: {
                            typeStr = "uint64";
                            valueStr = String(prefs.getULong(key.c_str()));
                            break;
                        }
                        case PreferenceType::PT_STR: {
                            typeStr = "string";
                            valueStr = prefs.getString(key.c_str());
                            break;
                        }
                        case PreferenceType::PT_BLOB: {
                            typeStr = "blob";
                            size_t size = prefs.getBytesLength(key.c_str());
                            uint8_t* value = new uint8_t[size];
                            std::unique_ptr<uint8_t[]> valuePtr(value);
                            prefs.getBytes(key.c_str(), value, size);
                            for (size_t i = 0; i < size; i++) {
                                if (i > 0) {
                                    valueStr += " ";
                                }
                                valueStr += String(value[i], HEX);
                            }
                            valueStr = "???";
                            break;
                        }
                        case PreferenceType::PT_INVALID: {
                            typeStr = "invalid";
                            valueStr = "???";
                            break;
                        }
                    }
                    telnet.printf("Тип: %s, значення: %s", typeStr.c_str(), valueStr.c_str());
                    telnet.println();
                }
                prefs.end();
            } else if (subcommand == "rm") {
                if (args.size() != 2 && args.size() != 3) {
                    telnet.println("Помилка: невірна кількість параметрів");
                    return;
                }
                String ns = args[1];
                Preferences prefs;
                prefs.begin(ns.c_str(), false);
                if (args.size() == 2) {
                    if (prefs.clear()) {
                        telnet.println("Всі ключі в namespace " + ns + " видалено");
                    } else {
                        telnet.println("Помилка: не вдалося видалити namespace");
                    }
                } else {
                    String key = args[2];
                    if (prefs.remove(key.c_str())) {
                        telnet.println("Ключ " + key + " видалено");
                    } else {
                        telnet.println("Помилка: не вдалося видалити ключ");
                    }
                }
                prefs.end();
            } else {
                telnet.println("Помилка: невідома підкоманда");
            }
        },
    },
    {
        "exit",
        [](std::vector<String> args) { telnet.disconnectClient(); },
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
        telnet.println("Введіть 'help' для отримання списку команд");
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
            String argsString = input.substring(spaceIndex + 1);
            argsString.trim();

            std::vector<String> args;
            // Split args by space
            while (argsString.length()) {
                spaceIndex = argsString.indexOf(' ');
                if (spaceIndex == -1) {
                    String param = argsString;
                    param.trim();
                    if (!param.isEmpty()) {
                        args.push_back(param);
                    }
                    break;
                }
                String param = argsString.substring(0, spaceIndex);
                param.trim();
                if (!param.isEmpty()) {
                    args.push_back(param);
                }
                argsString = argsString.substring(spaceIndex + 1);
            }

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
            taskYIELD();
        } else {
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
    }
}

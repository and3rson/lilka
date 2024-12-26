#include <ff.h>
#include <FS.h>
#include <SD.h>
#include <SPIFFS.h>
#include "launcher.h"
#include "appmanager.h"

#include "servicemanager.h"
#include "services/network.h"

#include "wifi_config.h"
#include "demos/lines.h"
#include "demos/disk.h"
#include "demos/ball.h"
#include "demos/transform.h"
#include "demos/cube.h"
#include "demos/epilepsy.h"
#include "demos/letris.h"
#include "demos/keyboard.h"
#include "demos/user_spi.h"
#include "demos/scan_i2c.h"
#include "demos/petpet.h"
#include "demos/combo.h"
#include "gpiomanager.h"
#include "tamagotchi/tamagotchi.h"
#include "lua/luarunner.h"
#include "mjs/mjsrunner.h"
#include "nes/nesapp.h"
#include "ftp/ftp_server.h"
#include "weather/weather.h"
#include "modplayer/modplayer.h"
#include "liltracker/liltracker.h"
#include "ble_gamepad/app.h"
#include "pastebin/pastebinApp.h"
#include "settings/sound.h"

#include "icons/demos.h"
#include "icons/sdcard.h"
#include "icons/memory.h"
#include "icons/dev.h"
#include "icons/settings.h"
#include "icons/info.h"

#include "icons/app_group.h"
#include "icons/app.h"
#include "icons/normalfile.h"
#include "icons/folder.h"
#include "icons/nes.h"
#include "icons/bin.h"
#include "icons/lua.h"
#include "icons/js.h"
#include "icons/music.h"
#include "fmanager.h"

LauncherApp::LauncherApp() : App("Menu") {
    networkService = static_cast<NetworkService*>(ServiceManager::getInstance()->getService<NetworkService>("network"));
}

void LauncherApp::run() {
    for (lilka::Button button : {lilka::Button::UP, lilka::Button::DOWN, lilka::Button::LEFT, lilka::Button::RIGHT}) {
        lilka::controller.setAutoRepeat(button, 10, 300);
    }

    item_t root_item = ITEM::SUBMENU(
        "Головне меню",
        {
            ITEM::SUBMENU(
                "Додатки",
                {
                    ITEM::SUBMENU(
                        "Демо",
                        {
                            ITEM::MENU("Лінії", [this]() { this->runApp<DemoLines>(); }),
                            ITEM::MENU("Диск", [this]() { this->runApp<DiskApp>(); }),
                            ITEM::MENU("Перетворення", [this]() { this->runApp<TransformApp>(); }),
                            ITEM::MENU("М'ячик", [this]() { this->runApp<BallApp>(); }),
                            ITEM::MENU("Куб", [this]() { this->runApp<CubeApp>(); }),
                            ITEM::MENU("Епілепсія", [this]() { this->runApp<EpilepsyApp>(); }),
                            ITEM::MENU("PetPet", [this]() { this->runApp<PetPetApp>(); }),
                        }
                    ),
                    ITEM::SUBMENU(
                        "Тести",
                        {
                            ITEM::MENU("Клавіатура", [this]() { this->runApp<KeyboardApp>(); }),
                            ITEM::MENU("Тест SPI", [this]() { this->runApp<UserSPIApp>(); }),
                            ITEM::MENU("I2C-сканер", [this]() { this->runApp<ScanI2CApp>(); }),
                            ITEM::MENU("GPIO-менеджер", [this]() { this->runApp<GPIOManagerApp>(); }),
                            ITEM::MENU("Combo", [this]() { this->runApp<ComboApp>(); }),
                        }
                    ),
                    ITEM::MENU("ЛілТрекер", [this]() { this->runApp<LilTrackerApp>(); }),
                    ITEM::MENU("Летріс", [this]() { this->runApp<LetrisApp>(); }),
                    ITEM::MENU("Тамагочі", [this]() { this->runApp<TamagotchiApp>(); }),
                    ITEM::MENU("Погода", [this]() { this->runApp<WeatherApp>(); }),
                    ITEM::MENU("BLE Геймпад", [this]() { this->runApp<ble_gamepad_app::MainApp>(); }),
                    ITEM::MENU("Pastebin", [this]() { this->runApp<pastebinApp>(); }),
                },
                &demos_img,
                lilka::colors::Pink
            ),
            ITEM::MENU(
                "Браузер SD-карти",
                [this]() { this->runApp<FileManagerApp>(&SD, "/"); },
                &sdcard_img,
                lilka::colors::Arylide_yellow
            ),
            ITEM::MENU(
                "Браузер SPIFFS",
                [this]() { this->runApp<FileManagerApp>(&SPIFFS, "/"); },
                &memory_img,
                lilka::colors::Dark_sea_green
            ),
            ITEM::SUBMENU(
                "Розробка",
                {
                    ITEM::MENU("Live Lua", [this]() { this->runApp<LuaLiveRunnerApp>(); }),
                    ITEM::MENU("Lua REPL", [this]() { this->runApp<LuaReplApp>(); }),
                    ITEM::MENU("FTP сервер", [this]() { this->runApp<FTPServerApp>(); }),
                },
                &dev_img,
                lilka::colors::Jasmine
            ),
            ITEM::SUBMENU(
                "Налаштування",
                {
                    ITEM::MENU(
                        "WiFi-адаптер",
                        [this]() { this->wifiToggle(); },
                        nullptr,
                        0,
                        [this](void* item) {
                            lilka::MenuItem* menuItem = static_cast<lilka::MenuItem*>(item);
                            menuItem->postfix = networkService->getEnabled() ? "ON" : "OFF";
                        }
                    ),
                    ITEM::MENU("Мережі WiFi", [this]() { this->wifiManager(); }),
                    ITEM::MENU("Звук", [this]() { this->runApp<SoundConfigApp>(); }),
                    ITEM::MENU("Про систему", [this]() { this->about(); }),
                    ITEM::MENU("Інфо про пристрій", [this]() { this->info(); }),
                    ITEM::MENU("Таблиця розділів", [this]() { this->partitions(); }),
                    ITEM::MENU("Форматування SD-карти", [this]() { this->formatSD(); }),
                    ITEM::MENU("Light sleep", []() { 
                        lilka::board.enablePowerSavingMode();
                        esp_light_sleep_start();
                    }),
                    ITEM::MENU("Deep sleep", []() { 
                        lilka::board.enablePowerSavingMode();
                        esp_deep_sleep_start();
                     }),
                    ITEM::MENU("Перезавантаження", []() {
                            esp_restart();
                     }),
                },
                &settings_img,
                lilka::colors::Orchid
            ),
        }
    );
    showMenu(root_item.name, root_item.submenu, false);
}
void LauncherApp::showMenu(const char* title, ITEM_LIST& list, bool back) {
    int itemCount = list.size();
    lilka::Menu menu(title);
    for (int i = 0; i < list.size(); i++) {
        menu_icon_t* icon = list[i].icon;
        if (icon == NULL) {
            icon = list[i].submenu.empty() ? &app_img : &app_group_img;
        }
        menu.addItem(list[i].name, icon, list[i].color);
    }
    if (back) {
        menu.addActivationButton(lilka::Button::B);
        menu.addItem("<< Назад");
    }
    while (1) {
        while (!menu.isFinished()) {
            for (int i = 0; i < list.size(); i++) {
                if (list[i].update != nullptr) {
                    lilka::MenuItem menuItem;
                    menu.getItem(i, &menuItem);
                    list[i].update(&menuItem);
                    menu.setItem(i, menuItem.title, menuItem.icon, menuItem.color, menuItem.postfix);
                }
            }
            menu.update();
            menu.draw(canvas);
            queueDraw();
        }
        if (menu.getButton() == lilka::Button::B) {
            break;
        }
        int16_t index = menu.getCursor();
        if (back && index == itemCount) {
            break;
        }

        item_t item = list[index];
        if (item.callback != nullptr) {
            item.callback();
        }
        if (!item.submenu.empty()) {
            showMenu(item.name, item.submenu);
        }
    }
}
void LauncherApp::alert(String title, String message) {
    lilka::Alert alert(title, message);
    alert.draw(canvas);
    queueDraw();
    while (!alert.isFinished()) {
        alert.update();
        taskYIELD();
    }
}
template <typename T, typename... Args>
void LauncherApp::runApp(Args&&... args) {
    AppManager::getInstance()->runApp(new T(std::forward<Args>(args)...));
}

void LauncherApp::wifiToggle() {
    networkService->setEnabled(!networkService->getEnabled());
}
void LauncherApp::wifiManager() {
    if (!networkService->getEnabled()) {
        alert("Помилка", "Спочатку увімкніть WiFi-адаптер");
        return;
    }
    AppManager::getInstance()->runApp(new WiFiConfigApp());
}
void LauncherApp::about() {
    alert("Keira OS", "by Андерсон & friends");
}
void LauncherApp::info() {
    char buf[256];
    sprintf(
        buf,
        "Модель: %s\n"
        "Ревізія: %d\n"
        "Версія ESP-IDF: %s\n"
        "Частота: %d МГц\n"
        "Кількість ядер: %d\n"
        "IP: %s",
        ESP.getChipModel(),
        ESP.getChipRevision(),
        esp_get_idf_version(),
        ESP.getCpuFreqMHz(),
        ESP.getChipCores(),
        networkService->getIpAddr().c_str()
    );
    alert("Інфо про пристрій", buf);
}
void LauncherApp::partitions() {
    String names[16];
    int partitionCount = lilka::sys.get_partition_labels(names);

    ITEM_LIST partitionsMenu;
    for (int i = 0; i < partitionCount; i++) {
        String partition = names[i];
        partitionsMenu.push_back(ITEM::MENU(names[i].c_str(), [this, partition]() {
            alert(
                partition,
                String("Адреса: 0x") + String(lilka::sys.get_partition_address(partition.c_str()), HEX) + "\n" +
                    "Розмір: 0x" + String(lilka::sys.get_partition_size(partition.c_str()), HEX)
            );
        }));
    }
    showMenu("Таблиця розділів", partitionsMenu);
}
void LauncherApp::formatSD() {
    lilka::Alert confirm(
        "Форматування",
        "УВАГА: Це очистить ВСІ дані з SD-карти!\n"
        "\nПродовжити?\n\nSTART - продовжити\nA - скасувати"
    );
    confirm.addActivationButton(lilka::Button::START);
    confirm.draw(canvas);
    queueDraw();
    while (!confirm.isFinished()) {
        confirm.update();
        taskYIELD();
    }
    if (confirm.getButton() != lilka::Button::START) {
        return;
    }

    lilka::ProgressDialog dialog("Форматування", "Будь ласка, зачекайте...");
    dialog.draw(canvas);
    queueDraw();
    if (!lilka::fileutils.createSDPartTable()) {
        alert(
            "Помилка",
            "Не вдалося створити нову таблицю розділів. "
            "Можливо відсутня карта.\n\n"
            "Систему буде перезавантажено."
        );
        esp_restart();
    }
    if (!lilka::fileutils.formatSD()) {
        this->alert(
            "Помилка",
            "Не вдалося форматувати SD-карту.\n\n"
            "Систему буде перезавантажено."
        );
        esp_restart();
    }
    this->alert(
        "Форматування",
        "Форматування SD-карти завершено!\n\n"
        "Систему буде перезавантажено."
    );
    esp_restart();
}

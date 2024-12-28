#pragma once

#include "services/network.h"
#include "icons/app.h"
#include "icons/app_group.h"
#include "app.h"

typedef struct item_t {
    const char* name;
    const menu_icon_t* icon;
    uint16_t color;
    std::vector<item_t> submenu;
    std::function<void()> callback;
    std::function<void(void*)> update;

public:
    static item_t SUBMENU(
        const char* name, const std::vector<item_t>& submenu, const menu_icon_t* icon = NULL, uint16_t color = 0
    ) {
        return item_t{
            name,
            icon ? icon : &app_group_img,
            color,
            submenu,
        };
    }

    static item_t MENU(
        const char* name, std::function<void()> callback, const menu_icon_t* icon = NULL, uint16_t color = 0,
        std::function<void(void*)> update = nullptr
    ) {
        return item_t{
            name,
            icon,
            color,
            {},
            callback,
            update,
        };
    }

    static item_t APP(
        const char* name, std::function<void()> callback, const menu_icon_t* icon = NULL, uint16_t color = 0,
        std::function<void(void*)> update = nullptr
    ) {
        return item_t::MENU(name, callback, icon ? icon : &app_img, color, update);
    }

} ITEM;

#define ITEM_LIST std::vector<item_t>

class LauncherApp : public App {
public:
    LauncherApp();

private:
    NetworkService* networkService;

private:
    void run() override;

    void showMenu(const char* title, ITEM_LIST& menu, bool back = true);
    void alert(String title, String message);
    template <typename T, typename... Args>
    void runApp(Args&&... args);
    void setWiFiTxPower();
    void wifiToggle();
    void wifiManager();
    void about();
    void info();
    void partitions();
    void formatSD();
};

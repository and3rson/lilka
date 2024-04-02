#include <map>
#include <HTTPClient.h>
#include <lilka/config.h>

#include "Preferences.h"
#include "servicemanager.h"
#include "services/clock.h"
#include "utils/json.h"
#include "weather.h"
#include "icons/weather_icons.h"

typedef struct {
    const uint32_t len;
    const uint8_t* data;
} icon_data_t;

typedef struct {
    icon_data_t day;
    icon_data_t night;
} icon_t;

const icon_t clear = {
    {clear_d_length, clear_d},
    {clear_n_length, clear_n},
};
const icon_t few_clouds = {
    {few_clouds_d_length, few_clouds_d},
    {few_clouds_n_length, few_clouds_n},
};
const icon_t scattered_clouds = {
    {scattered_clouds_d_length, scattered_clouds_d},
    {scattered_clouds_n_length, scattered_clouds_n},
};
const icon_t broken_clouds = {
    {broken_clouds_d_length, broken_clouds_d},
    {broken_clouds_n_length, broken_clouds_n},
};
const icon_t shower_rain = {
    {shower_rain_d_length, shower_rain_d},
    {shower_rain_n_length, shower_rain_n},
};
const icon_t rain = {
    {rain_d_length, rain_d},
    {rain_n_length, rain_n},
};
const icon_t thunderstorm = {
    {thunderstorm_d_length, thunderstorm_d},
    {thunderstorm_n_length, thunderstorm_n},
};
const icon_t snow = {
    {snow_d_length, snow_d},
    {snow_n_length, snow_n},
};
const icon_t mist = {
    {mist_d_length, mist_d},
    {mist_n_length, mist_n},
};
#define STR_HELPER(x) #x
#define STR(x)        STR_HELPER(x)
const char* userAgent = "Lilka/" STR(LILKA_VERSION);

// WMO weather code to icon mapping (https://open-meteo.com/en/docs/#hourly=temperature_2m,weather_code)
static std::map<uint8_t, const icon_t*> icons = {
    {0, &clear},
    //
    {1, &few_clouds},
    {2, &scattered_clouds},
    {3, &broken_clouds},
    //
    {45, &mist},
    {48, &mist},
    //
    {51, &rain},
    {53, &shower_rain},
    {55, &shower_rain},
    //
    {56, &rain},
    {57, &shower_rain},
    //
    {61, &rain},
    {63, &shower_rain},
    {65, &shower_rain},
    //
    {66, &rain},
    {67, &shower_rain},
    //
    {71, &snow},
    {73, &snow},
    {75, &snow},
    //
    {77, &snow},
    //
    {80, &rain},
    {81, &shower_rain},
    {82, &shower_rain},
    //
    {85, &snow},
    {86, &snow},
    //
    {95, &thunderstorm},
    {96, &thunderstorm},
    {99, &thunderstorm},
};

static std::map<uint8_t, const char*> titles = {
    {0, "Чисте небо"},
    //
    {1, "Переважно ясно"},
    {2, "Частково хмарно"},
    {3, "Хмарно"},
    //
    {45, "Туман"},
    {48, "Паморозь"},
    //
    {51, "Легка мжичка"},
    {53, "Мжичка"},
    {55, "Сильна мжичка"},
    //
    {56, "Легка мжичка"},
    {57, "Сильна мжичка"},
    //
    {61, "Легкий дощ"},
    {63, "Дощ"},
    {65, "Сильний дощ"},
    //
    {66, "Дощ"},
    {67, "Сильний дощ"},
    //
    {71, "Легкий сніг"},
    {73, "Сніг"},
    {75, "Сильний сніг"},
    //
    {77, "Сніжинки"},
    //
    {80, "Легка злива"},
    {81, "Злива"},
    {82, "Сильна злива"},
    //
    {85, "Дощ зі снігом"},
    {86, "Сильний дощ зі снігом"},
    //
    {95, "Легка гроза"},
    {96, "Гроза"},
    {99, "Сильна гроза"},
};

const char* urlTemplate = "https://api.open-meteo.com/v1/forecast"
                          "?latitude=%.4f"
                          "&longitude=%.4f"
                          "&current=temperature_2m,wind_speed_10m,weather_code";

WeatherApp::WeatherApp() : App("Weather") {
}

void WeatherApp::run() {
    JsonDocument data(&spiRamAllocator);
    WiFiClientSecure client;
    HTTPClient http;

    while (1) {
        Preferences prefs;
        // Lviv geo coordinates
        // float lat = 49.8397;
        // float lon = 24.0297;
        settings_t settings = getSettings();

        canvas->fillScreen(lilka::colors::Black);
        canvas->setFont(FONT_9x15);
        canvas->setTextBound(32, 32, canvas->width() - 64, canvas->height() - 64);
        canvas->setTextColor(lilka::colors::White);
        canvas->setCursor(32, 32 + 15);
        uint32_t wait = 60000;
        if (settings.isConfigured) {
            canvas->println("Отримання даних...");
            queueDraw();
            // Get weather data
            char url[strlen(urlTemplate) + 32];
            snprintf(url, sizeof(url), urlTemplate, settings.lat, settings.lon);
            // Disable SSL verification
            client.setInsecure();
            // Perform GET request
            http.setUserAgent(userAgent);
            http.begin(client, url);
            int httpCode = http.GET();

            const icon_data_t* iconData = 0;
            const char* title = 0;
            float temp, wind;
            if (httpCode == HTTP_CODE_OK) {
                // Parse JSON
                DeserializationError error = deserializeJson(data, http.getString());
                if (error) {
                    lilka::serial_err("deserializeJson() failed: %s", error.c_str());
                    canvas->fillScreen(lilka::colors::Black);
                    canvas->setCursor(32, 32 + 15);
                    canvas->println("Помилка десеріалізації");
                    wait = 5000;
                    queueDraw();
                } else {
                    // Extract data
                    temp = data["current"]["temperature_2m"];
                    wind = data["current"]["wind_speed_10m"];
                    uint8_t code = data["current"]["weather_code"];
                    const icon_t* icon = icons[code];
                    ClockService* clockService = ServiceManager::getInstance()->getService<ClockService>("clock");
                    struct tm time = clockService->getTime();
                    if (time.tm_hour >= 6 && time.tm_hour < 18) {
                        iconData = &icon->day;
                    } else {
                        iconData = &icon->night;
                    }
                    title = titles[code];
                    lilka::serial_log("Temperature: %.1f, Wind: %.1f, Code: %d", temp, wind, code);
                }
            } else {
                lilka::serial_log("HTTP GET failed, error: %s", http.errorToString(httpCode).c_str());
                canvas->fillScreen(lilka::colors::Black);
                canvas->setCursor(32, 32 + 15);
                canvas->println("Помилка отримання даних:");
                canvas->println("Код відповіді: " + String(httpCode));
                wait = 1000;
                queueDraw();
            }
            if (iconData && title) {
                lilka::Image* img =
                    lilka::Image::newFromRLE(iconData->data, iconData->len, 200, 200, lilka::colors::Black, 100, 100);
                std::unique_ptr<lilka::Image> imgPtr(img);
                canvas->fillScreen(lilka::colors::Black);
                canvas->drawImage(img, canvas->width() / 4, canvas->height() / 2);
                uint16_t titleW;
                {
                    uint16_t h;
                    int16_t x, y;
                    canvas->getTextBounds(title, 0, 0, &x, &y, &titleW, &h);
                }
                canvas->setTextBound(0, 0, canvas->width(), canvas->height());
                canvas->setCursor(canvas->width() / 2 - titleW / 2, canvas->height() - 16);
                canvas->setTextColor(lilka::colors::White);
                canvas->print(title);
                canvas->setFont(u8g2_font_10x20_te); // FONT_10x20 does not have degree symbol
                canvas->setCursor(canvas->width() / 2, canvas->height() / 2 + 20 / 2 - 20);
                canvas->print(String(temp, 1) + " °C");
                canvas->setFont(FONT_10x20);
                canvas->setCursor(canvas->width() / 2, canvas->height() / 2 + 20 / 2 + 10);
                canvas->print(String(wind, 1) + " м/с");
                queueDraw();
            }
        } else {
            canvas->println("Локацію не налаштовано");
            canvas->println("[SELECT] - налаштування");
            canvas->println("[A] - вихід");
            queueDraw();
        }
        uint64_t waitUntil = millis() + wait;
        while (millis() < waitUntil) {
            lilka::State state = lilka::controller.getState();
            if (state.a.justPressed) {
                return;
            } else if (state.select.justPressed) {
                runSettings();
                break;
            }
            taskYIELD();
        }
    }
}

// Show settings menu
// Returns true if settings were saved, false if exited without saving
bool WeatherApp::runSettings() {
    settings_t settings = getSettings();
    bool saveSettings = false;
    bool exitSettings = false;
    while (!saveSettings && !exitSettings) {
        lilka::Menu settingsMenu("Налаштування");
        settingsMenu.addActivationButton(lilka::Button::B);
        settingsMenu.addItem("Широта", 0, 0, String(settings.lat));
        settingsMenu.addItem("Довгота", 0, 0, String(settings.lon));
        settingsMenu.addItem("Зберегти", 0, 0, "");
        settingsMenu.addItem("Скасувати", 0, 0, "");
        while (!settingsMenu.isFinished()) {
            settingsMenu.update();
            settingsMenu.draw(canvas);
            queueDraw();
        }
        if (settingsMenu.getButton() == lilka::Button::B) {
            exitSettings = true;
        } else {
            if (settingsMenu.getCursor() == 0 || settingsMenu.getCursor() == 1) {
                lilka::InputDialog inputDialog(
                    String("Введіть ") + (settingsMenu.getCursor() == 0 ? "широту" : "довготу")
                );
                inputDialog.setValue(String(settingsMenu.getCursor() == 0 ? settings.lat : settings.lon));
                while (!inputDialog.isFinished()) {
                    inputDialog.update();
                    inputDialog.draw(canvas);
                    queueDraw();
                }
                if (settingsMenu.getCursor() == 0) {
                    settings.lat = inputDialog.getValue().toFloat();
                } else {
                    settings.lon = inputDialog.getValue().toFloat();
                }
            } else if (settingsMenu.getCursor() == 2) {
                saveSettings = true;
            } else if (settingsMenu.getCursor() == 3) {
                exitSettings = true;
            }
        }
    }
    if (saveSettings) {
        Preferences prefs;
        prefs.begin("weather", false);
        prefs.putFloat("lat", settings.lat);
        prefs.putFloat("lon", settings.lon);
        prefs.end();
        return true;
    }
    return false;
}

settings_t WeatherApp::getSettings() {
    Preferences prefs;
    settings_t settings = {false, 0, 0};
    prefs.begin("weather", false);
    if (prefs.isKey("lat") && prefs.isKey("lon")) {
        settings.lat = prefs.getFloat("lat", settings.lat);
        settings.lon = prefs.getFloat("lon", settings.lon);
        settings.isConfigured = true;
    }
    prefs.end();
    return settings;
}

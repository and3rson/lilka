#include <map>
#include <HTTPClient.h>

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

// const uint8_t icons[] = {1, 2, 3, 4, 9, 10, 11, 13, 50};
// Lviv geo coordinates
const float lat = 49.8397;
const float lon = 24.0297;

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
        // Get weather data
        char url[strlen(urlTemplate) + 32];
        snprintf(url, sizeof(url), urlTemplate, lat, lon);
        // Disable SSL verification
        client.setInsecure();
        // Perform GET request
        http.begin(client, url);
        int httpCode = http.GET();

        if (httpCode == HTTP_CODE_OK) {
            // Parse JSON
            DeserializationError error = deserializeJson(data, http.getStream());
            if (error) {
                Serial.printf("deserializeJson() failed: %s\n", error.c_str());
            } else {
                // Extract data
                float temp = data["current"]["temperature_2m"];
                float wind = data["current"]["wind_speed_10m"];
                uint8_t code = data["current"]["weather_code"];
                const icon_t* icon = icons[code];
                Serial.printf("Temperature: %.1f, Wind: %.1f, Code: %d\r\n", temp, wind, code);
            }
        } else {
            Serial.printf("HTTP GET failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}

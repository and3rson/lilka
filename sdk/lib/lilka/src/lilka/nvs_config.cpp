#include "nvs_config.h"
#include <Preferences.h>

namespace lilka {

Preferences preferences;

void saveConfiguration(const Configuration& config) {
    preferences.begin("config", false); // "config" is the namespace, false indicates read-only
    preferences.putString("config_data", String(reinterpret_cast<const char*>(&config), sizeof(Configuration)));
    preferences.end();
}

void loadConfiguration(Configuration& config) {
    preferences.begin("config", true); // "config" is the namespace, true indicates read-only
    String data = preferences.getString("config_data", "");
    preferences.end();

    if (data.length() == sizeof(Configuration)) {
        memcpy(&config, data.c_str(), sizeof(Configuration));
    }
    setDefaultConfiguration();
}

void setDefaultConfiguration() {
    Configuration defaultConfig;
    saveConfiguration(defaultConfig);
}

} // namespace lilka
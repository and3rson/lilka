#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <Arduino.h>

namespace lilka{
    
    // Define your configuration struct
    struct Configuration {
        String language = "en";
    };

    void saveConfiguration(const Configuration& config);
    void loadConfiguration(Configuration& config);
    void setDefaultConfiguration();
}
#endif // CONFIGURATION_H
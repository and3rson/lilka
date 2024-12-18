#include "Localization.h"

namespace lilka {

// Vector of full language names
const std::vector<std::string> Language_FullName = {
    "English",
    "Ukrainian"
    // Add more full language names as needed
};

// Vector of native full language names
const std::vector<std::string> Language_FullNameNative = {
    "English",
    "Українська"
    // Add more native full language names as needed
};

// Vector of short language names
const std::vector<std::string> Language_ShortName = {
    "en",
    "ua"
    // Add more short language names as needed
};

void Localization::addString(Language_Code language, const std::string& key, const std::string& value) {
    languages[language][key] = value;
}

std::string Localization::getString(Language_Code language, const std::string& key) const {
    auto languageIt = languages.find(language);
    if (languageIt != languages.end()) {
        const auto& languageStrings = languageIt->second;
        auto keyIt = languageStrings.find(key);
        if (keyIt != languageStrings.end()) {
            return keyIt->second; // Return localized string for the key
        } else {
            // If the key is not found in the requested language, return English variant
            const auto& englishStrings = languages.at(Language_Code::EN);
            auto englishKeyIt = englishStrings.find(key);
            if (englishKeyIt != englishStrings.end()) {
                return englishKeyIt->second; // Return English version for the key
            } else {
                return "English version not found for key: " + key;
            }
        }
    } else {
        // If the requested language is not supported, return English variant
        const auto& englishStrings = languages.at(Language_Code::EN);
        auto englishKeyIt = englishStrings.find(key);
        if (englishKeyIt != englishStrings.end()) {
            return englishKeyIt->second; // Return English version for the key
        } else {
            return "Language not supported and English version not found for key: " + key;
        }
    }
}
} // namespace lilka

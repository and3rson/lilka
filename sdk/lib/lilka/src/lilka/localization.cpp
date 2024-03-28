#include "Localization.h"

namespace lilka {

// Vector of full language names
const std::vector<std::string> Language_FullName = {
    "English",
    "Ukraine"
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
            return keyIt->second;
        }
        return "Localization not found for key: " + Language_FullName[static_cast<int>(language)] + " in language: " + key;
    }
    return "Language not supported: " + Language_FullName[static_cast<int>(language)];
}

} // namespace lilka

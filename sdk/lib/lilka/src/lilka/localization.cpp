
#include "Localization.h"

namespace lilka {

// Array of full language names
const char* Language_FullName[static_cast<int>(Language_Code::NumLanguages)] = {
    "English",
    "Ukraine"
    // Add more full language names as needed
};

// Array of native full language names
const char* Language_FullNameNative[static_cast<int>(Language_Code::NumLanguages)] = {
    "English",
    "Українська"
    // Add more native full language names as needed
};

// Array of short language names
const char* Language_ShortName[static_cast<int>(Language_Code::NumLanguages)] = {
    "en",
    "ua"
    // Add more short language names as needed
};

void Localization::addString(const std::string& language, const std::string& key, const std::string& value) {
    languages[language][key] = value;
}

std::string Localization::getString(const std::string& language, const std::string& key) const {
    auto languageIt = languages.find(language);
    if (languageIt != languages.end()) {
        const auto& languageStrings = languageIt->second;
        auto keyIt = languageStrings.find(key);
        if (keyIt != languageStrings.end()) {
            return keyIt->second;
        }
        return "Localization not found for key: " + key + " in language: " + language;
    }
    return "Language not supported: " + language;
}

} // namespace lilka
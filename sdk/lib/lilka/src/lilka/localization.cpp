
#include "Localization.h"

namespace lilka {

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

    

}
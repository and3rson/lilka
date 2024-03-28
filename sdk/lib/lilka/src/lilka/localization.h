#ifndef LILKA_LOCALIZATION_H
#define LILKA_LOCALIZATION_H

#include <map>
#include <string>

namespace lilka {

// Enum to represent supported languages
enum class Language_Code {
    EN = 0,
    UA,
    // Add more languages as needed
    NumLanguages // Special value to represent the number of languages
};

// Define your configuration struct
struct Configuration {
    Language_Code language;
};

// Array of full language names
extern const char* Language_FullName[static_cast<int>(Language_Code::NumLanguages)];

// Array of native full language names
extern const char* Language_FullNameNative[static_cast<int>(Language_Code::NumLanguages)];

// Array of short language names
extern const char* Language_ShortName[static_cast<int>(Language_Code::NumLanguages)];

class Localization {
public:
    void addString(const std::string& language, const std::string& key, const std::string& value);
    std::string getString(const std::string& language, const std::string& key) const;

private:
    std::map<std::string, std::map<std::string, std::string>> languages;
};
} // namespace lilka

#endif // LILKA_LOCALIZATION_H

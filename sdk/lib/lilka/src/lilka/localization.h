#ifndef LILKA_LOCALIZATION_H
#define LILKA_LOCALIZATION_H

#include <map>
#include <string>
#include <vector> // Include vector header

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

// Vector of full language names
extern const std::vector<std::string> Language_FullName;

// Vector of native full language names
extern const std::vector<std::string> Language_FullNameNative;

// Vector of short language names
extern const std::vector<std::string> Language_ShortName;

class Localization {
public:
    void addString(Language_Code language, const std::string& key, const std::string& value);
    std::string getString(Language_Code language, const std::string& key) const;

private:
    std::map<Language_Code, std::map<std::string, std::string>> languages;
};

} // namespace lilka

#endif // LILKA_LOCALIZATION_H
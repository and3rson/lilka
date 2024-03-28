#ifndef LILKA_LOCALIZATION_H
#define LILKA_LOCALIZATION_H

#include <map>
#include <string>

    namespace lilka {

    class Localization {
    public:
        void addString(const std::string& language, const std::string& key, const std::string& value);
        std::string getString(const std::string& language, const std::string& key) const;

    private:
        std::map<std::string, std::map<std::string, std::string>> languages;
    };

}

#endif // LOCALIZATION_H


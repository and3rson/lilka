#ifndef LILKA_FILESYSTEM_H
#define LILKA_FILESYSTEM_H

#include <SPIFFS.h>

namespace lilka {

class Filesystem {
public:
    Filesystem();
    void begin();
    bool available();
    int readdir(String filenames[]);
    int readdir(String filenames[], String extension);
    String abspath(String filename);

private:
    FS* _filesystem;
    bool _available;
};

extern Filesystem filesystem;

} // namespace lilka

#endif // LILKA_FILESYSTEM_H

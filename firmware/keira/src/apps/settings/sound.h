#define VOLUME_INCREMENT 5
#define REPEAT_INTERVAL  100

#include <lilka.h>
#include "app.h"

class SoundConfigApp : public App {
public:
    SoundConfigApp();

private:
    void run() override;
};
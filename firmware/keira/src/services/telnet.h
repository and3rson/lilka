#include <ESPTelnet.h>

#include "service.h"

class TelnetService : public Service {
public:
    TelnetService();
    ~TelnetService();

    static ESPTelnet telnet;

private:
    void run() override;
};

#include <ESPTelnet.h>

#include "service.h"

class TelnetService : public Service {
public:
    TelnetService();
    ~TelnetService();

private:
    void run() override;
};

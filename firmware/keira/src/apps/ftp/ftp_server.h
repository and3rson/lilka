#include "app.h"
#include "ESP32FtpServer.h"

class FTPServerApp : public App {
public:
    FTPServerApp();

private:
    void run() override;
    FtpServer ftpSrv;
};

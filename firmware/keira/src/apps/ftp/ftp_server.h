#include "app.h"
#include "FtpServer.h"

class FTPServerApp : public App {
public:
    FTPServerApp();

private:
    void run() override;
    FtpServer ftpSrv;
};

#include <FtpServer.h>
#include "services/network.h"
#include "service.h"

#define FTP_SETTINGS        "ftp"
#define FTP_USER            "lilka"
#define FTP_PASSWORD_LENGTH 6

class FTPService : public Service {
private:
    bool enabled;
    String user = FTP_USER;
    String password;
    NetworkService* networkService;
    FtpServer* ftpServer = nullptr;

public:
    FTPService();
    ~FTPService();

    bool getEnabled();
    void setEnabled(bool enabled);
    String getUser();
    String getPassword();
    String getEndpoint();
    void createPassword();

private:
    void run() override;
};

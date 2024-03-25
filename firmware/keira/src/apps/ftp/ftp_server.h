#include "app.h"

class FTPServerApp : public App {
public:
    FTPServerApp();

private:
    void run() override;
    String createPassword();
    String getPassword();
};

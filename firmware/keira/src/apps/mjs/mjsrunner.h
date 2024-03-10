#pragma once

#include <lilka.h>
#include "app.h"

class MJSApp : public App {
public:
    explicit MJSApp(String path);
    void run() override;
    static void* ffi_resolver(void* handle, const char* name);

private:
    String path;
};

#include "mjsrunner.h"
#include "lilka.h"
#include "mjs.h"

MJSApp::MJSApp(String path) : App("mJS", 0, 0, lilka::display.width(), lilka::display.height()), path(path) {
}

void MJSApp::run() {
    struct mjs* mjs = mjs_create();

    mjs_val_t res;

    mjs_set_ffi_resolver(mjs, ffi_resolver);
    mjs_err_t err = mjs_exec_file(mjs, path.c_str(), &res);
    if (err != MJS_OK) {
        const char* error = mjs_strerror(mjs, err);
        lilka::Alert alert("mJS", String("Помилка: ") + err);
        alert.draw(canvas);
        queueDraw();
        while (!alert.isDone()) {
            alert.update();
        }
    }
}

void* MJSApp::ffi_resolver(void* handle, const char* name) {
    // TODO: Expose more functions
    if (strcmp(name, "print") == 0) {
        return (void*)printf;
    }
    return NULL;
}

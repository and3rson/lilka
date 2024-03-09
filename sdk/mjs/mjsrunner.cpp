#include "mjsrunner.h"
#include "lilka.h"

namespace lilka {

void *mjslilka_ffi_resolver(void *handle, const char *name) {
    if (strcmp(name, "print") == 0) {
        return (void *)printf;
    }
    return NULL;
}

int mjs_run(Canvas *canvas, String path) {
#ifndef LILKA_NO_MJS
    struct mjs *mjs = mjs_create();

    mjs_val_t res;

    mjs_set_ffi_resolver(mjs, mjslilka_ffi_resolver);
    mjs_err_t err = mjs_exec_file(mjs, path.c_str(), &res);
    if (err != MJS_OK) {
        printf("Error: %s\n", mjs_strerror(mjs, err));
        // TODO: FreeRTOS experiment
        ui_alert(canvas, "Помилка", mjs_strerror(mjs, err));
        return 1;
    }
    return 0;
#else
    lilka::ui_alert(canvas, "Помилка", "MJS не підтримується");
#endif
}

} // namespace lilka

#include <stdio.h>
#include <Preferences.h>

#include "multiboot.h"
#include "fileutils.h"
#include "serial.h"

extern "C" bool verifyRollbackLater() {
    return true;
}

namespace lilka {

extern FileUtils fileutils;

#define MULTIBOOT_PATH_KEY "multiboot_path"

MultiBoot::MultiBoot() :
    ota_handle(0), current_partition(NULL), ota_partition(NULL), path(""), bytesTotal(0), bytesWritten(0), file(NULL) {
}

void MultiBoot::begin() {
    current_partition = esp_ota_get_running_partition();
    serial_log(
        "Current partition: %s, type: %d, subtype: %d, size: %d",
        current_partition->label,
        current_partition->type,
        current_partition->subtype,
        current_partition->size
    );
    ota_partition = esp_ota_get_next_update_partition(current_partition); // get ota1 (we're in ota0 now)
    serial_log(
        "OTA partition: %s, type: %d, subtype: %d, size: %d",
        ota_partition->label,
        ota_partition->type,
        ota_partition->subtype,
        ota_partition->size
    );

    // А тут я згаяв трохи часу. Нижче наведено спроби увімкнути автоматичний відкат прошивки з кінцевим рішенням.

    // Спроба 1:

    /*
    // Check if rollback is possible
    esp_ota_img_states_t ota_state;
    esp_err_t err = esp_ota_get_state_partition(esp_ota_get_running_partition(), &ota_state);
    if (err != ESP_OK) {
        serial_err("Failed to get state partition: %d", err);
        return;
    }

    serial_log("OTA state: %d", ota_state);

    if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
        serial_log("Rollback is possible");
        // Mark ota0 as active partition so that we return to main application after next restart
        esp_ota_set_boot_partition(esp_ota_get_next_update_partition(NULL));
    } else {
        serial_log("Rollback is not possible");
    }
    */

    // Спроба 2:
    // Auto-rollback does not work properly - my dev board's bootloader might be messing it up,
    // since every OTA update is marked as successful, even if we don't mark it as valid
    // TODO: Maybe it will work with actual Lilka v2?
    // So here's a workaround: I'm going to set ota0 as active partition anyway, so that we return to main application after next restart.

    // Mark ota0 as active partition so that we return to main application after next restart
    // esp_ota_img_states_t ota_state;
    // esp_err_t err = esp_ota_get_state_partition(esp_ota_get_running_partition(), &ota_state);
    // serial_log("OTA state: %d", ota_state);
    // if (err != ESP_OK) {
    //     serial_err("Failed to get state partition: %d", err);
    //     return;
    // }
    // if (current_partition->subtype == ESP_PARTITION_SUBTYPE_APP_OTA_1) {
    //     esp_ota_set_boot_partition(esp_ota_get_next_update_partition(NULL));
    // }

    // Спроба 3:
    // Ок, розібрався. (Так, я по звичці писав попередні коментарі англійською мовою, але намагаюсь використовувати українську.)
    // Потрібно було перевизначити verifyRollbackLater(), щоб він завжди повертав false, тоді автоматичний відкат працюватиме.
    // https://github.com/espressif/arduino-esp32/issues/7423

    esp_ota_img_states_t ota_state;
    esp_err_t err = esp_ota_get_state_partition(esp_ota_get_running_partition(), &ota_state);
    serial_log("OTA state: %d", ota_state);
}

int MultiBoot::start(String path) {
    // Завантаження прошивки з microSD-картки.
    this->path = path;

    if (!fileutils.isSDAvailable()) {
        serial_err("SD card not available");
        return -1;
    }

    // String abspath = sdcard.abspath(path);

    // TODO: Use sdcard instead of SD
    file = fopen(path.c_str(), "r");
    if (file == NULL) {
        serial_err("Failed to open file: %s", path.c_str());
        return -2;
    }

    bytesWritten = 0;
    // Get file size
    fseek(file, 0, SEEK_END);
    bytesTotal = ftell(file);
    fseek(file, 0, SEEK_SET);

    current_partition = esp_ota_get_running_partition();
    if (current_partition == NULL) {
        serial_err("Failed to get current partition");
        return -3;
    }
    serial_log(
        "Current partition: %s, type: %d, subtype: %d, size: %d",
        current_partition->label,
        current_partition->type,
        current_partition->subtype,
        current_partition->size
    );
    ota_partition = esp_ota_get_next_update_partition(current_partition); // get ota1 (we're in ota0 now)
    if (ota_partition == NULL) {
        serial_err("Failed to get next OTA partition");
        return -4;
    }
    serial_log(
        "OTA partition: %s, type: %d, subtype: %d, size: %d",
        ota_partition->label,
        ota_partition->type,
        ota_partition->subtype,
        ota_partition->size
    );

    esp_err_t err = esp_ota_begin(ota_partition, bytesTotal, &ota_handle);
    if (err != ESP_OK) {
        serial_err("Failed to begin OTA: %d", err);
        return -5;
    }

    Preferences prefs;
    prefs.begin("lilka", false);
    String arg = path;
    // Remove "/sd" prefix
    // TODO: Maybe we should use absolute path (including "/sd")?
    // TODO: Store arg in RAM?
    arg = lilka::fileutils.getLocalPathInfo(arg).path;

    prefs.putString(MULTIBOOT_PATH_KEY, arg);
    prefs.end();

    return 0;
}

int MultiBoot::process() {
    char buf[4096];

    // Записуємо 16 КБ.

    for (int i = 0; i < 4; i++) {
        // Read 1024 bytes
        int len = fread(buf, 1, sizeof(buf), file);
        if (len == 0) {
            fclose(file);
            return 0;
        }

        esp_err_t err = esp_ota_write(ota_handle, buf, len);
        if (err != ESP_OK) {
            serial_err("Failed to write OTA: %d", err);
            return -6;
        }

        bytesWritten += len;
    }

    return bytesWritten;
}

void MultiBoot::cancel() {
    if (file != NULL) {
        fclose(file);
    }
    if (ota_handle) {
        esp_ota_abort(ota_handle);
        ota_handle = 0;
    }
}

int MultiBoot::getBytesTotal() {
    return bytesTotal;
}

int MultiBoot::getBytesWritten() {
    return bytesWritten;
}

int MultiBoot::finishAndReboot() {
    serial_log("Written %d bytes", bytesWritten);

    esp_err_t err = esp_ota_end(ota_handle);
    if (err != ESP_OK) {
        serial_err("Failed to end OTA: %d", err);
        return -7;
    }

    // Перевстановлення активного розділу на OTA-розділ (його буде запущено лише один раз, після чого активним залишиться основний розділ).
    err = esp_ota_set_boot_partition(ota_partition);
    if (err != ESP_OK) {
        serial_err("Failed to set boot partition: %d", err);
        return -8;
    }

    // Запуск нової прошивки.
    esp_restart();

    return 0; // unreachable
}

String MultiBoot::getFirmwarePath() {
    Preferences prefs;
    prefs.begin("lilka", false);
    String arg = "";
    if (prefs.isKey(MULTIBOOT_PATH_KEY)) {
        arg = prefs.getString(MULTIBOOT_PATH_KEY);
        prefs.remove(MULTIBOOT_PATH_KEY);
    }
    prefs.end();
    return arg;
}

MultiBoot multiboot;

} // namespace lilka

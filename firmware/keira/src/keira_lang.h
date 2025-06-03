#pragma once
///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Keira OS Header file
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
// GUIDELINE: Keira all strings used in messages, or somehow written to serial, should be defined here

// Multi purpose strings  /////////////////////////////////////////////////////////////////////////////
#define K_S_ERROR                       "Помилка"
#define K_S_ATTENTION                   "Увага"
#define K_S_SUCCESS                     "Успіх"
#define K_S_PLEASE_STANDBY              "Будь ласка, зачекайте..."
#define K_S_LILKA_V2_OR_HIGHER_REQUIRED "Ця програма потребує Лілку версії 2 або вище."
#define K_S_FEATURE_IN_DEVELOPMENT      "Ця функція перебуває в розробці"
#define K_S_CANT_OPEN_DIR               "Не можу відкрити директорію"
#define K_S_MENU_BACK                   "<< Назад"

#define K_S_PASSWORD                    "Пароль"
#define K_S_USER                        "Користувач"

#define K_S_SETTINGS                    "Налаштування"
#define K_S_ON                          "ON"
#define K_S_OFF                         "OFF"
#define K_S_STATUS                      "Статус"

#define K_S_SERVICES                    "Сервіси"
#define K_S_FTP                         "FTP"

#define K_S_OS_NAME                     "Keira OS"
#define K_S_OS_DESCRIPTION              "by Андерсон & friends"
///////////////////////////////////////////////////////////////////////////////////////////////////////

// service.cpp  ///////////////////////////////////////////////////////////////////////////////////////
#define K_S_SERVICE_STARTUP "Starting service %s"
#define K_S_SERVICE_DIE     "Service %s died"
///////////////////////////////////////////////////////////////////////////////////////////////////////

// apps/launcher.cpp //////////////////////////////////////////////////////////////////////////////////
#define K_S_LAUNCHER_MAIN_MENU         "Головне меню"
#define K_S_LAUNCHER_APPS              "Додатки"
#define K_S_LAUNCHER_DEMOS             "Демо"
#define K_S_LAUNCHER_LINES             "Лінії"
#define K_S_LAUNCHER_DISK              "Диск"
#define K_S_LAUNCHER_TRANSFORM         "Перетворення"
#define K_S_LAUNCHER_BALL              "М'ячик"
#define K_S_LAUNCHER_CUBE              "Куб"
#define K_S_LAUNCHER_EPILEPSY          "Епілепсія"
#define K_S_LAUNCHER_PET_PET           "PetPet"
#define K_S_LAUNCHER_TESTS             "Тести"
#define K_S_LAUNCHER_KEYBOARD          "Клавіатура"
#define K_S_LAUNCHER_SPI_TEST          "Тест SPI"
#define K_S_LAUNCHER_I2C_SCANNER       "I2C-сканер"
#define K_S_LAUNCHER_GPIO_MANAGER      "GPIO-менеджер"
#define K_S_LAUNCHER_COMBO             "Combo" // wtf?
#define K_S_LAUNCHER_CALLBACK_TEST     "CallbackTest"
#define K_S_LAUNCHER_LILCATALOG        "ЛілКаталог"
#define K_S_LAUNCHER_LILTRACKER        "ЛілТрекер"
#define K_S_LAUNCHER_LETRIS            "Летріс"
#define K_S_LAUNCHER_TAMAGOTCHI        "Тамагочі"
#define K_S_LAUNCHER_WEATHER           "Погода"
#define K_S_LAUNCHER_PASTEBIN          "Pastebin"
#define K_S_LAUNCHER_SD_BROWSER        "Браузер SD-карти"
#define K_S_LAUNCHER_SPIFFS_BROWSER    "Браузер SPIFFS"
#define K_S_LAUNCHER_DEV_MENU          "Розробка"
#define K_S_LAUNCHER_LIVE_LUA          "Live Lua" // Lua live?
#define K_S_LAUNCHER_LUA_REPL          "Lua REPL"
#define K_S_LAUNCHER_WIFI_ADAPTER      "WiFi-адаптер"
#define K_S_LAUNCHER_WIFI_NETWORKS     "Мережі WiFi"
#define K_S_LAUNCHER_WIFI_TX_POWER     "Потужність WiFi"
#define K_S_LAUNCHER_SOUND             "Звук"
#define K_S_LAUNCHER_SERVICES          K_S_SERVICES
#define K_S_LAUNCHER_FTP               K_S_FTP
#define K_S_LAUNCHER_FTP_STATUS        K_S_STATUS
#define K_S_LAUNCHER_FTP_USER          K_S_USER
#define K_S_LAUNCHER_FTP_PASSWORD      K_S_PASSWORD

#define K_S_LAUNCHER_ABOUT_SYSTEM      "Про систему"
#define K_S_LAUNCHER_DEVICE_INFO       "Інфо про пристрій"
#define K_S_PARTITION_TABLE            "Таблиця розділів"
#define K_S_LAUNCHER_SD_FORMAT         "Форматування SD-карти"
#define K_S_LAUNCHER_LIGHT_SLEEP       "Light sleep"
#define K_S_LAUNCHER_DEEP_SLEEP        "Deep sleep"
#define K_S_LAUNCHER_REBOOT            "Перезавантаження"

#define K_S_LAUNCHER_SELECT_TX_POWER   "Оберіть потужність"

#define K_S_LAUNCHER_ENABLE_WIFI_FIRST "Спочатку увімкніть WiFi-адаптер"

#define K_S_LAUNCHER_CHIP_MODEL        "Модель: %s"
#define K_S_LAUNCHER_CHIP_REVISION     "Ревізія: %d"
#define K_S_LAUNCHER_ESP_IDF_VERSION   "Версія ESP-IDF: %s"
#define K_S_LAUNCHER_CPU_FREQUENCY     "Частота: %d МГц"
#define K_S_LAUNCHER_CHIP_CORES        "Кількість ядер: %d"
#define K_S_LAUNCHER_IP                "IP: %s"
#define K_S_LAUNCHER_ADDRESS_PREFIX    "Адреса: 0x"
#define K_S_LAUNCHER_SIZE_PREFIX       "Розмір: 0x"
#define K_S_LAUNCHER_FORMAT            "Форматування"

#define K_S_LAUNCHER_FORMAT_DISCLAIMER_ALERT    \
    "УВАГА: Це очистить ВСІ дані з SD-карти!\n" \
    "\nПродовжити?\n\nSTART - продовжити\nA - скасувати"

#define K_S_LAUNCHER_PLEASE_STANDBY K_S_PLEASE_STANDBY

#define K_S_LAUNCHER_FORMAT_ERROR_ALERT    \
    "Не вдалося форматувати SD-карту.\n\n" \
    "Систему буде перезавантажено."

#define K_S_LAUNCHER_FORMAT_SUCCESS_ALLERT \
    "Форматування SD-карти завершено!\n\n" \
    "Систему буде перезавантажено."
///////////////////////////////////////////////////////////////////////////////////////////////////////

// apps/wifi_config.cpp ///////////////////////////////////////////////////////////////////////////////
#define K_S_WIFI_CONFIG_SCANING_NETWORKS                  "Скануємо мережі WiFi..."
#define K_S_WIFI_CONFIG_SCAN_ERROR_CODE_PREFIX            "Не вдалося сканувати мережі, код помилки: "
#define K_S_WIFI_CONFIG_NETWORKS                          "Мережі"
#define K_S_WIFI_CONFIG_CHANNEL_PREFIX                    "Канал: "
#define K_S_WIFI_CONFIG_SIGNAL_STRENGTH_PREFIX            "Сила сигналу: "
#define K_S_WIFI_CONFIG_MAC_PREFIX                        "MAC: "
#define K_S_WIFI_CONFIG_SECURITY_PREFIX                   "Захист: "
#define K_S_WIFI_CONFIG_CONNECTING_TO_OPEN_NETWORK_PREFIX "Ви під'єднуєтеся до незахищеної мережі "

#define K_S_WIFI_CONFIG_CONTINUE_OR_SELECT_ANOTHER_NETWORK \
    "\n\n"                                                 \
    "A - продовжити\n"                                     \
    "B - обрати іншу мережу"
#define K_S_WIFI_CONFIG_ENTER_PASSWORD                 "Введіть пароль:"
#define K_S_WIFI_CONFIG_CONNECTING                     "Під'єднуємось..."
#define K_S_WIFI_CONFIG_CONNECTED_TO_NETWORK_PREFIX    "Під'єднано до мережі "
#define K_S_WIFI_CONFIG_CANT_CONNECT_TO_NETWORK_PREFIX "Не вдалося під'єднатись до мережі "
///////////////////////////////////////////////////////////////////////////////////////////////////////

// apps/demos/scan_i2c.cpp ////////////////////////////////////////////////////////////////////////////
#define K_S_I2C_SCANNER_SDA_PREFIX    "I2C init: SDA="
#define K_S_I2C_SCANNER_SCL_PREFIX    ", SCL="
#define K_S_I2C_SCANNER_SCAN_START    "Starting I2C scan..."
#define K_S_I2C_SCANNER_SCAN_DONE     "I2C scan done."
#define K_S_I2C_SCANNER_DEVICES_FOUND "Found %d devices."
///////////////////////////////////////////////////////////////////////////////////////////////////////

// apps/demos/transform.cpp ///////////////////////////////////////////////////////////////////////////
#define K_S_TRANSFORM_CANT_LOAD_FACE "Не вдалось завантажити face.bmp з SD-карти." // FACEPALM.BMP
#define K_S_DRAWING_FACE_AT_PREFIX   "Drawing face at "
///////////////////////////////////////////////////////////////////////////////////////////////////////

// apps/fmanager.cpp/.h ///////////////////////////////////////////////////////////////////////////////
#define K_S_FMANAGER_COPYING               "Копіювання..."
#define K_S_FMANAGER_MD5_CALC              "Обчислення МD5"
#define K_S_FMANAGER_LOADING               "Завантаження"
#define K_S_FMANAGER_ENTER_NEW_FOLDER_NAME "Введіть назву нової папки"
#define K_S_FMANAGER_ENTER_NEW_NAME        "Введіть нову назву"
#define K_S_FMANAGER_OPTIONS               "Опції"
#define K_S_FMANAGER_OPEN                  "Відкрити"
#define K_S_FMANAGER_OPEN_WITH             "Відкрити з"
#define K_S_FMANAGER_CREATE_FOLDER         "Створити папку"
#define K_S_FMANAGER_RENAME                "Перейменувати"
#define K_S_FMANAGER_DELETE                "Видалити"
#define K_S_FMANAGER_INFO_ABOUT_FILE       "Інформація"
#define K_S_FMANAGER_SELECT_APP            "Оберіть додаток"
#define K_S_FMANAGER_FILE_MANAGER          "Файловий менеджер"
#define K_S_FMANAGER_NES_EMULATOR          "Емулятор NES"
#define K_S_FMANAGER_FIRMWARE_LOADER       "Завантажувач прошивок"
#define K_S_FMANAGER_LUA                   "Lua"
#define K_S_FMANAGER_MJS                   "mJS"
#define K_S_FMANAGER_LILTRACKER            K_S_LAUNCHER_LILTRACKER
#define K_S_FMANAGER_MOD_PLAYER            "Програвач MOD"
#define K_S_FMANAGER_ACTIONS_ON_SELECTED   "Дії над вибраним"
#define K_S_FMANAGER_COPY_SELECTED         "Копіювати вибране"
#define K_S_FMANAGER_MOVE_SELECTED         "Перемістити вибране"
#define K_S_FMANAGER_DELETE_SELECTED       "Видалити вибране"
#define K_S_FMANAGER_CLEAR_SELECTION       "Очистити вибране"
#define K_S_FMANAGER_CALC_INTERRUPTED      "Не обчислено"
#define K_S_FMANAGER_ARE_YOU_SURE_ALERT    "Ви впевнені?"
#define K_S_FMANAGER_THIS_OP_WOULD_DELETE  "Ця операція видалить "
#define K_S_FMANAGER_FILE_S                " файл(ів)"
#define K_S_FMANAGER_CONTINUE_OR_EXIT      "\nПродовжити: START\nВихід: B"
#define K_S_FMANAGER_TYPE_DIR              "Тип: директорія"
#define K_S_FMANAGER_PATH_PREFIX           "Шлях: "
#define K_S_FMANAGER_TYPE_FILE             "Тип: файл"
#define K_S_FMANAGER_SIZE_PREFIX           "Розмір: "
#define K_S_FMANAGER_MD5_PREFIX            "MD5: "
// TODO: MOVE MULTIBOOT TO SEPARATE APP
#define K_S_FMANAGER_MULTIBOOT_STARTING                 "Починаємо..."
#define K_S_FMANAGER_MULTIBOOT_ERROR_STAGE1_CODE_PREFIX "Етап: 1\nКод: "
#define K_S_FMANAGER_MULTIBOOT_ERROR_STAGE2_CODE_PREFIX "Етап: 2\nКод: "
#define K_S_FMANAGER_MULTIBOOT_ERROR_STAGE3_CODE_PREFIX "Етап: 3\nКод: "
#define K_S_FMANAGER_SORTING                            "Сортування..."
#define K_S_FMANAGER_ALMOST_DONE                        "Майже готово..."
#define K_S_FMANAGER_THIS_OP_WOULD_DELETE_FILE          "Ця операція видалить файл\n"
#define K_S_FMANAGER_NOT_ENOUGH_MEMORY_TO_FINISH_OP     "Недостатньо пам'яті аби завершити операіцю"
#define K_S_FMANAGER_SELECTED_FILE_S                    "Вибрано %d файл(ів)"
#define K_S_FMANAGER_CANT_DO_OP                         "Не можу виконати операцію"
#define K_S_FMANAGER_FILE_ADDED_TO_BUFFER_EXCHANGE      "Файл додано в буфер обміну"
///////////////////////////////////////////////////////////////////////////////////////////////////////
// apps/liltracker/liltracker.cpp /////////////////////////////////////////////////////////////////////
#define K_S_LILTRACKER_CANT_CREATE_DIR         "Не вдалося створити директорію"
#define K_S_LILTRACKER_CANT_OPEN_DIR           "Не вдалося відкрити директорію"
#define K_S_LILTRACKER_DIRECTORY_PREFIX        "Директорія "
#define K_S_LILTRACKER_EMPTY_POSTFIX           " порожня"
#define K_S_LILTRACKER_SAVE_TRACK              "Зберегти трек"
#define K_S_LILTRACKER_OPEN_TRACK              "Відкрити трек"
#define K_S_LILTRACKER_CREATE_NEW_TRACK        "++ Створити новий"
#define K_S_LILTRACKER_ENTER_FILENAME          "Введіть назву файлу"
#define K_S_LILTRACKER_FILENAME_CANT_BE_EMPTY  "Назва файлу не може бути порожньою"
#define K_S_LILTRACKER_CANT_OPEN_FILE_PREFIX   "Не вдалося відкрити файл "
#define K_S_LILTRACKER_CANT_OPEN_DIR_PREFIX    "Не вдалося відкрити директорію "
#define K_S_LILTRACKER_CANT_REMOVE_FILE_PREFIX "Не вдалося видалити файл "

///////////////////////////////////////////////////////////////////////////////////////////////////////

// apps/lilcatalog/lilcatalog.cpp /////////////////////////////////////////////////////////////////////
#define K_S_LILCATALOG_LANGUAGE                      "uk"
#define K_S_LILCATALOG_APP                           "ЛілКаталог"

#define K_S_LILCATALOG_FETCH_CATALOG                 "Оновити каталог"
#define K_S_LILCATALOG_STOP                          "Вихід"
#define K_S_LILCATALOG_BACK                          "Назад"
#define K_S_LILCATALOG_EMPTY                         ""

#define K_S_LILCATALOG_START                         "Запустити"
#define K_S_LILCATALOG_INSTALL                       "Встановити"
#define K_S_LILCATALOG_REMOVE                        "Видалити"
#define K_S_LILCATALOG_UPDATE                        "Оновити"
#define K_S_LILCATALOG_ENTRY_DESCRIPTION             "Опис"
#define K_S_LILCATALOG_LOADING                       "Завантаження"
#define K_S_LILCATALOG_STARTING                      "Починаємо..."
#define K_S_LILCATALOG_ENTRY_DESCRIPTION_NAME        "Назва: "
#define K_S_LILCATALOG_ENTRY_DESCRIPTION_AUTHOR      "Автор: "
#define K_S_LILCATALOG_ENTRY_DESCRIPTION_DESCRIPTION "Опис: "
#define K_S_LILCATALOG_ENTRY_DESCRIPTION_FILES       "Файли: "

#define K_S_LILCATALOG_CATEGORY_POSTFIX              " елементів"
#define K_S_LILCATALOG_SIZE                          "Розмір:"
#define K_S_LILCATALOG_FILE_LOADING                  "Завантаження файлу..."
#define K_S_LILCATALOG_FILE_LOADING_COMPLETE         "Файл завантажено, та збережено"

#define K_S_LILCATALOG_ERROR_CREATE_FOLDER           "Помилка створення каталогу"
#define K_S_LILCATALOG_ERROR_NO_CATALOG              "Каталог не знайдено. Завантажте його з інтернету"
#define K_S_LILCATALOG_ERROR_LOAD_CATALOG            "Помилка завантаження каталогу"
#define K_S_LILCATALOG_ERROR_FILE_OPEN               "Помилка відкриття файлу"
#define K_S_LILCATALOG_ERROR_CONNECTION              "Помилка підключення:"
#define K_S_LILCATALOG_ERROR_DIRETORY_CREATE         "Помилка створення директорії"
#define K_S_LILCATALOG_ERROR_STAGE1                  "Етап: 1\nКод: "
#define K_S_LILCATALOG_ERROR_STAGE2                  "Етап: 2\nКод: "
#define K_S_LILCATALOG_ERROR_STAGE3                  "Етап: 3\nКод: "
#define K_S_LILCATALOG_SD_NOTFOUND                   "SD карта не знайдена. Неможливо продовжити"
///////////////////////////////////////////////////////////////////////////////////////////////////////

// apps/demos/letris.cpp //////////////////////////////////////////////////////////////////////////////
#define K_S_LETRIS_GAME_OVER      "Game over"
#define K_S_LETRIS_GAME_OVER_LONG "Гру завершено!\nТи намагався. :)"
///////////////////////////////////////////////////////////////////////////////////////////////////////
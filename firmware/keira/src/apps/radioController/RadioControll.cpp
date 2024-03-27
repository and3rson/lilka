#include "RadioControll.h"
#include <sbus.h>
#include "app.h"

RadioControllApp::RadioControllApp() : App("Ball") {
}

// TODO: add support for ibus, CRSF and other

bfs::SbusTx sbus_tx(&Serial2, -1, 12, false);
// /* SBUS data */

bool isNumeric(String str) {
    // Check if the string is empty
    if (str.length() == 0) {
        return false;
    }

    // Check each character of the string
    for (int i = 0; i < str.length(); i++) {
        // Allow '-' sign only at the beginning
        if (i == 0 && str.charAt(i) == '-') {
            continue;
        }
        // Check if the character is not a digit
        if (!isdigit(str.charAt(i))) {
            return false;
        }
    }

    return true; // If all characters are digits or '-' sign at the beginning, return true
}

bool RadioControllApp::readSettings() {
    FILE* file = fopen("/sd/RadioContollSetting.csv", "r");
    if (!file) {
        Serial.println("Error opening file!");
        return false;
    }

    // Check if the file is empty
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    if (fileSize == 0) {
        Serial.println("File is empty!");
        fclose(file);
        return false;
    }
    fseek(file, 0, SEEK_SET); // Reset file pointer to the beginning

    int lineNum = 0;
    char line[100];
    while (fgets(line, sizeof(line), file)) {
        // Skip empty lines
        if (strlen(line) <= 1) {
            continue;
        }

        if (lineNum < CHDataArray::NUM_CH) {
            // Tokenize the line
            char* token = strtok(line, ",");
            int values[3];
            int index = 0;
            while (token != NULL && index < 3) {
                values[index++] = strtol(token, NULL, 10);
                token = strtok(NULL, ",");
            }

            if (index == 3) {
                // Assign values to CHData struct
                data.ch[lineNum].min_value = values[0];
                data.ch[lineNum].max_value = values[1];
                data.ch[lineNum].mid_value = values[2];
                data.ch[lineNum].current_value = 0; // Set current_value to zero
            } else {
                Serial.print("Error parsing line ");
                Serial.print(lineNum + 1);
                Serial.println(" - Invalid data format");
                Serial.println(line); // Print the problematic line
                fclose(file);
                return false;
            }
        } else {
            Serial.println("Too many lines in CSV file!");
            fclose(file);
            return false;
        }
        lineNum++;
    }
    fclose(file);
    return true;
}

void RadioControllApp::saveSetting() {
    FILE* fp = fopen("/sd/RadioContollSetting.csv", "w+");
    if (fp != NULL) {
        for (int i = 0; i < data.NUM_CH; i++) {
            char buf[100];
            sprintf(buf, "%d,%d,%d;\n", data.ch[i].min_value, data.ch[i].max_value, data.ch[i].mid_value);
            fwrite(buf, sizeof(char), strlen(buf), fp);
        }
        fclose(fp);
    } else {
        lilka::Alert alert("ERROR", "Не можливо зберегти файл");
        alert.draw(canvas);
        queueDraw();
        while (!alert.isFinished()) {
            alert.update();
        }
    }
}

void RadioControllApp::SettingsMenu() {
    lilka::Menu SettingsMenu("CH");
    lilka::Menu CHSettingsMenu("SETTINGS");

    for (int i = 0; i < data.NUM_CH; i++) {
        SettingsMenu.addItem("CH: ", NULL, lilka::display.color565(255, 255, 255), String(i));
    }

    SettingsMenu.addItem("<< Назад");

    while (1) {
        while (!SettingsMenu.isFinished()) {
            SettingsMenu.update();
            SettingsMenu.draw(canvas);
            queueDraw();
        }
        if (SettingsMenu.getCursor() == 4) {
            break;
        } else {
            while (1) {
                CHSettingsMenu.clearItems();
                CHSettingsMenu.addItem(
                    "MIN: ",
                    NULL,
                    lilka::display.color565(255, 255, 255),
                    String(data.ch[SettingsMenu.getCursor()].min_value)
                );
                CHSettingsMenu.addItem(
                    "MAX: ",
                    NULL,
                    lilka::display.color565(255, 255, 255),
                    String(data.ch[SettingsMenu.getCursor()].max_value)
                );
                CHSettingsMenu.addItem(
                    "MID: ",
                    NULL,
                    lilka::display.color565(255, 255, 255),
                    String(data.ch[SettingsMenu.getCursor()].mid_value)
                );
                CHSettingsMenu.addItem("<< Назад");

                while (!CHSettingsMenu.isFinished()) {
                    CHSettingsMenu.update();
                    CHSettingsMenu.draw(canvas);
                    queueDraw();
                }
                if (CHSettingsMenu.getCursor() == 3) {
                    break;
                } else {
                    while (1) {
                        lilka::InputDialog GetValue("Введіть значення");
                        while (!GetValue.isFinished()) {
                            GetValue.update();
                            GetValue.draw(canvas);
                            queueDraw();
                        }
                        String value = GetValue.getValue();
                        if (isNumeric(value)) {
                            if (value.toInt() >= -100 && value.toInt() <= 100) {
                                switch (CHSettingsMenu.getCursor()) {
                                    case 0:
                                        data.ch[SettingsMenu.getCursor()].min_value = value.toInt();
                                        break;
                                    case 1:
                                        data.ch[SettingsMenu.getCursor()].max_value = value.toInt();
                                        break;
                                    case 2:
                                        data.ch[SettingsMenu.getCursor()].mid_value = value.toInt();
                                        break;
                                    default:
                                        break;
                                }
                                break;
                            } else {
                                lilka::Alert alert("Увага", "Значення мають бути від -100 до 100");
                                alert.draw(canvas);
                                queueDraw();
                                while (!alert.isFinished()) {
                                    alert.update();
                                }
                            }
                        } else {
                            lilka::Alert alert("Увага", "ВВедіть число, не текст");
                            alert.draw(canvas);
                            queueDraw();
                            while (!alert.isFinished()) {
                                alert.update();
                            }
                        }
                    }
                }
            }
        }
    }
    this->saveSetting();
}

void RadioControllApp::run() {
    // sbus_tx.Begin();
    time_t timePressMenu;

    if (!this->readSettings()) {
        // CHDataArray new_data;
        // data = new_data;
        this->saveSetting();
    }

    while (1) {
        int leftX, leftY, rightX, rightY;
        lilka::State state = lilka::controller.getState();
        if (state.up.pressed) {
            data.ch[0].current_value = data.ch[0].max_value;
        } else if (state.down.pressed) {
            data.ch[0].current_value = data.ch[0].min_value;
        } else {
            data.ch[0].current_value = data.ch[0].mid_value;
        }

        if (state.left.pressed) {
            data.ch[1].current_value = data.ch[1].min_value;
        } else if (state.right.pressed) {
            data.ch[1].current_value = data.ch[1].max_value;
        } else {
            data.ch[1].current_value = data.ch[1].mid_value;
        }

        if (state.c.pressed) {
            data.ch[2].current_value = data.ch[2].max_value;
        } else if (state.b.pressed) {
            data.ch[2].current_value = data.ch[2].min_value;
        } else {
            data.ch[2].current_value = data.ch[2].mid_value;
        }

        if (state.d.pressed) {
            data.ch[3].current_value = data.ch[3].min_value;
        } else if (state.a.pressed) {
            data.ch[3].current_value = data.ch[3].max_value;
        } else {
            data.ch[3].current_value = data.ch[3].mid_value;
        }

        if (state.left.pressed && state.right.pressed && state.up.pressed && state.down.pressed &&
            state.select.pressed) {
            return;
        }

        canvas->fillScreen(canvas->color565(0, 0, 0));

        canvas->setFont(FONT_9x15);

        canvas->setCursor(10, 20);
        canvas->print("LX");
        canvas->setCursor(10, 50);
        canvas->print(data.ch[0].current_value);

        canvas->setCursor(60, 20);
        canvas->print("LY");
        canvas->setCursor(60, 50);
        canvas->print(data.ch[1].current_value);

        canvas->setCursor(110, 20);
        canvas->print("RX");
        canvas->setCursor(110, 50);
        canvas->print(data.ch[2].current_value);

        canvas->setCursor(160, 20);
        canvas->print("RY");
        canvas->setCursor(160, 50);
        canvas->print(data.ch[3].current_value);

        canvas->setFont(FONT_4x6);
        canvas->setCursor(0, 80);
        canvas->print("PRESS(1s) SELECT AND START FOR SETTING MENU");
        canvas->setCursor(0, 90);
        canvas->print("PRESS ALL LEFT BUTTONS AND SELECT FOR LEAVE");
        bfs::SbusData _data;

        _data.ch[0] = data.ch[0].current_value;
        _data.ch[1] = data.ch[1].current_value;
        _data.ch[2] = data.ch[2].current_value;
        _data.ch[3] = data.ch[3].current_value;

        sbus_tx.data(_data);
        sbus_tx.Write();

        if (state.select.pressed && state.start.pressed) {
            if (millis() - timePressMenu > 1000) {
                this->SettingsMenu();
            }
        } else {
            timePressMenu = millis();
        }

        queueDraw();
    }
}

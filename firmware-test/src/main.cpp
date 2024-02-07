#include <SD.h>

#define SD_SCK 0
#define SD_MISO 8
#define SD_MOSI 1
#define SD_SS 20

#define DBG_PIN 5

#define INTERNAL_TRANSACTION_SPEED 40000000 // 40 MHz

void debugMark() {
    delay(5);
    digitalWrite(DBG_PIN, HIGH);
    delay(5);
    digitalWrite(DBG_PIN, LOW);
    delay(5);
}

void setup() {
    Serial.begin(115200);
    SPI.begin(SD_SCK, SD_MISO, SD_MOSI);
    pinMode(DBG_PIN, OUTPUT);
    digitalWrite(5, LOW);

    delay(2000);

    // Init SD card
    Serial.print("Initializing SD card... ");
    if (!SD.begin(SD_SS)) {
        Serial.println("FAILED!");
        return;
    }
    Serial.println("OK.");

    // Read file
    File f = SD.open("/foo.txt");
    if (f) {
        Serial.println("foo.txt: " + f.readString());
        f.close();
    }

    debugMark();

    // Start internal transaction
    // for (int i = 0; i < 256; i++) {
    SPI.beginTransaction(SPISettings(INTERNAL_TRANSACTION_SPEED, MSBFIRST, SPI_MODE3));
    // SPI.write(i);
    SPI.endTransaction();
    // }

    debugMark();

    // SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE3));
    // SPI.write(0);
    // SPI.endTransaction();

    // Read file again
    f = SD.open("/foo.txt");
    if (f) {
        Serial.println("foo.txt: " + f.readString());
        f.close();
    }
}

void loop() {}

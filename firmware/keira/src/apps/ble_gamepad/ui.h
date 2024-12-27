#pragma once

#include <Arduino.h>
#include <lilka.h>

namespace ble_gamepad_app {

class UI {
public:
    UI(uint16_t width, uint16_t height);
    void drawFrame(bool useDiffColors, int lastSecondsToExit);
    lilka::Canvas* getFrameBuffer();

private:
    static constexpr const int NUM_OF_STEPS = 7;

    static constexpr const uint16_t GRAY_COLORS[] = {0xFFFF, 0xD6BA, 0xAD55, 0x8410, 0x52AA, 0x2965, 0x0};
    static constexpr const uint16_t YELLOW_COLORS[] = {0xFEC0, 0xDDC0, 0xBCE0, 0x93E0, 0x72E0, 0x4A00, 0x2900};
    static constexpr const uint16_t BLUE_COLORS[] = {0x03BF, 0x033B, 0x02B7, 0x0232, 0x01AE, 0x0129, 0x0085};

    static constexpr const char* USER_MESSAGE_TEXTS[] = {"Для виходу", "затисніть", "select %d сек"};

    lilka::Canvas buffer;

    void drawBoundPoint(uint16_t color, int pos);
    void drawUserMessage(int lastSecondsToExit);

    const uint16_t sizeX, sizeY, width, height;
    const int maxPosSide0, maxPosSide1, maxPosSide2, maxPosSide3, quarter;
    int boundPos;

    const int16_t userMessageOriginX, userMessageOriginY, userMessageDxMax, userMessageDxMin;
    int8_t userMessageDx, userMessageDy, userMessageDirection;
};
} // namespace ble_gamepad_app
#include <Arduino.h>
#include "ui.h"

namespace ble_gamepad_app {

constexpr const uint16_t UI::GRAY_COLORS[];
constexpr const uint16_t UI::YELLOW_COLORS[];
constexpr const uint16_t UI::BLUE_COLORS[];

UI::UI(uint16_t width, uint16_t height) :
    buffer(width, height),
    sizeX(8),
    sizeY(8),
    width(width),
    height(height),
    maxPosSide0(width / sizeX - 1),
    maxPosSide1(maxPosSide0 + height / sizeY - 1),
    maxPosSide2(maxPosSide1 + width / sizeX - 1),
    maxPosSide3(maxPosSide2 + height / sizeY - 1),
    quarter((maxPosSide3 + 1) / 4),
    userMessageOriginX(85),
    userMessageOriginY(80),
    userMessageDxMin(-20),
    userMessageDxMax(20) {
    userMessageDirection = -1;
    userMessageDx = userMessageDy = 0;
    boundPos = 0;
}

void UI::drawBoundPoint(uint16_t color, int pos) {
    int16_t x = 0, y = 0;
    if (pos < 0) {
        pos += maxPosSide3;
    } else if (pos > maxPosSide3) {
        pos = pos % maxPosSide3;
    }
    if (pos >= 0 && pos <= maxPosSide0) {
        x = pos * sizeX;
        y = 0;
    } else if (pos > maxPosSide0 && pos <= maxPosSide1) {
        x = width - sizeX;
        y = (pos - maxPosSide0) * sizeY;
    } else if (pos > maxPosSide1 && pos <= maxPosSide2) {
        x = width - sizeX - (pos - maxPosSide1) * sizeX;
        y = height - sizeY;
    } else {
        x = 0;
        y = height - sizeY - (pos - maxPosSide2) * sizeY;
    }
    buffer.fillRect(x, y, sizeX, sizeY, color);
};

void UI::drawUserMessage(int lastSecondsToExit) {
    char userMessageText3[strlen(USER_MESSAGE_TEXTS[2])];
    sprintf(userMessageText3, USER_MESSAGE_TEXTS[2], lastSecondsToExit);

    int leftIndent = userMessageOriginX + userMessageDx;
    int leftIndentCorrection1 = leftIndent + 5;
    int leftIndentCorrection2 = leftIndent - 7;
    int topIndent = userMessageOriginY + userMessageDy;
    int topIndentCorrection1 = topIndent + 18;
    int topIndentCorrection2 = topIndent + 50;
    buffer.setCursor(leftIndent, topIndent);
    buffer.print(USER_MESSAGE_TEXTS[0]);
    buffer.setCursor(leftIndentCorrection1, topIndentCorrection1);
    buffer.print(USER_MESSAGE_TEXTS[1]);
    buffer.setCursor(leftIndentCorrection2, topIndentCorrection2);
    buffer.print(userMessageText3);
}

void UI::drawFrame(bool useDiffColors, int lastSecondsToExit) {
    buffer.fillScreen(lilka::colors::Black);

    const uint16_t* partColor0 = useDiffColors ? YELLOW_COLORS : GRAY_COLORS;
    const uint16_t* partColor1 = useDiffColors ? BLUE_COLORS : GRAY_COLORS;

    // bounds
    for (int8_t i = NUM_OF_STEPS - 1; i >= 0; i--) {
        drawBoundPoint(partColor0[i], boundPos - i);
        drawBoundPoint(partColor1[i], boundPos - i + quarter);
        drawBoundPoint(partColor0[i], boundPos - i + quarter * 2);
        drawBoundPoint(partColor1[i], boundPos - i + quarter * 3);
    }
    boundPos++;
    if (boundPos > maxPosSide3) {
        boundPos = 0;
    }

    // user message
    buffer.setTextSize(1);
    buffer.setTextColor(lilka::colors::White);
    drawUserMessage(lastSecondsToExit);
    userMessageDx += userMessageDirection;
    userMessageDy = -1 * userMessageDx;
    if (userMessageDx > userMessageDxMax || userMessageDx < userMessageDxMin) {
        userMessageDirection *= -1;
    }
}

lilka::Canvas* UI::getFrameBuffer() {
    return &buffer;
}

} // namespace ble_gamepad_app
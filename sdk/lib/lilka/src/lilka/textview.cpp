#include "ui.h"
#define TEXT_VIEW_FILE_BLOCK_SIZE 512
#define TEXT_VIEW_END_MARK        "<<TEXT_END>>"
#define MARGIN_L_R                20 // Left/Right margin for text
#define MARGIN_BOT                20 // Status bar place
#ifdef TEXTVIEW_DBG
#    define TDBG if (1)
#else
#    define TDBG if (0)
#endif
namespace lilka {
TextView::TextView(String text_or_path, bool isFile) {
    if (isFile) {
        pFile = fopen(text_or_path.c_str(), "r");
        if (!pFile) done = true;
        readFileBlock();
    } else {
        textBlock = text_or_path + TEXT_VIEW_END_MARK;
    }
}
TextView::~TextView() {
    if (pFile) fclose(pFile);
}

void TextView::setColors(uint16_t color, uint16_t bgcolor) {
    this->color = color;
    this->bgcolor = bgcolor;
}

void TextView::setFont(const uint8_t* font) {
    if (this->font != font) setTextWrapNeeded();
    this->font = font;
}

void TextView::setLineHeightMultiplier(float lHeightMultiplier) {
    if (this->lHeightMultiplier != lHeightMultiplier) setTextWrapNeeded();
    this->lHeightMultiplier = lHeightMultiplier;
}

void TextView::setTextScale(uint8_t scale) {
    if (this->scale != scale) setTextWrapNeeded();
    this->scale = scale;
}

bool TextView::isFinished() {
    return done;
}

void TextView::draw(Arduino_GFX* canvas) {
    prepareCanvas(canvas);
    auto charWidth = getCharWidth(canvas);
    auto margin_l_r = MARGIN_L_R < charWidth ? charWidth : MARGIN_L_R;
    canvas->setCursor(margin_l_r, 0);

    pLastCanvas = canvas;
    // TODO: optimize drawing cycle
    // limit to 2 redraws?
    if (textWrapNeeded) {
        bool lazy = pFile;
        textWrap(lazy);
    }
    // do draw
    // TODO: implement output by character. This would allow to
    // react on ANSI COLOR codes, which could be useful later
    // for adding syntax highlighting
    uint16_t lineHeight = getLineHeight(canvas);
    uint16_t maxLines = (canvas->height() - MARGIN_BOT) / lineHeight;
    lastDisplayedLines = maxLines;
    for (size_t i = 0; i < lines.size() && i < maxLines; ++i) {
        // there's nothing to draw after end of text
        if (lines[i] == TEXT_VIEW_END_MARK) break;
        canvas->setCursor(margin_l_r, (i + 1) * lineHeight);
        canvas->print(lines[i]);
    }
}

void TextView::update() {
    auto state = controller.getState();
    if (state.c.justPressed) {
        setTextScale(scale + 1);
    } else if (state.b.justPressed) {
        setTextScale(scale - 1);
    } else if (state.down.justPressed) {
        scrollDown();
    } else if (state.up.justPressed) {
        scrollUp();
    } else if (state.left.justPressed) {
        scrollPageUp();
    } else if (state.right.justPressed) {
        scrollPageDown();
    }
}
void TextView::setTextWrapNeeded() {
    textWrapNeeded = true;
    // clear offset history
    while (!fOffsets.empty()) {
        fOffsets.pop();
    }
    lastDisplayedLines = 0;
    foffset = 0;
    readFileBlock();
}

uint16_t TextView::getLineHeight(Arduino_GFX* canvas) {
    int16_t x1 = 0, y1 = 0;
    uint16_t w = 0, h = 0;
    canvas->getTextBounds("A", 0, 0, &x1, &y1, &w, &h);
    return h * lHeightMultiplier;
}

uint16_t TextView::getCharWidth(Arduino_GFX* canvas) {
    int16_t x1, y1;
    uint16_t w, h;
    canvas->getTextBounds("W", 0, 0, &x1, &y1, &w, &h);
    return w;
}

void TextView::scrollUp() {
    if (pFile) {
        if (!lines.empty()) {
            auto NextLine = lines[0];
            if (!fOffsets.empty()) {
                // Get the previous file offset
                foffset = fOffsets.top();
                fOffsets.pop();
                TDBG printf("Restored offset = %lld", foffset);
                readFileBlock();
            }
        }
    } else {
        if (!lines.empty()) {
            String line = lines.back(); // Get the last element
            lines.pop_back(); // Remove the last element
            lines.push_front(line); // Insert it at the front
            if (lines[0] == TEXT_VIEW_END_MARK && lines.size() != 1) scrollUp(); // do it again :)
        }
    }
}
void TextView::scrollPageUp() {
    TDBG printf("Displayed %d lines", lastDisplayedLines);
    for (auto i = 0; i < lastDisplayedLines; i++) {
        scrollUp();
        bool lazy = pFile;
        textWrap(lazy); // recalc values for next scroll
    }
}

void TextView::scrollDown() {
    if (pFile) {
        fOffsets.push(foffset);
        if (!lines.empty()) {
            foffset = foffset + lines[0].length();
            TDBG printf("New offset = %lld", foffset);
            if (lines[0] == TEXT_VIEW_END_MARK) foffset = 0;
            readFileBlock();
            textWrap(true);
        }
    } else {
        if (!lines.empty()) {
            String line = lines.front(); // Get the first element
            lines.pop_front(); // Remove the first element
            lines.push_back(line); // Insert it at the back
            if (lines[0] == TEXT_VIEW_END_MARK && lines.size() != 1) scrollDown(); // do it again :)
        }
    }
}
void TextView::scrollPageDown() {
    for (auto i = 0; i < lastDisplayedLines; i++) {
        scrollDown();
        bool lazy = pFile;
        textWrap(lazy); // recalc values for next scroll
    }
}

void TextView::textWrap(bool lazy) {
    prepareCanvas(pLastCanvas);
    auto charWidth = getCharWidth(pLastCanvas);
    auto margin_l_r = MARGIN_L_R < charWidth ? charWidth : MARGIN_L_R;
    pLastCanvas->setCursor(margin_l_r, 0);

    // TODO: implement HEX/OCT/BIN view
    // TODO: implement wrap by words
    // do something with that, repeats in draw
    int16_t x1, y1;
    uint16_t w, h;
    size_t prePos = 0;
    size_t pos = 0;
    lines.clear(); // remove old lines
    //TDBG printf("Text block length is %d", textBlock.length());
    while (pos < textBlock.length()) {
        if ((!lines.empty()) && lazy && (lines.size() == lastDisplayedLines)) {
            break;
        }

        size_t charLen = 1; // this value we would adjust to fit UTF-8

        unsigned char chr = textBlock[pos];
        TDBG printf("comparing str %s", textBlock.c_str() + pos);
        if (strcmp(textBlock.c_str() + pos, TEXT_VIEW_END_MARK) == 0) {
            // end of text reached
            TDBG printf("ENDMARK found!");
            lines.push_back(TEXT_VIEW_END_MARK);
            break;
        }

        if (chr == '\n') {
            pos++;
            lines.push_back(textBlock.substring(prePos, pos));
            prePos = pos;
            continue;
        }

        // Determine UTF-8 character length
        if ((chr & 0x80) == 0) charLen = 1; // ASCII
        else if ((chr & 0xE0) == 0xC0) charLen = 2; // 2-byte UTF-8
        else if ((chr & 0xF0) == 0xE0) charLen = 3; // 3-byte UTF-8
        else if ((chr & 0xF8) == 0xF0) charLen = 4; // 4-byte UTF-8

        // Check if adding this character would exceed width
        auto segment = textBlock.substring(prePos, pos + charLen);
        pLastCanvas->getTextBounds(segment.c_str(), 0, 0, &x1, &y1, &w, &h);

        if (w > (pLastCanvas->width() - margin_l_r * 2)) {
            // Line is too long with this character, wrap before it
            if (pos > prePos) { // Only add if there's content
                lines.push_back(textBlock.substring(prePos, pos));
                prePos = pos; // Start new line from current character
            } else {
                // Single character is too wide, must include it anyway
                pos += charLen;
                lines.push_back(textBlock.substring(prePos, pos));
                prePos = pos;
            }
        } else {
            // Character fits, move to next one
            pos += charLen;

            // Check for bounds to prevent overflow
            if (pos > textBlock.length()) break;
        }
    }

    // Don't forget to add the last line if there's content remaining
    if (prePos < textBlock.length()) {
        lines.push_back(textBlock.substring(prePos));
    }

    textWrapNeeded = false;
}

void TextView::readFileBlock() {
    if (pFile) {
        char fileBlock[TEXT_VIEW_FILE_BLOCK_SIZE + 1];
        fseek(pFile, foffset, SEEK_SET);
        auto readBytes = fread(fileBlock, 1, TEXT_VIEW_FILE_BLOCK_SIZE, pFile);

        // exchange null-bytes on <0> or something else, if implementing hex/bin/oct/dec view
        fileBlock[readBytes] = 0;
        textBlock = fileBlock;
        if (feof(pFile)) textBlock = textBlock + TEXT_VIEW_END_MARK;
    }
    textWrapNeeded = true;
}
void TextView::prepareCanvas(Arduino_GFX* canvas) {
    // prepare canvas
    canvas->fillScreen(bgcolor);
    canvas->setTextColor(color);
    canvas->setTextSize(scale);
    canvas->setFont(font);
}

} // namespace lilka

#include "display.h"

#include "default_splash.h"
#include "spi.h"
#include "serial.h"
#include "fmath.h"
#include "buzzer.h"

namespace lilka {

#if LILKA_VERSION == 1
Arduino_ESP32SPI displayBus(LILKA_DISPLAY_DC, LILKA_DISPLAY_CS, LILKA_SPI_SCK, LILKA_SPI_MOSI);
#else
// Arduino_ESP32SPI displayBus(
//     LILKA_DISPLAY_DC, LILKA_DISPLAY_CS, LILKA_SPI_SCK, LILKA_SPI_MOSI, LILKA_SPI_MISO, SPI1_NUM, true
// );
Arduino_HWSPI displayBus(
    LILKA_DISPLAY_DC, LILKA_DISPLAY_CS, LILKA_SPI_SCK, LILKA_SPI_MOSI, LILKA_SPI_MISO, &SPI1, true
);
#endif

Display::Display() :
    Arduino_ST7789(
        // &displayBus, LILKA_DISPLAY_RST, LILKA_DISPLAY_ROTATION, true, LILKA_DISPLAY_WIDTH, LILKA_DISPLAY_HEIGHT, 0, 20
        &displayBus, LILKA_DISPLAY_RST, LILKA_DISPLAY_ROTATION, true, LILKA_DISPLAY_WIDTH, LILKA_DISPLAY_HEIGHT, 0, 20,
        0, 20
    ),
    splash(NULL) {
}

void Display::begin() {
    serial_log("initializing display");
#ifdef LILKA_BREADBOARD
    Arduino_ST7789::begin(40000000);
#else
    Arduino_ST7789::begin(80000000);
#endif
    setFont(FONT_10x20);
    setUTF8Print(true);
#ifdef LILKA_NO_SPLASH
    if (splash != NULL) {
#else
    if (splash == NULL) {
        splash = default_splash;
    }
#endif
        uint16_t row[default_splash_width];
        for (int i = 0; i <= 4; i++) {
            startWrite();
            writeAddrWindow(0, 0, default_splash_width, default_splash_height);
            for (int y = 0; y < default_splash_height; y++) {
                for (int x = 0; x < default_splash_width; x++) {
                    uint16_t color = splash[y * default_splash_width + x];
                    uint16_t r = ((color >> 11) & 0x1F) << 3;
                    uint16_t g = ((color >> 5) & 0x3F) << 2;
                    uint16_t b = (color & 0x1F) << 3;
                    row[x] = color565(r * i / 4, g * i / 4, b * i / 4);
                }
                writePixels(row, default_splash_width);
            }
            endWrite();
        }
        // TODO: Should not be here. Треба кудись винести.
        // const Tone helloTune[] = {{NOTE_C4, 8}, {NOTE_E4, 8}, {NOTE_E5, -4}, {NOTE_C6, 8}, {NOTE_C5, 8}};
        const Tone helloTune[] = {{NOTE_C3, 8}, {NOTE_C4, 8}, {NOTE_C5, 8}, {NOTE_C7, 4}, {0, 8}, {NOTE_C6, 4}};
        buzzer.playMelody(helloTune, sizeof(helloTune) / sizeof(Tone), 160);
        delay(800);
        for (int i = 4; i >= 0; i--) {
            startWrite();
            writeAddrWindow(0, 0, default_splash_width, default_splash_height);
            for (int y = 0; y < default_splash_height; y++) {
                for (int x = 0; x < default_splash_width; x++) {
                    uint16_t color = splash[y * default_splash_width + x];
                    uint16_t r = ((color >> 11) & 0x1F) << 3;
                    uint16_t g = ((color >> 5) & 0x3F) << 2;
                    uint16_t b = (color & 0x1F) << 3;
                    row[x] = color565(r * i / 4, g * i / 4, b * i / 4);
                }
                writePixels(row, default_splash_width);
            }
            endWrite();
        }
#ifdef LILKA_NO_SPLASH
    }
#else
#endif
    serial_log("display ok");
}

void Display::setSplash(const uint16_t* splash) {
    this->splash = splash;
}

void Display::drawImage(Image* image, int16_t x, int16_t y) {
    if (image->transparentColor == -1) {
        draw16bitRGBBitmap(x, y, image->pixels, image->width, image->height);
    } else {
        draw16bitRGBBitmapWithTranColor(x, y, image->pixels, image->transparentColor, image->width, image->height);
    }
}

void Display::drawImageTransformed(Image* image, int16_t x, int16_t y, Transform transform) {
    int16_t w = image->width;
    int16_t h = image->height;
    int16_t x0 = transform.matrix[0][0] * x + transform.matrix[0][1] * y;
    int16_t y0 = transform.matrix[1][0] * x + transform.matrix[1][1] * y;
    int16_t x1 = transform.matrix[0][0] * (x + w) + transform.matrix[0][1] * y;
    int16_t y1 = transform.matrix[1][0] * (x + w) + transform.matrix[1][1] * y;
    int16_t x2 = transform.matrix[0][0] * (x + w) + transform.matrix[0][1] * (y + h);
    int16_t y2 = transform.matrix[1][0] * (x + w) + transform.matrix[1][1] * (y + h);
    int16_t x3 = transform.matrix[0][0] * x + transform.matrix[0][1] * (y + h);
    int16_t y3 = transform.matrix[1][0] * x + transform.matrix[1][1] * (y + h);
    int16_t minX = min(min(x0, x1), min(x2, x3));
    int16_t minY = min(min(y0, y1), min(y2, y3));
    int16_t maxX = max(max(x0, x1), max(x2, x3));
    int16_t maxY = max(max(y0, y1), max(y2, y3));
    int16_t destWidth = maxX - minX;
    int16_t destHeight = maxY - minY;
    Image dest(destWidth, destHeight, 0);
    for (int y = minY; y < maxY; y++) {
        for (int x = minX; x < maxX; x++) {
            int16_t srcX = transform.matrix[0][0] * x + transform.matrix[0][1] * y;
            int16_t srcY = transform.matrix[1][0] * x + transform.matrix[1][1] * y;
            if (srcX >= x && srcX < x + w) {
                if (srcY >= y && srcY < y + h) {
                    dest.pixels[x - minX + (y - minY) * destWidth] = image->pixels[srcX - x + (srcY - y) * w];
                } else {
                    dest.pixels[x - minX + (y - minY) * destWidth] = image->transparentColor;
                }
            }
        }
    }
    drawImage(&dest, minX, minY);
}

// Чомусь в Arduino_GFX немає варіанту цього методу для const uint16_t[] - є лише для uint16_t.
void Display::draw16bitRGBBitmapWithTranColor(
    int16_t x, int16_t y, const uint16_t bitmap[], uint16_t transparent_color, int16_t w, int16_t h
) {
    // Цей cast безпечний, оскільки Arduino_GFX.draw16bitRGBBitmapWithTranColor не змінює bitmap.
    Arduino_ST7789::draw16bitRGBBitmapWithTranColor(x, y, const_cast<uint16_t*>(bitmap), transparent_color, w, h);
}

void Display::renderCanvas(Canvas* canvas) {
    draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), canvas->width(), canvas->height());
}

Canvas::Canvas() : Arduino_Canvas(LILKA_DISPLAY_WIDTH, LILKA_DISPLAY_HEIGHT, NULL) {
    setFont(u8g2_font_10x20_t_cyrillic);
    setUTF8Print(true);
    begin();
}

Canvas::Canvas(uint16_t width, uint16_t height) : Arduino_Canvas(width, height, NULL) {
    setFont(u8g2_font_10x20_t_cyrillic);
    setUTF8Print(true);
    begin();
}

Canvas::Canvas(uint16_t x, uint16_t y, uint16_t width, uint16_t height) :
    Arduino_Canvas(width, height, NULL, x, y, 0) { // TODO: Rotation
    setFont(u8g2_font_10x20_t_cyrillic);
    setUTF8Print(true);
    begin();
}

void Canvas::drawImage(Image* image, int16_t x, int16_t y) {
    if (image->transparentColor == -1) {
        draw16bitRGBBitmap(x, y, image->pixels, image->width, image->height);
    } else {
        draw16bitRGBBitmapWithTranColor(x, y, image->pixels, image->transparentColor, image->width, image->height);
    }
}

void Canvas::drawImageTransformed(Image* image, int16_t x, int16_t y, Transform transform) {
    int16_t w = image->width;
    int16_t h = image->height;
    int16_t x0 = transform.matrix[0][0] * x + transform.matrix[0][1] * y;
    int16_t y0 = transform.matrix[1][0] * x + transform.matrix[1][1] * y;
    int16_t x1 = transform.matrix[0][0] * (x + w) + transform.matrix[0][1] * y;
    int16_t y1 = transform.matrix[1][0] * (x + w) + transform.matrix[1][1] * y;
    int16_t x2 = transform.matrix[0][0] * (x + w) + transform.matrix[0][1] * (y + h);
    int16_t y2 = transform.matrix[1][0] * (x + w) + transform.matrix[1][1] * (y + h);
    int16_t x3 = transform.matrix[0][0] * x + transform.matrix[0][1] * (y + h);
    int16_t y3 = transform.matrix[1][0] * x + transform.matrix[1][1] * (y + h);
    int16_t minX = min(min(x0, x1), min(x2, x3));
    int16_t minY = min(min(y0, y1), min(y2, y3));
    int16_t maxX = max(max(x0, x1), max(x2, x3));
    int16_t maxY = max(max(y0, y1), max(y2, y3));
    int16_t destWidth = maxX - minX;
    int16_t destHeight = maxY - minY;
    Image dest(destWidth, destHeight, 0);
    for (int y = minY; y < maxY; y++) {
        for (int x = minX; x < maxX; x++) {
            int16_t srcX = transform.matrix[0][0] * x + transform.matrix[0][1] * y;
            int16_t srcY = transform.matrix[1][0] * x + transform.matrix[1][1] * y;
            if (srcX >= x && srcX < x + w) {
                if (srcY >= y && srcY < y + h) {
                    // Піксель вихідного зображення знаходиться в межах вхідного зображення
                    dest.pixels[x - minX + (y - minY) * destWidth] = image->pixels[srcX - x + (srcY - y) * w];
                } else {
                    // Піксель вихідного зображення знаходиться поза межами вхідного зображення,
                    // тому він повинен бути прозорим.
                    dest.pixels[x - minX + (y - minY) * destWidth] = image->transparentColor;
                }
            }
        }
    }
    drawImage(&dest, minX, minY);
}

void Canvas::draw16bitRGBBitmapWithTranColor(
    int16_t x, int16_t y, const uint16_t bitmap[], uint16_t transparent_color, int16_t w, int16_t h
) {
    // Цей cast безпечний, оскільки Arduino_GFX.draw16bitRGBBitmapWithTranColor не змінює bitmap.
    Arduino_Canvas::draw16bitRGBBitmapWithTranColor(x, y, const_cast<uint16_t* const>(bitmap), transparent_color, w, h);
    // Arduino_Canvas::draw16bitRGBBitmapWithTranColor(x, y, (uint16_t *)(bitmap), transparent_color, w, h);
}

void Canvas::drawCanvas(Canvas* canvas) {
    draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), canvas->width(), canvas->height());
}

int16_t Canvas::x() {
    return _output_x;
}

int16_t Canvas::y() {
    return _output_y;
}

Image::Image(uint32_t width, uint32_t height, int32_t transparentColor) :
    width(width), height(height), transparentColor(transparentColor) {
    // Allocate pixels in PSRAM
    pixels = static_cast<uint16_t*>(ps_malloc(width * height * sizeof(uint16_t)));
}

Image::~Image() {
    delete[] pixels;
}

void Image::rotate(int16_t angle, Image* dest, int32_t blankColor) {
    // Rotate the image clockwise (Y-axis points down)
    int cx = width / 2;
    int cy = height / 2;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int dx = x - cx;
            int dy = y - cy;
            int x2 = cx + dx * fCos360(angle) + dy * fSin360(angle);
            int y2 = cy - dx * fSin360(angle) + dy * fCos360(angle);
            if (x2 >= 0 && x2 < width && y2 >= 0 && y2 < height) {
                dest->pixels[x + y * width] = pixels[x2 + y2 * width];
            } else {
                dest->pixels[x + y * width] = blankColor;
            }
        }
    }
}

void Image::flipX(Image* dest) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            dest->pixels[x + y * width] = pixels[(width - 1 - x) + y * width];
        }
    }
}

void Image::flipY(Image* dest) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            dest->pixels[x + y * width] = pixels[x + (height - 1 - y) * width];
        }
    }
}

Transform::Transform() : matrix{{1, 0}, {0, 1}} {
}

// Copy constructor
Transform::Transform(const Transform& other) {
    matrix[0][0] = other.matrix[0][0];
    matrix[0][1] = other.matrix[0][1];
    matrix[1][0] = other.matrix[1][0];
    matrix[1][1] = other.matrix[1][1];
}

// Copy assignment operator
Transform& Transform::operator=(const Transform& other) {
    if (this != &other) {
        matrix[0][0] = other.matrix[0][0];
        matrix[0][1] = other.matrix[0][1];
        matrix[1][0] = other.matrix[1][0];
        matrix[1][1] = other.matrix[1][1];
    }
    return *this;
}

Transform Transform::multiply(Transform other) {
    // Apply other transform to this transform
    Transform t;
    t.matrix[0][0] = matrix[0][0] * other.matrix[0][0] + matrix[0][1] * other.matrix[1][0];
    t.matrix[0][1] = matrix[0][0] * other.matrix[0][1] + matrix[0][1] * other.matrix[1][1];
    t.matrix[1][0] = matrix[1][0] * other.matrix[0][0] + matrix[1][1] * other.matrix[1][0];
    t.matrix[1][1] = matrix[1][0] * other.matrix[0][1] + matrix[1][1] * other.matrix[1][1];
    return t;
}

Transform Transform::rotate(int16_t angle) {
    // Rotate this transform by angle. Do this by multiplying with a rotation matrix.
    Transform t;
    t.matrix[0][0] = fCos360(angle);
    t.matrix[0][1] = -fSin360(angle);
    t.matrix[1][0] = fSin360(angle);
    t.matrix[1][1] = fCos360(angle);
    // Apply the rotation to this transform
    return t.multiply(*this);
}

Transform Transform::scale(float sx, float sy) {
    // Scale this transform by sx and sy. Do this by multiplying with a scaling matrix.
    Transform t;
    t.matrix[0][0] = sx;
    t.matrix[0][1] = 0;
    t.matrix[1][0] = 0;
    t.matrix[1][1] = sy;
    // Apply the scaling to this transform
    return t.multiply(*this);
}

Display display;

} // namespace lilka

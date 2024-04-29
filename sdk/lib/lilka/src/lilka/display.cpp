#include "display.h"

#include "default_splash.h"
#include "spi.h"
#include "serial.h"
#include "fmath.h"
#include "buzzer.h"

namespace lilka {

template class GFX<Display>;
template class GFX<Canvas>;

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
    splash(default_splash),
    rleLength(default_splash_length) {
    // Apply rotation immediately.
    // This is necessary because setRotation is called in begin(), so display width/height are not valid at this point.
    // We call this so that width/height are valid as early as possible.
    Arduino_TFT::setRotation(LILKA_DISPLAY_ROTATION);
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
    if (splash != NULL) {
        uint16_t row[display.width()];
        for (int i = 0; i <= 4; i++) {
            startWrite();
            writeAddrWindow(0, 0, display.width(), display.height());
            RLEDecoder decoder(static_cast<const uint8_t*>(splash), rleLength);
            for (int y = 0; y < display.height(); y++) {
                for (int x = 0; x < display.width(); x++) {
                    uint16_t color;
                    if (rleLength) {
                        color = decoder.next();
                    } else {
                        color = static_cast<const uint16_t*>(splash)[y * display.width() + x];
                    }
                    uint16_t r = ((color >> 11) & 0x1F) << 3;
                    uint16_t g = ((color >> 5) & 0x3F) << 2;
                    uint16_t b = (color & 0x1F) << 3;
                    row[x] = color565(r * i / 4, g * i / 4, b * i / 4);
                }
                writePixels(row, display.width());
            }
            endWrite();
        }
        delay(800);
        for (int i = 4; i >= 0; i--) {
            startWrite();
            writeAddrWindow(0, 0, display.width(), display.height());
            RLEDecoder decoder(static_cast<const uint8_t*>(splash), rleLength);
            for (int y = 0; y < display.height(); y++) {
                for (int x = 0; x < display.width(); x++) {
                    uint16_t color;
                    if (rleLength) {
                        color = decoder.next();
                    } else {
                        color = static_cast<const uint16_t*>(splash)[y * display.width() + x];
                    }
                    uint16_t r = ((color >> 11) & 0x1F) << 3;
                    uint16_t g = ((color >> 5) & 0x3F) << 2;
                    uint16_t b = (color & 0x1F) << 3;
                    row[x] = color565(r * i / 4, g * i / 4, b * i / 4);
                }
                writePixels(row, display.width());
            }
            endWrite();
        }
    }
    serial_log("display ok");
}

void Display::setSplash(const void* splash, uint32_t rleLength) {
    this->splash = splash;
    this->rleLength = rleLength;
}

uint16_t Display::color565hsv(uint16_t h, uint8_t s, uint8_t v) {
    uint8_t region, remainder, p, q, t;
    uint16_t red, green, blue;

    if (s == 0) {
        red = green = blue = (v * 31) / 100;
        return (red << 11) | (green << 5) | blue;
    }

    region = h / 60;
    remainder = (h - (region * 60)) * 6;

    p = (v * (100 - s)) / 100;
    q = (v * (100 - (s * remainder) / 100)) / 100;
    t = (v * (100 - (s * (60 - remainder)) / 100)) / 100;

    switch (region) {
        case 0:
            red = v;
            green = t;
            blue = p;
            break;
        case 1:
            red = q;
            green = v;
            blue = p;
            break;
        case 2:
            red = p;
            green = v;
            blue = t;
            break;
        case 3:
            red = p;
            green = q;
            blue = v;
            break;
        case 4:
            red = t;
            green = p;
            blue = v;
            break;
        default:
            red = v;
            green = p;
            blue = q;
            break;
    }

    red = (red * 31) / 100;
    green = (green * 63) / 100;
    blue = (blue * 31) / 100;

    return (red << 11) | (green << 5) | blue;
}

template <typename T>
void GFX<T>::drawImage(Image* image, int16_t x, int16_t y) {
    Arduino_GFX* base = static_cast<T*>(this);
    if (image->transparentColor == -1) {
        base->draw16bitRGBBitmap(x - image->pivotX, y - image->pivotY, image->pixels, image->width, image->height);
    } else {
        base->draw16bitRGBBitmapWithTranColor(
            x - image->pivotX, y - image->pivotY, image->pixels, image->transparentColor, image->width, image->height
        );
    }
}

template <typename T>
void GFX<T>::drawImageTransformed(Image* image, int16_t destX, int16_t destY, Transform transform) {
    // Transform image around its pivot.
    // Draw the rotated image at the specified position.

    int32_t imageWidth = image->width;
    int32_t imageHeight = image->height;

    // Calculate the coordinates of the four corners of the destination rectangle.
    int_vector_t v1 = transform.transform(int_vector_t{-image->pivotX, -image->pivotY});
    int_vector_t v2 = transform.transform(int_vector_t{imageWidth - image->pivotX, -image->pivotY});
    int_vector_t v3 = transform.transform(int_vector_t{-image->pivotX, imageHeight - image->pivotY});
    int_vector_t v4 = transform.transform(int_vector_t{imageWidth - image->pivotX, imageHeight - image->pivotY});

    // Find the bounding box of the transformed image.
    int_vector_t topLeft = int_vector_t{min(min(v1.x, v2.x), min(v3.x, v4.x)), min(min(v1.y, v2.y), min(v3.y, v4.y))};
    int_vector_t bottomRight =
        int_vector_t{max(max(v1.x, v2.x), max(v3.x, v4.x)), max(max(v1.y, v2.y), max(v3.y, v4.y))};

    if (bottomRight.x - topLeft.x == 0 || bottomRight.y - topLeft.y == 0) {
        // The transformed image is empty.
        lilka::serial_err("Transform leads to image with zero width or height");
        return;
    }

    // Create a new image to hold the transformed image.
    Image destImage(bottomRight.x - topLeft.x, bottomRight.y - topLeft.y, image->transparentColor, 0, 0);

    // Draw the transformed image to the new image.
    Transform inverse = transform.inverse();
    int_vector_t point{0, 0};
    for (point.y = topLeft.y; point.y < bottomRight.y; point.y++) {
        for (point.x = topLeft.x; point.x < bottomRight.x; point.x++) {
            int_vector_t v = inverse.transform(point);
            // Apply pivot offset
            v.x += image->pivotX;
            v.y += image->pivotY;
            if (v.x >= 0 && v.x < image->width && v.y >= 0 && v.y < image->height) {
                destImage.pixels[point.x - topLeft.x + (point.y - topLeft.y) * destImage.width] =
                    image->pixels[v.x + v.y * image->width];
            } else {
                destImage.pixels[point.x - topLeft.x + (point.y - topLeft.y) * destImage.width] =
                    image->transparentColor;
            }
        }
    }

    drawImage(&destImage, destX + topLeft.x, destY + topLeft.y);
}

// Чомусь в Arduino_GFX немає варіанту цього методу для const uint16_t[] - є лише для uint16_t.
void Display::draw16bitRGBBitmapWithTranColor(
    int16_t x, int16_t y, const uint16_t bitmap[], uint16_t transparent_color, int16_t w, int16_t h
) {
    // Цей cast безпечний, оскільки Arduino_GFX.draw16bitRGBBitmapWithTranColor не змінює bitmap.
    Arduino_ST7789::draw16bitRGBBitmapWithTranColor(x, y, const_cast<uint16_t*>(bitmap), transparent_color, w, h);
}

uint8_t* Display::getFont() {
    return u8g2Font;
}

void Display::drawCanvasInterlaced(Canvas* canvas, bool odd) {
    this->startWrite();
    for (int y = odd ? 1 : 0; y < canvas->height(); y += 2) {
        this->writeAddrWindow(canvas->x(), canvas->y() + y, canvas->width(), 1);
        this->writePixels(canvas->getFramebuffer() + y * canvas->width(), canvas->width());
    }
    this->endWrite();
}

void Canvas::draw16bitRGBBitmapWithTranColor(
    int16_t x, int16_t y, const uint16_t bitmap[], uint16_t transparent_color, int16_t w, int16_t h
) {
    // Цей cast безпечний, оскільки Arduino_GFX.draw16bitRGBBitmapWithTranColor не змінює bitmap.
    Arduino_Canvas::draw16bitRGBBitmapWithTranColor(x, y, const_cast<uint16_t* const>(bitmap), transparent_color, w, h);
}

template <typename T>
void GFX<T>::drawCanvas(Canvas* canvas) {
    static_cast<T*>(this)->draw16bitRGBBitmap(
        canvas->x(), canvas->y(), canvas->getFramebuffer(), canvas->width(), canvas->height()
    );
}

Canvas::Canvas() : Arduino_Canvas(display.width(), display.height(), NULL) {
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

template <typename T>
int GFX<T>::drawTextAligned(const char* text, int16_t x, int16_t y, Alignment hAlign, Alignment vAlign) {
    // TODO: WARNING: This will break if we're not using U8g2 fonts.
    int16_t _x1, _y1;
    uint16_t w, _h;
    // U8g2 is a can of worms.
    T* base = static_cast<T*>(this);
    const uint8_t* font = base->getFont();
    const int8_t ascent = font[13]; // >0 (above the baseline, character 'A')
    const int8_t descent = font[14]; // <0 (below the baseline, character 'g')
    base->getTextBounds(text, 0, 0, &_x1, &_y1, &w, &_h);
    switch (hAlign) {
        case Alignment::ALIGN_START:
            break;
        case Alignment::ALIGN_CENTER:
            x -= w / 2;
            break;
        case Alignment::ALIGN_END:
            x -= w;
            break;
    }
    switch (vAlign) {
        case Alignment::ALIGN_START:
            y += ascent;
            break;
        case Alignment::ALIGN_CENTER:
            y += (ascent - descent) / 2;
            break;
        case Alignment::ALIGN_END:
            y += descent;
            break;
    }
    base->setCursor(x, y);
    base->print(text);
    return w;
}

template <typename T>
void GFX<T>::getTextBoundsAligned(
    const char* text, int16_t x, int16_t y, Alignment hAlign, Alignment vAlign, int16_t* x1, int16_t* y1, uint16_t* w,
    uint16_t* h
) {
    // TODO: WARNING: This will break if we're not using U8g2 fonts.
    // U8g2 is a can of worms.
    T* base = static_cast<T*>(this);
    const uint8_t* font = base->getFont();
    const int8_t ascent = font[13]; // >0 (above the baseline, character 'A')
    const int8_t descent = font[14]; // <0 (below the baseline, character 'g')
    base->getTextBounds(text, x, y, x1, y1, w, h);
    switch (hAlign) {
        case Alignment::ALIGN_START:
            break;
        case Alignment::ALIGN_CENTER:
            *x1 -= *w / 2;
            break;
        case Alignment::ALIGN_END:
            *x1 -= *w;
            break;
    }
    switch (vAlign) {
        case Alignment::ALIGN_START:
            *y1 += ascent;
            break;
        case Alignment::ALIGN_CENTER:
            *y1 += (ascent - descent) / 2;
            break;
        case Alignment::ALIGN_END:
            *y1 += descent;
            break;
    }
}

int16_t Canvas::x() {
    return _output_x;
}

int16_t Canvas::y() {
    return _output_y;
}

uint8_t* Canvas::getFont() {
    return u8g2Font;
}

int16_t getTextWidth(const uint8_t* font, const char* text) {
    lilka::Canvas canvas(4096, 1);
    canvas.setFont(font);
    int16_t x1, y1;
    uint16_t w, h;
    canvas.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    return w;
}

Image::Image(uint32_t width, uint32_t height, int32_t transparentColor, int16_t pivotX, int16_t pivotY) :
    width(width), height(height), transparentColor(transparentColor), pivotX(pivotX), pivotY(pivotY) {
    // Allocate pixels in PSRAM
    pixels = static_cast<uint16_t*>(ps_malloc(width * height * sizeof(uint16_t)));
}

Image::~Image() {
    delete[] pixels;
}

Image* Image::newFromRLE(
    const uint8_t* data, uint32_t length, uint32_t width, uint32_t height, int32_t transparentColor, int16_t pivotX,
    int16_t pivotY
) {
    Image* image = new Image(width, height, transparentColor, width / 2, height / 2);
    RLEDecoder decoder(data, length);
    for (uint32_t i = 0; i < width * height; i++) {
        image->pixels[i] = decoder.next();
    }
    return image;
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
    if (sx == 0 || sy == 0) {
        // Scaling by zero is not allowed
        serial_err("Scaling by zero is not allowed, attempted to scale by %f, %f", sx, sy);
        return *this;
    }
    Transform t;
    t.matrix[0][0] = sx;
    t.matrix[0][1] = 0;
    t.matrix[1][0] = 0;
    t.matrix[1][1] = sy;
    // Apply the scaling to this transform
    return t.multiply(*this);
}

Transform Transform::inverse() {
    // Calculate the inverse of this transform
    Transform t;
    float det = matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
    t.matrix[0][0] = matrix[1][1] / det;
    t.matrix[0][1] = -matrix[0][1] / det;
    t.matrix[1][0] = -matrix[1][0] / det;
    t.matrix[1][1] = matrix[0][0] / det;
    return t;
}

inline int_vector_t Transform::transform(int_vector_t v) {
    // Apply this transform to a vector
    return int_vector_t{
        static_cast<int32_t>(matrix[0][0] * v.x + matrix[0][1] * v.y),
        static_cast<int32_t>(matrix[1][0] * v.x + matrix[1][1] * v.y),
    };
}

RLEDecoder::RLEDecoder(const uint8_t* data, uint32_t length) :
    data(data), length(length), pos(0), count(0), current(0) {
}

uint16_t RLEDecoder::next() {
    if (count == 0) {
        // Read the next value from the RLE stream
        if (pos >= length) {
            // End of stream
            return lilka::colors::Yellow;
        }
        count = data[pos++];
        if (count == 0) {
            // This is bullshit. The count should never be zero. The user has messed up the data. I don't want to deal with this. /AD
            return lilka::colors::Black;
        }
        uint8_t lo = data[pos++];
        uint8_t hi = data[pos++];
        current = (hi << 8) | lo;
    }
    count--;
    return current;
}

Display display;

} // namespace lilka

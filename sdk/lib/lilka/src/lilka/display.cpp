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
        draw16bitRGBBitmap(x - image->pivotX, y - image->pivotY, image->pixels, image->width, image->height);
    } else {
        draw16bitRGBBitmapWithTranColor(
            x - image->pivotX, y - image->pivotY, image->pixels, image->transparentColor, image->width, image->height
        );
    }
}

void Display::drawImageTransformed(Image* image, int16_t destX, int16_t destY, Transform transform) {
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

    // Create a new image to hold the transformed image.
    Image destImage(bottomRight.x - topLeft.x, bottomRight.y - topLeft.y, image->transparentColor, 0, 0);

    // Draw the transformed image to the new image.
    Transform inverse = transform.inverse();
    for (int y = topLeft.y; y < bottomRight.y; y++) {
        for (int x = topLeft.x; x < bottomRight.x; x++) {
            int_vector_t v = inverse.transform(int_vector_t{x, y});
            // Apply pivot offset
            v.x += image->pivotX;
            v.y += image->pivotY;
            if (v.x >= 0 && v.x < image->width && v.y >= 0 && v.y < image->height) {
                destImage.pixels[x - topLeft.x + (y - topLeft.y) * destImage.width] =
                    image->pixels[v.x + v.y * image->width];
            } else {
                destImage.pixels[x - topLeft.x + (y - topLeft.y) * destImage.width] = image->transparentColor;
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
        draw16bitRGBBitmap(x - image->pivotX, y - image->pivotY, image->pixels, image->width, image->height);
    } else {
        draw16bitRGBBitmapWithTranColor(
            x - image->pivotX, y - image->pivotY, image->pixels, image->transparentColor, image->width, image->height
        );
    }
}

void Canvas::drawImageTransformed(Image* image, int16_t destX, int16_t destY, Transform transform) {
    // Transform image around its pivot.
    // Draw the rotated image at the specified position.

    // Calculate the coordinates of the four corners of the destination rectangle.
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

    // Create a new image to hold the transformed image.
    Image destImage(bottomRight.x - topLeft.x, bottomRight.y - topLeft.y, image->transparentColor, 0, 0);

    // Draw the transformed image to the new image.
    Transform inverse = transform.inverse();
    uint64_t start = esp_timer_get_time();
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
    uint64_t end = esp_timer_get_time();
    Serial.println("Transformed image in " + String(end - start) + " us");

    // TODO: Draw directly to the canvas?
    drawImage(&destImage, destX + topLeft.x, destY + topLeft.y);
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

Image::Image(uint32_t width, uint32_t height, int32_t transparentColor, int16_t pivotX, int16_t pivotY) :
    width(width), height(height), transparentColor(transparentColor), pivotX(pivotX), pivotY(pivotY) {
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
        static_cast<int32_t>(matrix[1][0] * v.x + matrix[1][1] * v.y)
    };
}

Display display;

} // namespace lilka

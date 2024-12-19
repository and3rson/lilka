#include <lodepng.h>

#include "screenshot.h"
#include "appmanager.h"
#include "servicemanager.h"
#include "clock.h"

#if !defined(KEIRA_SCREENSHOT_BMP) && !defined(KEIRA_SCREENSHOT_PNG)
// Uncomment one of the following lines to choose the screenshot format
// #    define KEIRA_SCREENSHOT_BMP
#    define KEIRA_SCREENSHOT_PNG
#endif

#ifdef KEIRA_SCREENSHOT_BMP
class BMPEncoder {
public:
    BMPEncoder(const uint16_t* data, uint16_t width, uint16_t height) :
        data(data), width(width), height(height), offset(0) {
    }

    uint32_t getLength() {
        return 14 + 40 + width * height * 3;
    }

    bool encode(uint8_t* dest) {
        uint32_t length = getLength();
        offset = 0;

        // Write header (14 bytes)
        writeByte(dest, 'B'); // Signature
        writeByte(dest, 'M');
        writeDWord(dest, length); // File size
        writeWord(dest, 0); // Reserved
        writeWord(dest, 0); // Reserved
        writeDWord(dest, 14 + 40); // Offset to pixel data

        // Write DIB header (BITMAPINFOHEADER, 40 bytes)
        writeDWord(dest, 40); // Header size
        writeDWord(dest, width); // Width
        writeDWord(dest, height); // Height
        writeWord(dest, 1); // Planes
        writeWord(dest, 24); // Bits per pixel
        writeDWord(dest, 0); // Compression
        writeDWord(dest, width * height * 3); // Raw pixel data size
        writeDWord(dest, 0); // Horizontal resolution
        writeDWord(dest, 0); // Vertical resolution
        writeDWord(dest, 0); // Colors in palette
        writeDWord(dest, 0); // Important colors

        // Write pixel data
        for (int16_t y = height - 1; y >= 0; y--) {
            for (uint16_t x = 0; x < width; x++) {
                uint16_t pixel = data[y * width + x];
                // Convert RGB565 to RGB24
                uint8_t r = (pixel >> 11) & 0x1F;
                uint8_t g = (pixel >> 5) & 0x3F;
                uint8_t b = pixel & 0x1F;
                r = (r * 255) / 31;
                g = (g * 255) / 63;
                b = (b * 255) / 31;
                writeByte(dest, b);
                writeByte(dest, g);
                writeByte(dest, r);
            }
        }
        if (offset != length) {
            lilka::serial_err("BMPEncoder error: offset != length");
            return false;
        }
        return true;
    }

    void writeByte(uint8_t* dest, uint8_t byte) {
        dest[offset++] = byte;
    }

    void writeWord(uint8_t* dest, uint16_t word) {
        dest[offset++] = word & 0xFF;
        dest[offset++] = word >> 8;
    }

    void writeDWord(uint8_t* dest, uint32_t dword) {
        dest[offset++] = dword & 0xFF;
        dest[offset++] = (dword >> 8) & 0xFF;
        dest[offset++] = (dword >> 16) & 0xFF;
        dest[offset++] = dword >> 24;
    }

private:
    const uint16_t* data;
    uint16_t width;
    uint16_t height;
    uint32_t offset;
};
#endif

ScreenshotService::ScreenshotService() : Service("clock") {
}

void ScreenshotService::run() {
    bool activated = false;
    lilka::Canvas* canvas;
    while (1) {
        lilka::State state = lilka::controller.peekState();
        if (state.select.pressed && state.start.pressed && !activated) {
            activated = true;

            // Init canvas
            canvas = new lilka::Canvas(lilka::display.width(), lilka::display.height());
            std::unique_ptr<lilka::Canvas> canvasPtr(canvas);

            // Take screenshot
            AppManager* appManager = AppManager::getInstance();
            appManager->renderToCanvas(canvas);

            uint32_t length = 0;
            uint8_t* buffer = NULL;

            bool success = true;

#if defined(KEIRA_SCREENSHOT_BMP)
            const char* ext = "bmp";

            // Init BMP encoder
            BMPEncoder encoder(canvas->getFramebuffer(), canvas->width(), canvas->height());

            // Init buffer
            length = encoder.getLength();
            buffer = new uint8_t[length];
            std::unique_ptr<uint8_t[]> bufferPtr(buffer);

            // Encode & write BMP
            success = encoder.encode(buffer);
#elif defined(KEIRA_SCREENSHOT_PNG)
            const char* ext = "png";

            int16_t width = canvas->width();
            int16_t height = canvas->height();

            std::vector<uint8_t> image;
            image.resize(width * height * 4);
            for (int y = 0; y < height; ++y)
                for (int x = 0; x < width; ++x) {
                    const uint16_t* framebufferLine = canvas->getFramebuffer() + y * canvas->width();
                    uint16_t rgb565 = framebufferLine[x];
                    image[4 * width * y + 4 * x + 0] = (rgb565 >> 11 & 0x1F) * 255 / 31;
                    image[4 * width * y + 4 * x + 1] = (rgb565 >> 5 & 0x3F) * 255 / 63;
                    image[4 * width * y + 4 * x + 2] = (rgb565 & 0x1F) * 255 / 31;
                    image[4 * width * y + 4 * x + 3] = 255;
                }

            std::vector<uint8_t> pngData;
            success = !lodepng::encode(pngData, image, width, height);
            if (success) {
                length = pngData.size();
                buffer = new uint8_t[length];
                std::unique_ptr<uint8_t[]> bufferPtr(buffer);
                std::copy(pngData.begin(), pngData.end(), buffer);
            }
#else
#    error "Either KEIRA_SCREENSHOT_BMP or KEIRA_SCREENSHOT_PNG must be defined"
#endif
            // Write to file
            if (success) {
                // Generate filename
                struct tm time = ServiceManager::getInstance()->getService<ClockService>("clock")->getTime();
                char filename[64];
                snprintf(
                    filename,
                    sizeof(filename),
                    "/screenshots/screenshot_%04d%02d%02d_%02d%02d%02d.%s",
                    time.tm_year + 1900,
                    time.tm_mon + 1,
                    time.tm_mday,
                    time.tm_hour,
                    time.tm_min,
                    time.tm_sec,
                    ext
                );
                lilka::serial_log("Screenshot filename: %s", filename);

                File file = SD.open(filename, FILE_WRITE, true);
                if (file) {
                    file.write(buffer, length);
                    file.close();
                } else {
                    success = false;
                }
            }

            if (success) {
                AppManager::getInstance()->startToast("Скріншот збережено");
            } else {
                AppManager::getInstance()->startToast("Помилка збереження скріншоту");
            }
        } else {
            activated = false;
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

#include "resources.h"
#include "stdio.h"
#include "display.h"
#include "serial.h"
#include <contrib/LodePNG/lodepng.h>

#define RGB888_TO_RGB565(r, g, b) ((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3))

namespace lilka {

Image* Resources::loadImageBMP(FILE* file, int32_t transparentColor, int32_t pivotX, int32_t pivotY) {
    Image* image = 0;

    // Parse BMP image
    uint8_t fileheader[10];
    // Check file header
    if (fread(fileheader, 1, 10, file) == 10) {
        int dataOffset = fileheader[6] + (fileheader[7] << 8) + (fileheader[8] << 16) + (fileheader[9] << 24);
        uint8_t fileinfo[40];
        fread(fileinfo, 1, 40, file);
        // Get number of bits per pixel (offset from start of file: 0x1C)
        int bitsPerPixel = fileinfo[14] + (fileinfo[15] << 8);
        uint8_t bytesPerPixel = bitsPerPixel / 8;
        uint32_t width = fileinfo[4] + (fileinfo[5] << 8) + (fileinfo[6] << 16) + (fileinfo[7] << 24);
        uint32_t height = fileinfo[8] + (fileinfo[9] << 8) + (fileinfo[10] << 16) + (fileinfo[11] << 24);

        if (width <= 1024 && height <= 1024) {
            image = new Image(width, height, transparentColor, pivotX, pivotY);
            fseek(file, dataOffset, SEEK_SET);
            uint8_t row[width * bytesPerPixel];
            for (int y = height - 1; y >= 0; y--) {
                // Read row data
                fread(row, 1, width * bytesPerPixel, file);
                for (int x = 0; x < width; x++) {
                    const uint8_t* pixel = &row[x * bytesPerPixel];
                    uint8_t b = pixel[0];
                    uint8_t g = pixel[1];
                    uint8_t r = pixel[2];
                    uint16_t color = RGB888_TO_RGB565(r, g, b);
                    image->pixels[y * width + x] = color;
                }
            }
        } else {
            // Are you trying to load a 1 megapixel image? Are you insane? :)
            // serial_err("Image too large: %s (%d x %d)\n", filename.c_str(), width, height);
        }
    }
    return image;
}

Image* Resources::loadImagePNG(FILE* file, int32_t transparentColor, int32_t pivotX, int32_t pivotY) {
    Image* image = 0;

    fseek(file, 0, SEEK_SET);
    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    std::vector<uint8_t> fileBuffer(fileSize);
    fread(fileBuffer.data(), 1, fileSize, file);

    std::vector<uint8_t> imageBuffer;
    unsigned int width, height;
    if (!lodepng::decode(imageBuffer, width, height, fileBuffer)) {
        image = new Image(width, height, transparentColor, pivotX, pivotY);
        for (unsigned y = 0; y < height; ++y) {
            for (unsigned x = 0; x < width; ++x) {
                size_t index = 4 * (y * width + x);
                uint8_t r = imageBuffer[index];
                uint8_t g = imageBuffer[index + 1];
                uint8_t b = imageBuffer[index + 2];
                uint8_t a = imageBuffer[index + 3];
                image->pixels[y * width + x] = (a == 0) ? transparentColor : RGB888_TO_RGB565(r, g, b);
            }
        }
    }
    return image;
}

Image* Resources::loadImage(String filename, int32_t transparentColor, int32_t pivotX, int32_t pivotY) {
    Image* image = 0;
    FILE* file = fopen(filename.c_str(), "r");
    if (file) {
        // Check file signature
        uint8_t fileSignature[4];
        if (fread(fileSignature, 1, 4, file) == 4) {
            if (fileSignature[0] == 'B' && fileSignature[1] == 'M') {
                image = loadImageBMP(file, transparentColor, pivotX, pivotY);
            } else if (fileSignature[0] == 0x89 && fileSignature[1] == 'P' && fileSignature[2] == 'N' && fileSignature[3] == 'G') {
                image = loadImagePNG(file, transparentColor, pivotX, pivotY);
            } else {
                // serial_err("Invalid file type: %d %d %d %d\n", fileSignature[0], fileSignature[1], fileSignature[2], fileSignature[3]);
            }
        }
        fclose(file);
    } else {
        // serial_err("File not found: %s\n", filename.c_str());
    }
    return image;
}

int Resources::readFile(String filename, String& fileContent) {
    FILE* file = fopen(filename.c_str(), "r");
    if (!file) {
        // serial_err("File not found: %s\n", filename.c_str());
        return 1;
    }
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* buffer = new char[fileSize + 1];
    fread(buffer, 1, fileSize, file);
    buffer[fileSize] = 0;
    fileContent = String(buffer);
    delete[] buffer;
    fclose(file);
    return 0;
}

int Resources::writeFile(String filename, String fileContent) {
    FILE* file = fopen(filename.c_str(), "w");
    if (!file) {
        // serial_err("Failed to open file for writing: %s\n", filename.c_str());
        return 1;
    }
    fwrite(fileContent.c_str(), 1, fileContent.length(), file);
    fclose(file);
    return 0;
}

Resources resources;

} // namespace lilka

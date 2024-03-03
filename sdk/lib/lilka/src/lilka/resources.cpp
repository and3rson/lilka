#include "resources.h"
#include "stdio.h"
#include "display.h"
#include "serial.h"

namespace lilka {

Image* Resources::loadImage(String filename, int32_t transparentColor) {
    FILE* file = fopen(filename.c_str(), "r");
    if (!file) {
        // serial_err("File not found: %s\n", filename.c_str());
        return 0;
    }
    // Parse BMP image
    uint8_t fileheader[14];
    // Check file header
    if (fread(fileheader, 1, 14, file) != 14 || fileheader[0] != 'B' || fileheader[1] != 'M') {
        // serial_err("Invalid BMP file: %s\n", filename.c_str());
        fclose(file);
        return 0;
    }
    int dataOffset = fileheader[10] + (fileheader[11] << 8) + (fileheader[12] << 16) + (fileheader[13] << 24);
    uint8_t fileinfo[40];
    fread(fileinfo, 1, 40, file);
    // Get number of bits per pixel (offset from start of file: 0x1C)
    int bitsPerPixel = fileinfo[14] + (fileinfo[15] << 8);
    uint8_t bytesPerPixel = bitsPerPixel / 8;
    uint32_t width = fileinfo[4] + (fileinfo[5] << 8) + (fileinfo[6] << 16) + (fileinfo[7] << 24);
    uint32_t height = fileinfo[8] + (fileinfo[9] << 8) + (fileinfo[10] << 16) + (fileinfo[11] << 24);
    // int bitsPerPixel = fileinfo[14] + (fileinfo[15] << 8);
    // uint16_t *pixels = new uint16_t[width * height];

    if (width > 1024 || height > 1024) {
        // Are you trying to load a 1 megapixel image? Are you insane? :)
        serial_err("Image too large: %s (%d x %d)\n", filename.c_str(), width, height);
        fclose(file);
        return 0;
    }

    Image* image = new Image(width, height, transparentColor);
    fseek(file, dataOffset, SEEK_SET);
    uint8_t row[width * bytesPerPixel];
    for (int y = height - 1; y >= 0; y--) {
        // Read row data
        fread(row, 1, width * bytesPerPixel, file);
        for (int x = 0; x < width; x++) {
            uint8_t* pixel = &row[x * bytesPerPixel];
            uint8_t b = pixel[0];
            uint8_t g = pixel[1];
            uint8_t r = pixel[2];
            uint16_t color = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
            image->pixels[y * width + x] = color;
        }
    }
    fclose(file);

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

#include "resources.h"
#include "stdio.h"
#include "display.h"
#include "serial.h"

namespace lilka {

Bitmap* Resources::loadBitmap(String filename, int32_t transparentColor) {
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
    uint32_t width = fileinfo[4] + (fileinfo[5] << 8) + (fileinfo[6] << 16) + (fileinfo[7] << 24);
    uint32_t height = fileinfo[8] + (fileinfo[9] << 8) + (fileinfo[10] << 16) + (fileinfo[11] << 24);
    // int bitsPerPixel = fileinfo[14] + (fileinfo[15] << 8);
    // uint16_t *pixels = new uint16_t[width * height];

    if (width > 1024 || height > 1024) {
        // Are you trying to load a 1 megapixel image? Are you insane? :)
        // serial_err("Image too large: %s (%d x %d)\n", filename.c_str(), width, height);
        fclose(file);
        return 0;
    }

    Bitmap* bitmap = new Bitmap(width, height, transparentColor);
    fseek(file, dataOffset, SEEK_SET);
    for (int y = height - 1; y >= 0; y--) {
        for (int x = 0; x < width; x++) {
            uint32_t color;
            fread(&color, 1, 3, file);
            bitmap->pixels[x + y * width] = display.color565((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
        }
    }
    fclose(file);

    return bitmap;
}

Resources resources;

} // namespace lilka

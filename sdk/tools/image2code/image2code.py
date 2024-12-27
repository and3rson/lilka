#!/usr/bin/env python3
# Load image from file

import re
import sys

from PIL import Image

if len(sys.argv) < 2:
    print("Використання: image2code.py [--rle] зображення.png [зображення_2.png] ...")
    sys.exit(1)

print(f"Завантажуємо {sys.argv[1]}... ", end="")

# Pop flags from argv
i = 1
use_rle = False
while i < len(sys.argv):
    if sys.argv[i].startswith("-"):
        arg = sys.argv.pop(i)
        if arg == "--rle":
            use_rle = True
    else:
        i += 1

for fname in sys.argv[1:]:
    img = Image.open(fname)

    print(f"Розмір: {img.width}x{img.height}")

    out = fname.rpartition(".")[0] + ".h"
    var_name = re.sub(
        r"[^a-zA-Z0-9_]", "_", fname.rpartition(".")[0].rpartition("/")[2]
    )

    pixels = []

    # Iterate through each pixel
    for y in range(img.height):
        for x in range(img.width):
            # Get RGB values
            r, g, b, *_ = img.getpixel((x, y))

            # Convert to RGB-565
            r = r >> 3
            g = g >> 2
            b = b >> 3

            pixels.append((r << 11) | (g << 5) | b)

            # Print RGB values
            # print(r, g, b)

    if use_rle:
        # Encode data with RLE
        encoded = bytearray()
        i = 0
        while i < len(pixels):
            count = 1
            while (
                i + count < len(pixels)
                and count < 255
                and pixels[i] == pixels[i + count]
            ):
                count += 1
            encoded.append(count)
            encoded.append(pixels[i] & 0xFF)
            encoded.append(pixels[i] >> 8)
            i += count
        print(f"RLE: стиснуто {len(pixels) * 2} байтів до {len(encoded)} байтів")
        if len(encoded) > len(pixels * 2):
            print(
                "RLE: УВАГА! Стиснення не оптимальне! Вихідний файл буде більшим, ніж без стиснення!"
            )
        with open(out, "w") as f:
            print('Записуємо стиснені дані у "{}"... '.format(out), end="")
            print("// This is a generated file, do not edit.", file=f)
            print("// clang-format off", file=f)
            print(f"#include <stdint.h>", file=f)
            print(f"const uint32_t {var_name}_length = {len(encoded)};", file=f)
            print(f"const uint8_t {var_name}[] = {{", file=f)
            for byte in encoded:
                print(f"    0x{byte:02x},", file=f)
            print("};", file=f)
            print("// clang-format on", file=f)
            print("Зроблено!")

    else:
        with open(out, "w") as f:
            print(f"Записуємо {len(pixels)} пікселів у {out}... ", end="")
            print("// This is a generated file, do not edit.", file=f)
            print("// clang-format off", file=f)
            print(f"#include <stdint.h>", file=f)
            print(f"const uint16_t {var_name}_img_width = {img.width};", file=f)
            print(f"const uint16_t {var_name}_img_height = {img.height};", file=f)
            print(f"const uint16_t {var_name}_img[] = {{", file=f)
            for pixel in pixels:
                print(f"    0x{pixel:04x},", file=f)
            print("};", file=f)
            print("// clang-format on", file=f)
            print("Зроблено!")

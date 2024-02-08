#!/usr/bin/env python3
# Load image from file

import sys

from PIL import Image

if len(sys.argv) != 2:
    print("Використання: image2code.py зображення.png")
    sys.exit(1)

print(f"Завантажуємо {sys.argv[1]}... ", end="")

fname = sys.argv[1]
img = Image.open(fname)

print(f"Розмір: {img.width}x{img.height}")

out = fname.rpartition(".")[0] + ".h"
var_name = fname.rpartition(".")[0].rpartition("/")[2]

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

with open(out, "w") as f:
    print(f"Записуємо {len(pixels)} пікселів у {out}... ", end="")
    print("// This is a generated file, do not edit.", file=f)
    print("// clang-format off", file=f)
    print(f"#include <stdint.h>", file=f)
    print(f"const uint16_t {var_name}_width = {img.width};", file=f)
    print(f"const uint16_t {var_name}_height = {img.height};", file=f)
    print(f"const uint16_t {var_name}[] = {{", file=f)
    for pixel in pixels:
        print(f"    0x{pixel:04x},", file=f)
    print("};", file=f)
    print("// clang-format on", file=f)
    print("Зроблено!")

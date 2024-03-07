"""
This script removes some files from the libraries that Lilka uses, specifically:
  - GFX library code that is not used by the ST7789 display and ESP32 SPI interface
  - U8g2 display code (we only use the U8g2lib library for the font data)

This really speeds up the build process, but I'm not sure if it won't break in the future.
Але, як то кажуть - "це працює на моїй машині" ;)
"""
import os
import re
import shutil
import sys
from pathlib import Path
from itertools import chain

Import("env")

keep = ["Arduino_ESP32SPI", "Arduino_ST7789", "Arduino_HWSPI"]

try:
    lib_dir = Path(os.path.join(env.get("PROJECT_LIBDEPS_DIR"), env.get("PIOENV")))
    for lib in lib_dir.glob("*"):
        modified_count = 0
        deleted_count = 0
        if lib.name == "GFX Library for Arduino":
            # Remote all files in the GFX library that are not used by the ST7789 display and ESP32 SPI interface

            # Delete all files in the databus folder that don't match the pattern "Arduino_ESP32SPI.(c|h)"
            databus_dir = lib / "src" / "databus"
            for f in chain(databus_dir.glob("*.cpp"), databus_dir.glob("*.h")):
                if f.stem not in keep:
                    # print("Deleting {}".format(f.name))
                    f.unlink()
                    deleted_count += 1
            display_dir = lib / "src" / "display"
            for f in chain(display_dir.glob("*.cpp"), display_dir.glob("*.h")):
                if f.stem not in keep:
                    # print("Deleting {}".format(f.name))
                    f.unlink()
                    deleted_count += 1

            # Remove Arduino_GFX_Library.cpp because it includes some of the files we just deleted
            main_cpp = lib / "src" / "Arduino_GFX_Library.cpp"
            if main_cpp.exists():
                main_cpp.unlink()
                deleted_count += 1

            # Cleanup includes
            main_h = lib / "src" / "Arduino_GFX_Library.h"
            text = main_h.read_text()
            lines = text.splitlines()
            lines = [
                line
                for line in lines
                if not (
                    line.startswith("#include")
                    and ("databus/" in line or "display/" in line)
                    and not any([k + ".h" in line for k in keep])
                )
            ]
            new_text = "\n".join(lines)
            if text != new_text:
                main_h.write_text(new_text)
                modified_count += 1
        if lib.name == "U8g2":
            # Remove all display code
            clib_dir = lib / "src" / "clib"
            for f in clib_dir.glob("u8x8*.c"):
                # print("Deleting {}".format(f.name))
                f.unlink()
                deleted_count += 1
        if modified_count + deleted_count > 0:
            print(
                '  * Cleanup "{}": deleted {}, modified {}'.format(
                    lib.name, deleted_count, modified_count
                )
            )

except Exception as e:
    # Get exception line number
    tb = sys.exc_info()[2]
    print("  * Error in Lilka prepare.py script, line {}: {}".format(tb.tb_lineno, e))
    print("  * This is just a warning about failed cleanup. The build will continue.")

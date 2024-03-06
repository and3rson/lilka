# За замовчуванням PlatformIO кладе бінарний файл у директорію ".pio/build/<ENV>", але це не зручно.
# Цей скрипт копіює бінарний файл у поточну директорію та перейменовує його на "doom.bin".

import shutil
from pathlib import Path

Import("env")


def copy_firmware(source, target, env):
    try:
        src_path = Path(env.subst("$PROGPATH")).with_suffix(".bin")
        dest_path = Path(env.subst("$PROJECT_DIR")) / "doom.bin"

        print(f"Copying firmware from {src_path} to {dest_path}")
        shutil.copy(src_path, dest_path)
    except Exception as e:
        print(f"Failed to copy firmware: {e}")


env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", copy_firmware)

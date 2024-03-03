# За замовчуванням PlatformIO кладе бінарний файл у директорію ".pio/build/<ENV>", але це не зручно.
# Цей скрипт копіює бінарний файл у поточну директорію та перейменовує його на "doom.bin".

import shutil
from pathlib import Path

Import("env")


def move_firmware(source, target, env):
    src_path = Path(env.subst("$PROGPATH")).with_suffix(".bin")
    dest_path = Path(env.subst("$PROJECT_DIR")) / "doom.bin"

    print(f"Copying firmware from {src_path} to {dest_path}")
    shutil.copy(src_path, dest_path)


# Using checkprogsize instead of buildprog to ensure that the firmware is always copied
# https://community.platformio.org/t/post-action-not-running/18393/6?u=and3rson
env.AddPostAction("checkprogsize", move_firmware)

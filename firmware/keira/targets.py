import os
from pathlib import Path

Import("env")

def decode_backtrace(*args, **kwargs):
    args = env.get('PROGRAM_ARGS')
    args = list(filter(None, map(str.strip, ' '.join(args).split(' '))))

    if not args:
        print('No backtrace to decode.\nUsage:\n    pio run -e v2 -t decode_backtrace -a "0x11111111:0x22222222 0x33333333:0x44444444 ..."')
        return

    # Find bin directory of the toolchain
    bin_dir = Path(env.PioPlatform().get_package_dir("toolchain-riscv32-esp")) / "bin"

    # Find the addr2line binary
    if os.name == 'nt':
        addr2line = next(bin_dir.glob("*-addr2line.exe"), None)
    else:
        addr2line = next(bin_dir.glob("*-addr2line"), None)
    # Get .elf file
    src_path = Path(env.subst("$PROGPATH"))
    print('Decoding backtrace for', src_path)

    # Execute addr2line
    os.system(f"{addr2line} -e {src_path} {' '.join(args)}")

# Add the custom target to the environment
env.AddCustomTarget(
    "decode_backtrace", None, decode_backtrace, "Decode backtrace using addr2line",
)

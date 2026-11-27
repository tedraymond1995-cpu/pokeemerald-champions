#!/usr/bin/env python3
"""Pokémon Emerald Champions: writes the ready-made playtest save.

The save is a new game with the playtest setup applied (data/scripts/playtest.inc): all badges
and HMs, every trainer beaten, every town on the FLY map and a level 100 team, continuing in
front of PETALBURG CITY's Pokémon Center.

It runs the test "Playtest save" (test/playtest_save.c) in the test ROM with gPlaytestSaveDump
patched to TRUE. The test saves the game to flash and prints the flash contents, and this
script turns them into a 128 KiB .sav file that mGBA loads next to the ROM (same name, .sav).

Usage, from the repository root:
    make pokeemerald-test.elf -j$(nproc)
    python3 tools/make_playtest_save.py pokeemerald-test.elf emerald-champions.sav

The save only fits ROMs built from the same save layout (test/save.c checks the sizes). After a
change to the save blocks, build a new one, or use the debug menu's Utilities > Playtest setup.
"""

import os
import re
import shutil
import subprocess
import sys
import tempfile

FLASH_SIZE = 128 * 1024
SECTOR_SIZE = 4096
TEST_NAME = "Playtest save"


def main():
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    test_elf, out_path = sys.argv[1], sys.argv[2]
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    patchelf = os.path.join(root, "tools", "patchelf", "patchelf")
    rom_test = os.path.join(root, "tools", "mgba", "mgba-rom-test")

    with tempfile.TemporaryDirectory() as tmp:
        elf = os.path.join(tmp, "playtest.elf")
        shutil.copyfile(test_elf, elf)
        subprocess.run([patchelf, elf,
                        "gTestRunnerArgv", TEST_NAME + "\\0",
                        "gTestRunnerHeadless", "\\x01",
                        "gTestRunnerSkipIsFail", "\\x00",
                        "gTestRunnerN", "\\x01",
                        "gTestRunnerI", "\\x00",
                        "gPlaytestSaveDump", "\\x01"], check=True)
        run = subprocess.run([rom_test, "-l15", "-Rr0", elf],
                             stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, errors="replace")

    flash = bytearray(b"\xff" * FLASH_SIZE)
    lines = 0
    ended = passed = failed = False
    current_test = None
    for line in run.stdout.splitlines():
        if "GBA Debug:" not in line:
            continue
        message = line.split("GBA Debug: ", 1)[1]
        if message.startswith(":N"):
            current_test = message[2:]
        elif message.startswith(":P") and current_test and current_test.startswith(TEST_NAME):
            passed = True
        elif message.startswith(":F") and current_test and current_test.startswith(TEST_NAME):
            failed = True
        elif message == "PLAYTEST_SAVE_END":
            ended = True
        else:
            match = re.fullmatch(r"PLAYTEST_SAVE (\d+) (\d+) ([0-9a-f]+)", message)
            if match:
                start = int(match.group(1)) * SECTOR_SIZE + int(match.group(2))
                data = bytes.fromhex(match.group(3))
                flash[start:start + len(data)] = data
                lines += 1

    if failed or not passed or not ended or lines == 0:
        sys.stdout.write("\n".join(l for l in run.stdout.splitlines() if "GBA DMA" not in l)[-4000:] + "\n")
        sys.exit(f"The {TEST_NAME} test didn't write a save (passed={passed}, failed={failed}, "
                 f"finished={ended}, lines={lines}).")

    with open(out_path, "wb") as f:
        f.write(flash)
    print(f"Wrote {out_path} ({lines} flash lines from the test).")


if __name__ == "__main__":
    main()

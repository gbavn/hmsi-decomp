"""Build Sunshine Islands (USA) from the dsd config and compare it to the original ROM.

Usage:  python scripts/build.py
"""
import hashlib
import os
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
DSD = ROOT / "tools" / "dsd.exe"
MW = ROOT / "tools" / "mwccarm" / "2.0" / "sp2p2"
CONFIG = ROOT / "config" / "usa" / "arm9" / "config.yaml"
BUILD = ROOT / "build" / "usa"
BASE_ROM = ROOT / "roms" / "sunshine_islands_usa.nds"
OUT_ROM = BUILD / "sunshine_islands_usa.nds"

# Header bytes that depend on the ARM7 BIOS (secure area CRC) and the header CRC.
BIOS_DEPENDENT = {0x6C, 0x6D, 0x15E, 0x15F}

env = dict(os.environ, LM_LICENSE_FILE=str(ROOT / "tools" / "mwccarm" / "license.dat"))


def run(*args):
    print(">", " ".join(str(a) for a in args), flush=True)
    subprocess.run([str(a) for a in args], cwd=ROOT, env=env, check=True)


def main():
    BUILD.mkdir(parents=True, exist_ok=True)
    run(DSD, "delink", "-c", CONFIG)
    run(DSD, "lcf", "-c", CONFIG)
    run(MW / "mwldarm.exe", "-proc", "arm946e", "-nostdlib", "-interworking", "-nodead",
        "-m", "Entry", "-map", "closure,unused", "-o", BUILD / "arm9.o",
        BUILD / "arm9.lcf", "@" + str(BUILD / "objects.txt"))
    run(DSD, "check", "modules", "-c", CONFIG, "--fail")
    run(DSD, "rom", "config", "-e", BUILD / "arm9.o", "-c", CONFIG)
    run(DSD, "rom", "build", "-c", BUILD / "build" / "rom_config.yaml", "-o", OUT_ROM)

    base, built = BASE_ROM.read_bytes(), OUT_ROM.read_bytes()
    print("base  sha256:", hashlib.sha256(base).hexdigest())
    print("built sha256:", hashlib.sha256(built).hexdigest())
    if base == built:
        print("MATCH: ROM is identical.")
        return 0
    if len(base) != len(built):
        print(f"MISMATCH: size differs ({len(base)} vs {len(built)})")
        return 1
    diffs = [i for i in range(len(base)) if base[i] != built[i]]
    if set(diffs) <= BIOS_DEPENDENT:
        print("MATCH (except secure-area/header CRC; provide ARM7 BIOS for a full match).")
        return 0
    print(f"MISMATCH: {len(diffs)} bytes differ, first at 0x{diffs[0]:X}")
    return 1


if __name__ == "__main__":
    sys.exit(main())

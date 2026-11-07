"""Build Sunshine Islands (USA) from the dsd config and compare it to the original ROM.

Usage:  python scripts/build.py [--quick]
  --quick  stop after `dsd check modules` (code/data verified, ROM not rebuilt)
"""
import argparse
import hashlib
import json
import os
import re
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from flags import compiler_for, flags_for  # noqa: E402

ROOT = Path(__file__).resolve().parents[1]
DSD = ROOT / "tools" / "dsd.exe"
MWROOT = ROOT / "tools" / "mwccarm"
MW = MWROOT / "2.0" / "sp2p2"  # linker
CONFIG_DIR = ROOT / "config" / "usa" / "arm9"
CONFIG = CONFIG_DIR / "config.yaml"
BUILD = ROOT / "build" / "usa"
BASE_ROM = ROOT / "roms" / "sunshine_islands_usa.nds"
OUT_ROM = BUILD / "sunshine_islands_usa.nds"

# Header bytes that depend on the ARM7 BIOS (secure area CRC) and the header CRC.
BIOS_DEPENDENT = {0x6C, 0x6D, 0x15E, 0x15F}

env = dict(os.environ, LM_LICENSE_FILE=str(ROOT / "tools" / "mwccarm" / "license.dat"))


def run(*args):
    print(">", " ".join(str(a) for a in args), flush=True)
    subprocess.run([str(a) for a in args], cwd=ROOT, env=env, check=True)


def tree_stamp(*dirs):
    """Cheap fingerprint of every header under the given directories."""
    h = hashlib.sha1()
    for d in dirs:
        for p in sorted((ROOT / d).rglob("*.h")):
            st = p.stat()
            h.update(f"{p}:{st.st_mtime_ns}:{st.st_size}".encode())
    return h.hexdigest()


def compile_unit(src, obj, header_stamp):
    flags = flags_for(src)
    compiler = compiler_for(src)
    key = hashlib.sha1(("\0".join([compiler, *flags]) + header_stamp).encode() + (ROOT / src).read_bytes()).hexdigest()
    stamp = obj.with_suffix(".key")
    if obj.exists() and stamp.exists() and stamp.read_text() == key:
        return None
    obj.parent.mkdir(parents=True, exist_ok=True)
    r = subprocess.run([str(MWROOT / compiler / "mwccarm.exe"), *flags, "-c", str(src), "-o", str(obj)], cwd=ROOT, env=env,
                       capture_output=True, text=True)
    if r.returncode != 0:
        return f"{src}:\n{r.stdout}{r.stderr}"
    stamp.write_text(key)
    return None


def ensure_gap_symbols():
    """Make sure every data gap between complete units starts with a symbol, so it can be kept alive."""
    for delinks in CONFIG_DIR.rglob("delinks.txt"):
        header, _, files = delinks.read_text().partition("\n\n")
        module = {}
        for m in re.finditer(r"(\.(?:rodata|data|bss))\s+start:0x([0-9a-f]+) end:0x([0-9a-f]+)", header):
            module[m.group(1)] = (int(m.group(2), 16), int(m.group(3), 16))
        units = {k: [] for k in module}
        for block in re.split(r"\n\s*\n", files):
            if re.search(r"^\s+complete\s*$", block, re.M):
                for m in re.finditer(r"(\.(?:rodata|data|bss))\s+start:0x([0-9a-f]+) end:0x([0-9a-f]+)", block):
                    if m.group(1) in units:
                        units[m.group(1)].append((int(m.group(2), 16), int(m.group(3), 16)))
        starts = []
        for sec, (lo, hi) in module.items():
            cur = lo
            for s0, e0 in sorted(units[sec]):
                if s0 > cur:
                    starts.append((sec, cur))
                cur = max(cur, e0)
            if cur < hi:
                starts.append((sec, cur))
        syms = delinks.with_name("symbols.txt")
        lines = syms.read_text().splitlines()
        have = set()
        for line in lines:
            m = re.match(r"^\S+ kind:\S+ addr:0x([0-9a-f]+)", line)
            if m:
                have.add(int(m.group(1), 16))
        added = [f"data_{a:08x} kind:{'bss' if sec == '.bss' else 'data(any)'} addr:0x{a:08x}"
                 for sec, a in starts if a not in have]
        if added:
            syms.write_text("\n".join(lines + added) + "\n")
            subprocess.run([str(DSD), "format", "-c", str(CONFIG)], cwd=ROOT, capture_output=True)


def gap_data_symbols():
    """Data symbols that live in assembly gaps (not inside a complete unit's data)."""
    names = []
    for delinks in CONFIG_DIR.rglob("delinks.txt"):
        covered = []
        _, _, files = delinks.read_text().partition("\n\n")
        for block in re.split(r"\n\s*\n", files):
            if re.search(r"^\s+complete\s*$", block, re.M):
                for m in re.finditer(r"\.(?:rodata|data|bss|sbss|sdata)\s+start:0x([0-9a-f]+) end:0x([0-9a-f]+)",
                                     block):
                    covered.append((int(m.group(1), 16), int(m.group(2), 16)))
        for line in delinks.with_name("symbols.txt").read_text().splitlines():
            m = re.match(r"^(\S+) kind:(data|bss)\S* addr:0x([0-9a-f]+)(.*)$", line)
            if not m or "local" in m.group(4) or "ambiguous" in m.group(4):
                continue
            addr = int(m.group(3), 16)
            if not re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", m.group(1)):
                continue  # not expressible in the LCF; such symbols are referenced anyway
            if not any(s <= addr < e for s, e in covered):
                names.append(m.group(1))
    return names


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--quick", action="store_true")
    a = ap.parse_args()

    BUILD.mkdir(parents=True, exist_ok=True)
    ensure_gap_symbols()
    run(DSD, "delink", "-c", CONFIG)
    run(DSD, "lcf", "-c", CONFIG)

    # We link with -dead like the original, so unused library code and data are stripped.
    # Data that only exists in assembly gaps must survive: force it active.
    lcf = BUILD / "arm9.lcf"
    text = lcf.read_text()
    force = "FORCE_ACTIVE {\n    " + ",\n    ".join(gap_data_symbols()) + "\n}\n\n"
    text = text.replace("SECTIONS {", force + "SECTIONS {", 1)
    # Constants that Nintendo's LCF defined for the SDK.
    consts = []
    sdk_file = CONFIG_DIR / "sdk_symbols.txt"
    if sdk_file.exists():
        for line in sdk_file.read_text().splitlines():
            m = re.match(r"^\s*(\w+)\s*=\s*(0x[0-9a-fA-F]+)", line)
            if m:
                consts.append(f"    {m.group(1)} = {m.group(2)};\n")
    text = text.replace("SECTIONS {\n", "SECTIONS {\n" + "".join(consts), 1)
    # ITCM units named <file>_itcm.c are aliases: link <file>.o(.itcm) in their place.
    for alias in re.findall(r"([\w.]+)_itcm\.o\(\.text\)", text):
        text = text.replace(alias + "_itcm.o(.text)", alias + ".o(.itcm)")
    # DTCM units named <file>_dtcm.c likewise: <file>.o(.dtcm) for data, <file>.o(.dtcm.bss) for bss.
    text = re.sub(r"([\w.]+)_dtcm\.o\(\.data\)", r"\1.o(.dtcm)", text)
    text = re.sub(r"([\w.]+)_dtcm\.o\(\.bss\)", r"\1.o(.dtcm.bss)", text)
    text = re.sub(r"\s*[\w.]+_dtcm\.o\(\.\w+\)", "", text)  # other (empty) sections of the alias
    lcf.write_text(text)
    objects = BUILD / "objects.txt"
    objects.write_text("".join(l for l in objects.read_text().splitlines(True)
                               if "_itcm.o" not in l and "_dtcm.o" not in l))

    files = json.loads(subprocess.run([str(DSD), "json", "delinks", "-c", str(CONFIG)], cwd=ROOT,
                                      capture_output=True, text=True, check=True).stdout)["files"]
    jobs = sorted({(Path(f["name"]), ROOT / f["object_to_link"]) for f in files
                   if f["object_to_link"] != f["delink_file"] and (ROOT / f["name"]).exists()})
    stamp = tree_stamp("include", "libs")
    with ThreadPoolExecutor(max_workers=os.cpu_count() or 4) as pool:
        errors = [e for e in pool.map(lambda j: compile_unit(j[0], j[1], stamp), jobs) if e]
    if errors:
        print("\n".join(errors))
        sys.exit("compile failed")
    print(f"compiled {len(jobs)} units", flush=True)

    run(MW / "mwldarm.exe", "-proc", "arm946e", "-nostdlib", "-interworking", "-dead",
        "-m", "Entry", "-map", "closure,unused", "-o", BUILD / "arm9.o",
        BUILD / "arm9.lcf", "@" + str(BUILD / "objects.txt"))
    run(DSD, "check", "modules", "-c", CONFIG, "--fail")
    if a.quick:
        print("MATCH (modules)")
        return 0
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

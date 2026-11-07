"""Show how a library file differs from the game's version.

Lists, in address order, the original functions around the file's matched region
(matched ones with their name, unmatched ones with size) and the compiled
functions that found no match.

Usage:  python scripts/libdiff.py <source> [--module main] [--window 0x200]
"""
import argparse
import re
import subprocess
import sys
import tempfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import addlib  # noqa: E402
import match  # noqa: E402


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("source")
    ap.add_argument("--module", default="main")
    ap.add_argument("--window", default="0x200")
    a = ap.parse_args()
    out = subprocess.run([sys.executable, str(Path(__file__).parent / "addlib.py"), a.source, "--module", a.module,
                          "--dry-run"], capture_output=True, text=True).stdout
    matched = {}
    for line in out.splitlines():
        m = re.match(r"\s+0x([0-9a-f]+) (\S+)\s+-> (\S+)", line)
        if m:
            matched[int(m.group(1), 16)] = m.group(3)
    for line in out.splitlines():
        if line.startswith("warning"):
            print(line)
    with tempfile.TemporaryDirectory() as tmp:
        obj = Path(tmp) / "o.o"
        match.compile_src(match.ROOT / a.source, match.DEFAULT_COMPILER, None, obj)
        b, secs, syms, relocs = addlib.read_obj(obj)
    compiled = [(s["shndx"], s["name"], s["size"]) for s in syms
                if s["type"] == 2 and s["size"] and 0 < s["shndx"] < len(secs)
                and secs[s["shndx"]]["name"].startswith(".text")]
    _, ofuncs, _, _, _ = addlib.load_module(a.module)
    if not matched:
        print("nothing matched")
        return
    lo, hi = min(matched) - int(a.window, 16), max(matched) + int(a.window, 16)
    print("original (game) functions:")
    for addr, size, name in ofuncs:
        if lo <= addr <= hi:
            tag = f"= {matched[addr]}" if addr in matched else ("" if not name.startswith("func_") else "  <-- unmatched")
            print(f"  {addr:#010x} {size:#06x} {name:36} {tag}")
    print("compiled (reference) functions without a match, in source order:")
    names = set(matched.values())
    for shndx, name, size in sorted(compiled):
        if re.sub(r"\$.*", "", name) not in names:
            print(f"  {name:40} {size:#06x}")


if __name__ == "__main__":
    main()

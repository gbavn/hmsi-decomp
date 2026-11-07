"""Compile a source file and find which of its functions match original functions in an address range.

Usage:  python scripts/autoident.py <source> <start> <end> [--module main] [--rename]
Prints a mapping; with --rename it applies the names to symbols.txt (static functions become local).
"""
import argparse
import re
import subprocess
import sys
import tempfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import match  # noqa: E402


def compiled_functions(obj):
    b, secs, syms, relocs = match.read_elf(obj)
    out = {}
    for s in syms:
        if not (0 < s["shndx"] < len(secs)) or not s["size"]:
            continue
        sec = secs[s["shndx"]]
        if not sec["name"].startswith(".text"):
            continue
        start = s["value"] & ~1
        data = bytearray(b[sec["off"] + start: sec["off"] + start + s["size"]])
        masked = set()
        for off in relocs.get(s["shndx"], ()):
            if start <= off < start + s["size"]:
                masked.update(range(off - start, off - start + 4))
        out[s["name"]] = (data, masked)
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("source")
    ap.add_argument("start")
    ap.add_argument("end")
    ap.add_argument("--module", default="main")
    ap.add_argument("--rename", action="store_true")
    a = ap.parse_args()
    start, end = int(a.start, 16), int(a.end, 16)

    syms_file = match.module_info(a.module)[0]
    originals = []
    for line in syms_file.read_text().splitlines():
        m = match.SYM_RE.match(line)
        if m and start <= int(m.group(4), 16) < end:
            originals.append(m.group(1))

    with tempfile.TemporaryDirectory() as tmp:
        obj = Path(tmp) / "out.o"
        match.compile_src(match.ROOT / a.source, match.DEFAULT_COMPILER, None, obj)
        comp = compiled_functions(obj)
        b, secs, syms, _ = match.read_elf(obj)
        local = {s["name"] for s in syms if s["size"]}  # refined below

    pairs = {}
    unmatched = []
    for name in originals:
        mode, addr, orig, omask = match.original_bytes(name, a.module)
        found = None
        for cname, (data, cmask) in comp.items():
            if len(data) != len(orig):
                continue
            if not match.compare(orig, omask, data, cmask):
                found = cname
                break
        if found:
            pairs[name] = found
            print(f"{addr:#010x} {name:32} -> {found}")
        else:
            unmatched.append(name)
            print(f"{addr:#010x} {name:32} -> ??? (size {len(orig):#x})")
    print(f"matched {len(pairs)}/{len(originals)}")

    if a.rename and pairs:
        args = []
        for k, v in pairs.items():
            if k != v:
                args += [k, re.sub(r"\$.*", "", v)]
        if args:
            subprocess.run([sys.executable, str(Path(__file__).parent / "rename.py"), *args], check=True)


if __name__ == "__main__":
    main()

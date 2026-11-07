"""Fold the dsd-generated symbols inside a data object into the object itself.

dsd names every address it sees accessed (data_020ec7fc, ...). Once the object is defined in C
(e.g. a struct `gInput` of 0x58 bytes), those inner symbols no longer exist in any object file,
so references to them must point at `<object> + offset` instead.

Usage:  python scripts/mergedata.py <symbol> <size>
Removes the symbols inside (addr, addr+size) from the module's symbols.txt and rewrites every
relocation (all modules) that targets one of them as `to:<addr> add:<offset>`.
"""
import re
import sys
from pathlib import Path

CONFIG = Path(__file__).resolve().parents[1] / "config" / "usa" / "arm9"


def main():
    name, size = sys.argv[1], int(sys.argv[2], 0)
    base = None
    for syms in CONFIG.rglob("symbols.txt"):
        for line in syms.read_text().splitlines():
            m = re.match(rf"^{re.escape(name)} kind:\S+ addr:0x([0-9a-f]+)", line)
            if m:
                base, home = int(m.group(1), 16), syms
    if base is None:
        sys.exit(f"{name} not found")
    keep, dropped = [], 0
    for line in home.read_text().splitlines():
        m = re.match(r"^\S+ kind:\S+ addr:0x([0-9a-f]+)", line)
        if m and base < int(m.group(1), 16) < base + size:
            dropped += 1
            continue
        keep.append(line)
    home.write_text("\n".join(keep) + "\n")
    # Overlays share addresses: only rewrite references that target the symbol's own module.
    rel = home.parent.relative_to(CONFIG).parts
    if not rel:
        module = "main"
    elif rel[0] == "overlays":
        module = f"overlay({int(rel[1][2:])})"
    else:
        module = rel[0]
    fixed = 0
    for relocs in CONFIG.rglob("relocs.txt"):
        text = relocs.read_text()
        lines = text.splitlines()
        out = []
        changed = False
        for line in lines:
            m = re.match(r"^(from:\S+ kind:\S+ )to:0x([0-9a-f]+)( add:\S+)?( module:(\S+).*)$", line)
            if (m and m.group(5) == module and base < int(m.group(2), 16) < base + size
                    and not m.group(3)):
                off = int(m.group(2), 16) - base
                line = f"{m.group(1)}to:{base:#010x} add:{off:#x}{m.group(4)}"
                fixed += 1
                changed = True
            out.append(line)
        if changed:
            relocs.write_text("\n".join(out) + "\n")
    print(f"{name}: dropped {dropped} inner symbols, rewrote {fixed} relocations")


if __name__ == "__main__":
    main()

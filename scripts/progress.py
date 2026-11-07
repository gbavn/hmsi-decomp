"""Report decompilation progress: code bytes and functions inside `complete` units.

Usage:  python scripts/progress.py
"""
import re
from pathlib import Path

CONFIG = Path(__file__).resolve().parents[1] / "config" / "usa" / "arm9"
SYM_RE = re.compile(r"^\S+ kind:function\((?:arm|thumb),size=0x([0-9a-f]+)\) addr:0x([0-9a-f]+)")


def parse_delinks(path):
    total, done = [], []
    text = path.read_text()
    header, _, files = text.partition("\n\n")
    for m in re.finditer(r"\.(?:text|init)\s+start:0x([0-9a-f]+) end:0x([0-9a-f]+)", header):
        total.append((int(m.group(1), 16), int(m.group(2), 16)))
    for block in re.split(r"\n\s*\n", files):
        if re.search(r"^\s+complete\s*$", block, re.M):
            for m in re.finditer(r"\.(?:text|init)\s+start:0x([0-9a-f]+) end:0x([0-9a-f]+)", block):
                done.append((int(m.group(1), 16), int(m.group(2), 16)))
    return total, done


def main():
    grand = [0, 0, 0, 0]
    rows = []
    for delinks in sorted(CONFIG.rglob("delinks.txt")):
        total, done = parse_delinks(delinks)
        tsize = sum(e - s for s, e in total)
        dsize = sum(e - s for s, e in done)
        nf = nd = 0
        syms = delinks.with_name("symbols.txt")
        if syms.exists():
            for line in syms.read_text().splitlines():
                m = SYM_RE.match(line)
                if m:
                    addr = int(m.group(2), 16)
                    nf += 1
                    nd += any(s <= addr < e for s, e in done)
        name = delinks.parent.name if delinks.parent != CONFIG else "main"
        if tsize:
            rows.append((name, dsize, tsize, nd, nf))
        for i, v in enumerate((dsize, tsize, nd, nf)):
            grand[i] += v
    for name, d, t, nd, nf in rows:
        print(f"{name:8} {d / t * 100:6.2f}%  code {d:#x}/{t:#x}  functions {nd}/{nf}")
    d, t, nd, nf = grand
    print(f"{'TOTAL':8} {d / t * 100:6.2f}%  code {d:#x}/{t:#x}  functions {nd}/{nf}")


if __name__ == "__main__":
    main()

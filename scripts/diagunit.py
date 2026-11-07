"""After a failed `build.py --quick`, show where a unit's linked layout departs from the original.

Usage:  python scripts/diagunit.py <object basename, e.g. glbstate.o>
Reads build/usa/arm9.o.xMAP and config/usa/arm9/symbols.txt.
"""
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
obj = sys.argv[1]
orig = {}
for f in (ROOT / "config" / "usa" / "arm9").rglob("symbols.txt"):
    for line in f.read_text().splitlines():
        m = re.match(r"^(\S+) kind:(\S+) addr:0x([0-9a-f]+)", line)
        if m:
            size = re.search(r"size=0x([0-9a-f]+)", m.group(2))
            orig.setdefault(m.group(1), (int(m.group(3), 16), int(size.group(1), 16) if size else None))
shown = 0
for line in (ROOT / "build" / "usa" / "arm9.o.xMAP").read_text(errors="replace").splitlines():
    m = re.match(r"^\s+([0-9A-F]{8}) ([0-9A-F]{8}) (\.\w+)\s+(\S+)\s+\((.+)\)$", line)
    if not m or m.group(5) != obj or m.group(4).startswith("."):
        continue
    addr, size, sec, name = int(m.group(1), 16), int(m.group(2), 16), m.group(3), m.group(4)
    name = re.sub(r"\$.*", "", name)
    o = orig.get(name)
    status = "ok"
    if o is None:
        status = "NOT IN ORIGINAL (extra function/data kept by the linker)"
    elif o[0] != addr or (o[1] is not None and o[1] != size):
        status = f"orig at {o[0]:#010x} size {o[1]:#x}" if o[1] else f"orig at {o[0]:#010x}"
    if status != "ok" or shown < 3:
        print(f"{addr:#010x} {size:#06x} {sec:8} {name:40} {status}")
        shown += 1

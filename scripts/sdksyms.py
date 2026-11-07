"""Find the values of SDK linker-script symbols (SDK_*) a library file uses, from the original ROM.

Usage:  python scripts/sdksyms.py <source>   -> prints NAME = 0xVALUE lines
"""
import re
import subprocess
import struct
import sys
import tempfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import addlib  # noqa: E402
import match  # noqa: E402

src = sys.argv[1]
out = subprocess.run([sys.executable, str(Path(__file__).parent / "addlib.py"), src, "--dry-run"],
                     capture_output=True, text=True).stdout
pairs = {}
for line in out.splitlines():
    m = re.match(r"\s+0x([0-9a-f]+) (\S+)\s+-> (\S+)", line)
    if m:
        pairs[m.group(3)] = int(m.group(1), 16)
with tempfile.TemporaryDirectory() as t:
    o = Path(t) / "o.o"
    match.compile_src(match.ROOT / src, match.DEFAULT_COMPILER, None, o)
    b, secs, syms, relocs = addlib.read_obj(o)
_, _, _, obin, obase = addlib.load_module("main")
vals = {}
for s in syms:
    name = re.sub(r"\$.*", "", s["name"])
    if name not in pairs or s["type"] != 2:
        continue
    start = s["value"] & ~1
    for off, si, rt, add in relocs.get(s["shndx"], ()):
        t = syms[si]["name"]
        if t.startswith("SDK_") and rt == 2:
            a = pairs[name] + off - start
            word, = struct.unpack_from("<I", obin, a - obase)
            vals.setdefault(t, set()).add(word - add)
for k, v in sorted(vals.items()):
    print(f"{k} = {' / '.join(hex(x) for x in sorted(v))}")

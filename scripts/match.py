"""Compile a C/C++ file and compare one function against the original game code.

Usage:
    python scripts/match.py <source> <function> [--module main|itcm|ov002...]
                            [--compiler 2.0/sp2p2] [--flags "..."] [--all-compilers]

Relocated words (calls, literal pool addresses) are masked on both sides,
so only the instruction encoding is compared.
"""
import argparse
import os
import re
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from flags import flags_for  # noqa: E402

ROOT = Path(__file__).resolve().parents[1]
CONFIG = ROOT / "config" / "usa" / "arm9"
EXTRACT = ROOT / "extract" / "usa"
MWROOT = ROOT / "tools" / "mwccarm"
DEFAULT_COMPILER = "2.0/sp2p2"
DEFAULT_FLAGS = ("-O4,p -enum int -char signed -str noreuse -proc arm946e -gccext,on -fp soft "
                 "-inline noauto -Cpp_exceptions off -RTTI off -interworking -w off -sym on "
                 "-gccinc -nolink -msgstyle gcc -i include")

SYM_RE = re.compile(r"^(\S+) kind:function\((arm|thumb),size=0x([0-9a-f]+)\) addr:0x([0-9a-f]+)")


def module_info(module):
    """Return (symbols.txt, relocs.txt, binary path, base address)."""
    if module == "main":
        return CONFIG / "symbols.txt", CONFIG / "relocs.txt", EXTRACT / "arm9" / "arm9.bin", 0x02000000
    if module in ("itcm", "dtcm"):
        base = int(re.search(r"base_address: (\d+)", (EXTRACT / "arm9" / f"{module}.yaml").read_text()).group(1))
        return CONFIG / module / "symbols.txt", CONFIG / module / "relocs.txt", EXTRACT / "arm9" / f"{module}.bin", base
    m = re.fullmatch(r"ov(\d{3})", module)
    if not m:
        raise SystemExit(f"unknown module {module}")
    text = (EXTRACT / "arm9_overlays" / "overlays.yaml").read_text()
    for block in text.split("- id: ")[1:]:
        if int(block.split()[0]) == int(m.group(1)):
            base = int(re.search(r"base_address: (\d+)", block).group(1))
            d = CONFIG / "overlays" / module
            return d / "symbols.txt", d / "relocs.txt", EXTRACT / "arm9_overlays" / f"{module}.bin", base
    raise SystemExit(f"overlay {module} not found")


def find_module(func):
    mods = ["main", "itcm", "dtcm"] + sorted(p.name for p in (CONFIG / "overlays").iterdir())
    for mod in mods:
        syms = module_info(mod)[0]
        if syms.exists() and re.search(rf"^{re.escape(func)} ", syms.read_text(), re.M):
            return mod
    raise SystemExit(f"function {func} not found in any symbols.txt")


def original_bytes(func, module, length=None, at=None):
    syms, relocs, binary, base = module_info(module)
    for line in syms.read_text().splitlines():
        m = SYM_RE.match(line)
        if m and m.group(1) == func and (at is None or int(m.group(4), 16) == at):
            mode, size, addr = m.group(2), int(m.group(3), 16), int(m.group(4), 16)
            break
    else:
        raise SystemExit(f"{func} not in {syms}")
    if length and length > size:
        size = length  # include a literal pool that dsd didn't attribute to the function
    data = bytearray(binary.read_bytes()[addr - base: addr - base + size])
    masked = set()
    for line in relocs.read_text().splitlines():
        r = re.match(r"from:0x([0-9a-f]+)", line)
        if r:
            off = int(r.group(1), 16) - addr
            if 0 <= off < size:
                masked.update(range(off, off + 4))
    return mode, addr, data, masked


# ---- minimal ELF32 little-endian reader -------------------------------------------------------
def read_elf(path):
    b = path.read_bytes()
    shoff, = struct.unpack_from("<I", b, 0x20)
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", b, 0x2E)
    secs = []
    for i in range(shnum):
        name, typ, flags, addr, off, size, link, info, align, entsize = struct.unpack_from(
            "<10I", b, shoff + i * shentsize)
        secs.append(dict(name=name, type=typ, off=off, size=size, link=link, info=info, entsize=entsize))
    shstr = secs[shstrndx]

    def cstr(tab, idx):
        start = tab["off"] + idx
        return b[start:b.index(b"\0", start)].decode()

    for s in secs:
        s["name"] = cstr(shstr, s["name"])
    symtab = next(s for s in secs if s["type"] == 2)
    strtab = secs[symtab["link"]]
    syms = []
    for i in range(symtab["size"] // 16):
        name, value, size, info, other, shndx = struct.unpack_from("<IIIBBH", b, symtab["off"] + i * 16)
        syms.append(dict(name=cstr(strtab, name), value=value, size=size, shndx=shndx, type=info & 0xF))
    relocs = {}
    for s in secs:
        if s["type"] in (9, 4):  # REL, RELA
            ent = 8 if s["type"] == 9 else 12
            target = s["info"]
            for i in range(s["size"] // ent):
                off, = struct.unpack_from("<I", b, s["off"] + i * ent)
                relocs.setdefault(target, set()).add(off)
    return b, secs, syms, relocs


def compiled_bytes(obj, func):
    b, secs, syms, relocs = read_elf(obj)
    cands = [s for s in syms if s["name"] == func or s["name"].startswith(func + "__")
             or re.fullmatch(rf"{re.escape(func)}(\$.*)?", s["name"])]
    cands = [s for s in cands if 0 < s["shndx"] < len(secs) and s["size"]]
    if not cands:
        names = [s["name"] for s in syms if s["size"]]
        raise SystemExit(f"{func} not found in object; symbols: {names}")
    s = cands[0]
    sec = secs[s["shndx"]]
    start = s["value"] & ~1
    # a function alone in its section owns the trailing literal pool too
    nxt = min([o["value"] & ~1 for o in syms if o["shndx"] == s["shndx"] and o["type"] == 2
               and (o["value"] & ~1) > start] + [sec["size"]])
    size = nxt - start if nxt - start > s["size"] else s["size"]
    data = bytearray(b[sec["off"] + start: sec["off"] + start + size])
    masked = set()
    for off in relocs.get(s["shndx"], ()):
        if start <= off < start + size:
            masked.update(range(off - start, off - start + 4))
    return data, masked


def compile_src(src, compiler, flags, out):
    env = dict(os.environ, LM_LICENSE_FILE=str(MWROOT / "license.dat"))
    flag_list = flags.split() if flags else flags_for(Path(src).relative_to(ROOT) if Path(src).is_absolute() else src)
    cmd = [str(MWROOT / compiler / "mwccarm.exe"), *flag_list, "-c", str(src), "-o", str(out)]
    r = subprocess.run(cmd, cwd=ROOT, env=env, capture_output=True, text=True, timeout=120)
    if r.returncode != 0:
        print(r.stdout + r.stderr)
        raise SystemExit(f"compile failed ({compiler})")


def compare(orig, omask, comp, cmask):
    n = max(len(orig), len(comp))
    bad = []
    for i in range(0, n, 4):
        a, c = orig[i:i + 4], comp[i:i + 4]
        if i in omask or i in cmask:
            # A relocation in our object where the original has a plain word is allowed: it is
            # usually a linker-defined constant (SDK_*). The full build verifies the final value.
            if (i in omask and i not in cmask) or len(a) != len(c):
                bad.append(i)
            continue
        if a != c:
            bad.append(i)
    return bad


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("source")
    ap.add_argument("function")
    ap.add_argument("--module")
    ap.add_argument("--compiler", default=DEFAULT_COMPILER)
    ap.add_argument("--flags", default=None, help="override the per-area flags from flags.py")
    ap.add_argument("--all-compilers", action="store_true")
    ap.add_argument("-q", "--quiet", action="store_true")
    ap.add_argument("--addr", help="pick this address when several symbols share the name")
    a = ap.parse_args()

    module = a.module or find_module(a.function)
    at = int(a.addr, 16) if a.addr else None
    mode, addr, orig, omask = original_bytes(a.function, module, at=at)
    compilers = ([f"{fam}/{v.name}" for fam in ("2.0", "1.2", "dsi") for v in sorted((MWROOT / fam).iterdir())]
                 if a.all_compilers else [a.compiler])
    ok_any = False
    with tempfile.TemporaryDirectory() as tmp:
        for comp in compilers:
            obj = Path(tmp) / "out.o"
            try:
                compile_src(ROOT / a.source, comp, a.flags, obj)
                data, cmask = compiled_bytes(obj, a.function)
                if len(data) > len(orig):
                    _, _, orig, omask = original_bytes(a.function, module, len(data), at=at)
            except SystemExit as e:
                print(f"{comp}: {e}")
                continue
            bad = compare(orig, omask, data, cmask)
            status = "MATCH" if not bad and len(orig) == len(data) else \
                f"diff ({len(bad)} words, size {len(data):#x} vs {len(orig):#x})"
            ok_any |= status == "MATCH"
            print(f"{comp}: {a.function} @ {addr:#010x} [{module}, {mode}] -> {status}")
            if bad and not a.quiet and len(compilers) == 1:
                for i in bad[:40]:
                    print(f"  +{i:#05x}: orig {orig[i:i+4].hex() or '----':8}  ours {data[i:i+4].hex() or '----'}")
    return 0 if ok_any else 1


if __name__ == "__main__":
    sys.exit(main())

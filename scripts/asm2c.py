"""Convert one function from the dsd disassembly into an mwcc `asm` function body.

Used for functions whose C doesn't match yet (e.g. register allocation): the file
stays a complete C unit and the function is clearly marked as non-matching.

Usage:  python scripts/asm2c.py <function> ["C prototype"]
Names are resolved through the current symbols.txt files, so run it after renaming.
"""
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
from asm import find  # noqa: E402

CONFIG = ROOT / "config" / "usa" / "arm9"


def load_names():
    names = {}
    for f in CONFIG.rglob("symbols.txt"):
        for line in f.read_text().splitlines():
            m = re.match(r"^(\S+) kind:\S+ addr:0x([0-9a-f]+)", line)
            if m:
                names.setdefault(int(m.group(2), 16), m.group(1))
    return names


def resolve(sym, names):
    m = re.fullmatch(r"(?:func|data)_(?:ov\d+_)?([0-9a-f]{8})", sym)
    if m:
        return names.get(int(m.group(1), 16), sym)
    return sym


def convert(func, names):
    text = find(func)
    if text.startswith(";"):
        # maybe the asm still uses the old auto name
        for addr, name in names.items():
            if name == func:
                text = find(f"func_{addr:08x}")
                break
    lines = text.splitlines()
    pool = {}
    for line in lines:
        m = re.match(r"^\s*\.L_([0-9a-f]+): \.word (\S+)", line)
        if m:
            pool[m.group(1)] = m.group(2)
    out = []
    for line in lines:
        s = line.split(";")[0].rstrip()
        if not s.strip() or "_func_start" in s or "_func_end" in s:
            continue
        if re.match(r"^\S+: *$", s.strip()) and not s.strip().startswith(".L_"):
            continue  # function label
        if re.match(r"^\s*\.L_[0-9a-f]+: \.word", s):
            continue  # literal pool entry
        m = re.match(r"^\s*\.L_([0-9a-f]+):\s*$", s)
        if m:
            out.append(f"_{m.group(1)}:")
            continue
        s = s.strip()
        m = re.match(r"^(ldr\w*)\s+(r\d+|ip|lr|sp), \.L_([0-9a-f]+)$", s)
        if m and m.group(3) in pool:
            val = pool[m.group(3)]
            if not val.startswith("0x"):
                val = resolve(val, names)
            out.append(f"    {m.group(1)} {m.group(2)}, ={val}")
            continue
        s = re.sub(r"\.L_([0-9a-f]+)", r"_\1", s)
        s = re.sub(r"\b((?:func|data)_(?:ov\d+_)?[0-9a-f]{8})\b", lambda mm: resolve(mm.group(1), names), s)
        out.append("    " + s)
    return out


def main():
    func = sys.argv[1]
    proto = sys.argv[2] if len(sys.argv) > 2 else f"void {func}(void)"
    body = convert(func, load_names())
    print("// NONMATCHING: kept as assembly until the C version matches.")
    print(f"asm {proto} {{")
    print("\n".join(body))
    print("}")


if __name__ == "__main__":
    main()

"""Print the assembly of one function from the dsd disassembly (build/usa/asm).

Usage:  python scripts/asm.py <function> [<function> ...]
Run `dsd dis -c config/usa/arm9/config.yaml -a build/usa/asm` first.
"""
import re
import sys
from pathlib import Path

ASM = Path(__file__).resolve().parents[1] / "build" / "usa" / "asm"


def find(name):
    start = re.compile(rf"^\s+(arm|thumb)_func_start {re.escape(name)}$")
    for f in sorted(ASM.glob("*.s")):
        out, on = [], False
        with f.open(encoding="utf-8", errors="replace") as fh:
            for line in fh:
                if not on and start.match(line):
                    on = True
                if on:
                    out.append(line.rstrip())
                    if re.match(rf"^\s+(arm|thumb)_func_end {re.escape(name)}$", line):
                        return "\n".join(out)
    return f"; {name} not found"


if __name__ == "__main__":
    for n in sys.argv[1:]:
        print(find(n))
        print()

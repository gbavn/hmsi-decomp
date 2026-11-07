"""Compile a source file and print the compiler's disassembly of one function.

Usage:  python scripts/objdis.py <source> <function> [--compiler 2.0/sp2p2]
Compare with:  python scripts/asm.py <function>
"""
import argparse
import os
import re
import subprocess
import sys
import tempfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import match  # noqa: E402


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("source")
    ap.add_argument("function")
    ap.add_argument("--compiler", default=match.DEFAULT_COMPILER)
    a = ap.parse_args()
    env = dict(os.environ, LM_LICENSE_FILE=str(match.MWROOT / "license.dat"))
    with tempfile.TemporaryDirectory() as tmp:
        obj = Path(tmp) / "out.o"
        a.compiler = a.compiler or match.compiler_for(a.source)
        match.compile_src(match.ROOT / a.source, a.compiler, None, obj)
        out = Path(tmp) / "out.txt"
        subprocess.run([str(match.MWROOT / a.compiler / "mwldarm.exe"), "-dis", "-show", "only,code", str(obj),
                        "-o", str(out)], env=env, capture_output=True, text=True, timeout=120)
        text = out.read_text(errors="replace") if out.exists() else ""
    lines = text.splitlines()
    on = False
    for line in lines:
        if re.search(rf"\b{re.escape(a.function)}\b", line) and ("func" in line.lower() or line.rstrip().endswith(":")):
            on = True
        elif on and re.match(r"^\S.*:\s*$", line) and a.function not in line:
            break
        if on:
            print(line)
    if not on:
        print(text[:4000])


if __name__ == "__main__":
    main()

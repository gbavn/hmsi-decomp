"""Check every function of every `complete` C unit with match.py.

Usage:  python scripts/checkall.py [--flags "..."] [--only substring]
"""
import argparse
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CONFIG = ROOT / "config" / "usa" / "arm9"
SYM_RE = re.compile(r"^(\S+) kind:function\((?:arm|thumb),size=0x[0-9a-f]+\) addr:0x([0-9a-f]+)")


def units():
    for delinks in CONFIG.rglob("delinks.txt"):
        module = "main" if delinks.parent == CONFIG else delinks.parent.name
        syms = []
        for line in delinks.with_name("symbols.txt").read_text().splitlines():
            m = SYM_RE.match(line)
            if m:
                syms.append((int(m.group(2), 16), m.group(1)))
        _, _, files = delinks.read_text().partition("\n\n")
        for block in re.split(r"\n\s*\n", files):
            lines = block.strip().splitlines()
            if not lines or not re.search(r"^\s+complete\s*$", block, re.M):
                continue
            src = lines[0].rstrip(":")
            for m in re.finditer(r"\.text\s+start:0x([0-9a-f]+) end:0x([0-9a-f]+)", block):
                s, e = int(m.group(1), 16), int(m.group(2), 16)
                funcs = [(n, a) for a, n in syms if s <= a < e]
                yield module, src, funcs


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--flags")
    ap.add_argument("--only")
    a = ap.parse_args()
    bad = 0
    for module, src, funcs in units():
        if a.only and a.only not in src:
            continue
        for f, addr in funcs:
            cmd = [sys.executable, str(ROOT / "scripts" / "match.py"), src, f, "--module", module, "-q",
                   "--addr", hex(addr)]
            if a.flags:
                cmd += ["--flags", a.flags]
            r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
            line = (r.stdout.strip().splitlines() or [r.stderr.strip()])[-1]
            if "MATCH" not in line or "diff" in line:
                bad += 1
                print(f"{src}: {line}")
    print("non-matching:", bad)
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())

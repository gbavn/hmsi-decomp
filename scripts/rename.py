"""Rename symbols in the dsd symbols.txt files.

Usage:  python scripts/rename.py OLD NEW [OLD NEW ...] [--local NAME ...]
"""
import re
import sys
from pathlib import Path

CONFIG = Path(__file__).resolve().parents[1] / "config" / "usa" / "arm9"


def main(argv):
    local = set()
    if "--local" in argv:
        i = argv.index("--local")
        local = set(argv[i + 1:])
        argv = argv[:i]
    pairs = dict(zip(argv[0::2], argv[1::2]))
    found = set()
    for f in CONFIG.rglob("symbols.txt"):
        lines = f.read_text().splitlines()
        changed = False
        for i, line in enumerate(lines):
            name = line.split(" ", 1)[0]
            new = pairs.get(name)
            if new:
                line = new + line[len(name):]
                found.add(name)
            if (new or name) in local and not re.search(r"\blocal\b", line):
                line += " local"
            if line != lines[i]:
                lines[i] = line
                changed = True
        if changed:
            f.write_text("\n".join(lines) + "\n")
    missing = set(pairs) - found
    if missing:
        print("not found:", " ".join(sorted(missing)))
        return 1
    print(f"renamed {len(found)} symbols")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))

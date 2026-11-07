"""Try every order of a function's leading local declarations until match.py reports a MATCH.

mwcc's register allocation depends on declaration order, so a function that differs only in
register numbers often matches after reordering its locals.

Usage:  python scripts/permdecl.py <source> <function>
The declarations are the consecutive lines right after the function's opening brace that look
like `type name;` or `type name = value;`. The file is left with the first matching order
(or restored if none matches).
"""
import itertools
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
DECL = re.compile(r"^\s+[A-Za-z_][\w\s\*]*[\s\*]\**[A-Za-z_]\w*(\s*=\s*[^;]+)?;\s*$")


def main():
    src, func = sys.argv[1], sys.argv[2]
    path = ROOT / src
    text = path.read_text()
    lines = text.split("\n")
    head = re.compile(rf"^\S.*\b{re.escape(func)}\s*\(")
    start = next(i for i, l in enumerate(lines) if head.match(l))
    brace = next(i for i in range(start, len(lines)) if lines[i].strip() == "{")
    first = brace + 1
    last = first
    while last < len(lines) and DECL.match(lines[last]):
        last += 1
    decls = lines[first:last]
    if len(decls) < 2:
        sys.exit("fewer than two declarations found")
    print(f"{len(decls)} declarations, {len(list(itertools.permutations(decls)))} orders")
    for perm in itertools.permutations(decls):
        path.write_text("\n".join(lines[:first] + list(perm) + lines[last:]))
        r = subprocess.run([sys.executable, str(ROOT / "scripts" / "match.py"), src, func, "-q"],
                           cwd=ROOT, capture_output=True, text=True).stdout.strip()
        if r.endswith("MATCH"):
            print("MATCH with order:")
            print("\n".join(perm))
            return 0
    path.write_text(text)
    print("no order matched; file restored")
    return 1


if __name__ == "__main__":
    sys.exit(main())

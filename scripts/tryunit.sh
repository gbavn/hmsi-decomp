#!/usr/bin/env bash
# Add a library file, build, and explain why it fails (config is reverted afterwards).
# Usage: scripts/tryunit.sh <source> [addlib args]
cd "$(dirname "$0")/.."
src=$1; shift
if [ -n "$(git status --porcelain config/)" ]; then
    echo "config/ has uncommitted changes; commit them first (tryunit reverts config/ when done)"
    exit 1
fi
python scripts/addlib.py "$src" "$@" 2>&1 | grep -E "warning|start:|functions:|itcm" | sed 's/^/  addlib: /'
python scripts/build.py --quick > /tmp/tryunit.log 2>&1
if grep -q "^MATCH" /tmp/tryunit.log; then
    echo "  BUILD OK"
else
    grep -E -A1 "Undefined|ERROR|Linker Error|Error:" /tmp/tryunit.log \
        | grep -Ev "didn't pass|see logs|see errors|^--|Linker Error:|CalledProcessError" | head -8 | sed 's/^/  /'
    if grep -q "didn't pass" /tmp/tryunit.log; then
        python - <<'EOF'
import re
syms = []
for line in open("config/usa/arm9/symbols.txt"):
    m = re.match(r"^(\S+) kind:function\S* addr:0x([0-9a-f]+)", line)
    if m:
        syms.append((int(m.group(2), 16), m.group(1)))
syms.sort()
a = open("extract/usa/arm9/arm9.bin", "rb").read()
b = open("build/usa/build/arm9.bin", "rb").read()
d = [i for i in range(min(len(a), len(b))) if a[i] != b[i]]
print(f"  arm9 size {len(a):#x} vs {len(b):#x}, {len(d)} bytes differ")
seen = set()
for i in d[:400]:
    addr = 0x02000000 + i - i % 4
    if addr in seen:
        continue
    seen.add(addr)
    owner = max((s for s in syms if s[0] <= addr), default=(0, "?"))
    print(f"  {addr:#010x} in {owner[1]}+{addr-owner[0]:#x}: orig {a[addr-0x02000000:addr-0x02000000+4].hex()} ours {b[addr-0x02000000:addr-0x02000000+4].hex()}")
    if len(seen) >= 6:
        break
EOF
    fi
fi
git checkout -q -- config/

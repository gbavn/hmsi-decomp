#!/usr/bin/env bash
# Try to add many library files as units, one at a time.
# For each file: addlib.py, then a full build + ROM check. On failure the config is reverted.
# Successful files are committed locally (push separately).
# Usage: scripts/batchlib.sh file1.c file2.c ...
set -uo pipefail
cd "$(dirname "$0")/.."

ok=0; fail=0; skip=0
for src in "$@"; do
    if grep -q "^$src:" config/usa/arm9/delinks.txt; then
        continue
    fi
    out=$(python scripts/addlib.py "$src" 2>&1)
    if ! echo "$out" | grep -q "^added to"; then
        echo "SKIP  $src: $(echo "$out" | tail -1)"
        git checkout -q -- config/
        skip=$((skip+1))
        continue
    fi
    nfun=$(echo "$out" | grep -o "functions: [0-9]*")
    if python scripts/build.py --quick >/tmp/batchlib_build.log 2>&1 && grep -q "^MATCH" /tmp/batchlib_build.log; then
        git add -A config/ >/dev/null
        git commit -q -m "Add library unit $src ($nfun)

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
        echo "OK    $src ($nfun)"
        ok=$((ok+1))
    else
        echo "FAIL  $src ($nfun)"
        grep -E "Undefined|Error|error" /tmp/batchlib_build.log | head -3 | sed 's/^/      /'
        git checkout -q -- config/
        fail=$((fail+1))
    fi
done
echo "done: ok=$ok fail=$fail skip=$skip"

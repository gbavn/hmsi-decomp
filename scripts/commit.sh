#!/usr/bin/env bash
# Build, verify the ROM still matches, then commit and push.
# Usage: scripts/commit.sh "commit message"
set -euo pipefail
cd "$(dirname "$0")/.."

log=$(mktemp)
if ! python scripts/build.py >"$log" 2>&1; then
    echo "BUILD FAILED - not committing"
    grep -v "^\[INFO" "$log" | tail -20
    exit 1
fi
if ! grep -q "^MATCH" "$log"; then
    echo "ROM DOES NOT MATCH - not committing"
    tail -5 "$log"
    exit 1
fi
tail -1 "$log"

git add -A
git commit -q -F - <<EOF
$1

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>
EOF
git push 2>&1 | grep -v credential-manager | tail -1
python scripts/progress.py | tail -1

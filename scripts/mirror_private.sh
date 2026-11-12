#!/usr/bin/env bash
# Copies the new commits of the local main (whose history is the public one, gbavn/hmsi-decomp) to the
# private working repository (remote `old-private`, gbavn/sunshine-islands-decomp).
#
# The two repositories have different histories (the public one was rewritten without the ROM), but the
# same files. Each new commit is recreated on top of the private main with the same tree, author, date and
# message, plus a "Public-commit: <hash>" line that tells which public commits were already copied.
# Nothing is ever deleted from the private history (plain fast-forward push).
# Usage: scripts/mirror_private.sh
set -euo pipefail
cd "$(dirname "$0")/.."

REMOTE=old-private
git fetch -q "$REMOTE" 2>&1 | grep -v credential-manager || true
tip=$(git rev-parse "$REMOTE/main")

# Last public commit already copied (2fe60e3 = the state of both repositories on 2026-11-08).
last=$(git log -1 --format=%B "$tip" | sed -n 's/^Public-commit: //p')
if [ -z "$last" ]; then
    last=2fe60e3
fi
if ! git merge-base --is-ancestor "$last" main; then
    echo "mirror: $last is not in the local main, not copying"
    exit 1
fi

count=0
for c in $(git rev-list --reverse "$last"..main); do
    msg=$(git log -1 --format=%B "$c")
    tip=$(GIT_AUTHOR_NAME="$(git log -1 --format=%an "$c")" \
        GIT_AUTHOR_EMAIL="$(git log -1 --format=%ae "$c")" \
        GIT_AUTHOR_DATE="$(git log -1 --format=%ad --date=raw "$c")" \
        GIT_COMMITTER_NAME="$(git log -1 --format=%cn "$c")" \
        GIT_COMMITTER_EMAIL="$(git log -1 --format=%ce "$c")" \
        GIT_COMMITTER_DATE="$(git log -1 --format=%cd --date=raw "$c")" \
        git commit-tree "$c^{tree}" -p "$tip" -m "$msg" -m "Public-commit: $c")
    count=$((count + 1))
done

if [ "$count" -eq 0 ]; then
    echo "mirror: private repository already up to date"
    exit 0
fi
git push "$REMOTE" "$tip:refs/heads/main" 2>&1 | grep -v credential-manager | tail -1
echo "mirror: $count commit(s) copied to $REMOTE"

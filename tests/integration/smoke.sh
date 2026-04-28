#!/usr/bin/env bash
# Smoke test: init → add → status → commit. Each step must exit 0 and leave
# the repository in the expected on-disk shape.
set -euo pipefail

: "${VLS_BIN:?VLS_BIN must be set to the vls binary path}"
[[ -x "$VLS_BIN" ]] || { echo "VLS_BIN=$VLS_BIN not executable" >&2; exit 1; }

TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT
cd "$TMP"

# --- init -------------------------------------------------------------
"$VLS_BIN" init >/dev/null
for dir in .vls .vls/commits .vls/objects; do
  [[ -d "$dir" ]] || { echo "init: directory $dir missing" >&2; exit 1; }
done
for file in .vls/head .vls/stage.json; do
  [[ -f "$file" ]] || { echo "init: file $file missing" >&2; exit 1; }
done

# --- add --------------------------------------------------------------
echo "hello" > foo.txt
"$VLS_BIN" add foo.txt >/dev/null

[[ -s .vls/stage.json ]] \
  || { echo "add: stage.json is empty" >&2; exit 1; }
grep -q '"path":"foo.txt"' .vls/stage.json \
  || { echo "add: foo.txt not in stage.json:" >&2; cat .vls/stage.json >&2; exit 1; }
grep -q '"hash":"b1946ac92492d2347c6235b4d2611184"' .vls/stage.json \
  || { echo "add: wrong hash for 'hello\\n':" >&2; cat .vls/stage.json >&2; exit 1; }

# --- status -----------------------------------------------------------
"$VLS_BIN" status >/dev/null

# --- commit -----------------------------------------------------------
BLOB_HASH="b1946ac92492d2347c6235b4d2611184"
COMMIT_MSG="initial commit"

# Capture stage.json BEFORE commit (commit will mutate it).
STAGE_BEFORE="$(cat .vls/stage.json)"

"$VLS_BIN" commit "$COMMIT_MSG" >/dev/null

# Blob content (the working-tree file) must be in objects/.
[[ -f ".vls/objects/$BLOB_HASH" ]] \
  || { echo "commit: blob .vls/objects/$BLOB_HASH missing" >&2; ls -la .vls/objects >&2; exit 1; }
diff -q foo.txt ".vls/objects/$BLOB_HASH" >/dev/null \
  || { echo "commit: blob content differs from foo.txt" >&2; exit 1; }

# HEAD must hold a 32-char hex hash.
HEAD_HASH="$(cat .vls/head)"
[[ ${#HEAD_HASH} -eq 32 ]] \
  || { echo "commit: HEAD is not a 32-char hash: '$HEAD_HASH'" >&2; exit 1; }

# Commit dir + metadata.
CDIR=".vls/commits/$HEAD_HASH"
[[ -d "$CDIR" ]] || { echo "commit: $CDIR missing" >&2; exit 1; }
for f in commit.json hash msg prev next time; do
  [[ -f "$CDIR/$f" ]] || { echo "commit: $CDIR/$f missing" >&2; exit 1; }
done

[[ "$(cat "$CDIR/hash")" = "$HEAD_HASH" ]] \
  || { echo "commit: hash file != HEAD" >&2; exit 1; }
[[ "$(cat "$CDIR/msg")" = "$COMMIT_MSG" ]] \
  || { echo "commit: msg mismatch:" >&2; cat "$CDIR/msg" >&2; exit 1; }
[[ ! -s "$CDIR/prev" ]] \
  || { echo "commit: first commit's prev should be empty:" >&2; cat "$CDIR/prev" >&2; exit 1; }
[[ -s "$CDIR/time" ]] \
  || { echo "commit: time file is empty" >&2; exit 1; }
[[ "$(cat "$CDIR/commit.json")" = "$STAGE_BEFORE" ]] \
  || { echo "commit: commit.json doesn't match pre-commit stage.json" >&2; exit 1; }

# Stage statuses must transition to UNCHANGED (0).
grep -q '"status":0' .vls/stage.json \
  || { echo "commit: stage status not reset to UNCHANGED:" >&2; cat .vls/stage.json >&2; exit 1; }

# --- second commit (chain) -------------------------------------------
echo "world" > bar.txt
"$VLS_BIN" add bar.txt >/dev/null
"$VLS_BIN" commit "second" >/dev/null

HEAD2="$(cat .vls/head)"
[[ "$HEAD2" != "$HEAD_HASH" ]] \
  || { echo "commit: HEAD did not advance after second commit" >&2; exit 1; }

CDIR2=".vls/commits/$HEAD2"
[[ -d "$CDIR2" ]] || { echo "commit: $CDIR2 missing" >&2; exit 1; }

# Linked-list invariant: new commit's prev points back to previous HEAD.
[[ "$(cat "$CDIR2/prev")" = "$HEAD_HASH" ]] \
  || { echo "commit: second commit's prev != first commit hash:" >&2; cat "$CDIR2/prev" >&2; exit 1; }

# First commit dir must still exist (commits are append-only).
[[ -d "$CDIR" ]] || { echo "commit: first commit dir vanished after second commit" >&2; exit 1; }

echo "smoke: OK"

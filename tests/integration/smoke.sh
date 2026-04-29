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
for f in commit.json hash msg prev time; do
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

# --- reset (round-trip) ----------------------------------------------
# Modify foo.txt and commit a third state, then reset back to the first
# commit. foo.txt content must come back, HEAD must move back, stage must
# match the first commit's snapshot.
echo "modified" > foo.txt
"$VLS_BIN" add foo.txt >/dev/null
"$VLS_BIN" commit "modify foo" >/dev/null
HEAD3="$(cat .vls/head)"

"$VLS_BIN" reset "$HEAD_HASH" >/dev/null

[[ "$(cat .vls/head)" = "$HEAD_HASH" ]] \
  || { echo "reset: HEAD did not move back to first commit" >&2; exit 1; }
[[ "$(cat foo.txt)" = "hello" ]] \
  || { echo "reset: foo.txt was not restored to v1 ('hello'):" >&2; cat foo.txt >&2; exit 1; }
grep -q '"status":0' .vls/stage.json \
  || { echo "reset: stage statuses not normalized to UNCHANGED" >&2; cat .vls/stage.json >&2; exit 1; }

# Append-only objects: blobs of all three commits must coexist.
[[ -f ".vls/objects/$BLOB_HASH" ]] \
  || { echo "reset: original v1 blob missing — objects/ is not append-only" >&2; exit 1; }

# --- drop (untrack without touching disk) ----------------------------
# Stage a fresh file, drop it, verify the file stays on disk and stage no
# longer mentions it.
echo "scratch" > junk.txt
"$VLS_BIN" add junk.txt >/dev/null
grep -q '"path":"junk.txt"' .vls/stage.json \
  || { echo "drop: precondition failed — junk.txt not in stage" >&2; exit 1; }

"$VLS_BIN" drop junk.txt >/dev/null

[[ -f junk.txt ]] \
  || { echo "drop: working-tree file was wiped — drop must not touch disk" >&2; exit 1; }
grep -q '"path":"junk.txt"' .vls/stage.json \
  && { echo "drop: junk.txt still in stage.json:" >&2; cat .vls/stage.json >&2; exit 1; } || true

# --- regression: post-reset re-add must not collide with old commit ----
# Scenario: commit a file, delete it, commit, reset to first, reset to
# second (file lingers on disk but is absent from the index), re-add it,
# commit. The new commit's stage state happens to match the first commit's
# byte-for-byte; the commit hash must still differ so the prev-chain stays
# acyclic and `log` terminates.
REGRESSION_DIR="$(mktemp -d)"
(
  cd "$REGRESSION_DIR"
  "$VLS_BIN" init >/dev/null
  echo hello > F
  "$VLS_BIN" add F >/dev/null
  "$VLS_BIN" commit first >/dev/null
  R1="$(cat .vls/head)"
  rm F
  "$VLS_BIN" commit second >/dev/null
  R2="$(cat .vls/head)"
  "$VLS_BIN" reset "$R1" >/dev/null
  "$VLS_BIN" reset "$R2" >/dev/null
  "$VLS_BIN" add F >/dev/null
  "$VLS_BIN" commit third >/dev/null
  R3="$(cat .vls/head)"

  [[ "$R3" != "$R1" && "$R3" != "$R2" ]] \
    || { echo "regression: third commit collided with an ancestor (R1=$R1 R2=$R2 R3=$R3)" >&2; exit 1; }

  LOG_OUT="$(timeout 3 "$VLS_BIN" log)" \
    || { echo "regression: 'vls log' did not terminate or failed" >&2; exit 1; }
  N="$(printf '%s\n' "$LOG_OUT" | grep -c '^commit ')"
  [[ "$N" -eq 3 ]] \
    || { echo "regression: expected 3 commits in log, got $N" >&2; printf '%s\n' "$LOG_OUT" >&2; exit 1; }
)
rm -rf "$REGRESSION_DIR"

echo "smoke: OK"

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
HASH="b1946ac92492d2347c6235b4d2611184"

"$VLS_BIN" commit >/dev/null

[[ -f ".vls/objects/$HASH" ]] \
  || { echo "commit: blob .vls/objects/$HASH missing" >&2; ls -la .vls/objects >&2; exit 1; }
diff -q foo.txt ".vls/objects/$HASH" >/dev/null \
  || { echo "commit: blob content differs from foo.txt" >&2; exit 1; }

echo "smoke: OK"

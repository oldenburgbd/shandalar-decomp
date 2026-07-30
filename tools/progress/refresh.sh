#!/usr/bin/env bash
# One command that brings all project metrics up to date.
#
#   refresh.sh          fast: rescan inventories, recompute metrics, rebuild dashboard
#   refresh.sh --full   also re-run Ghidra headless analysis first (slow, ~2 min/binary)
#
# Safe to run any time; it never touches orig/ and never downgrades a function's status.
set -uo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
PY="${MTGRE_PYTHON:-$HOME/AppData/Local/Programs/Python/Python312/python.exe}"
cd "$ROOT" || exit 1

if [ ! -x "$PY" ] && ! command -v "$PY" >/dev/null 2>&1; then
  echo "refresh: python not found at $PY (set MTGRE_PYTHON)" >&2
  exit 1
fi

if [ "${1:-}" = "--full" ]; then
  echo "== re-running Ghidra headless analysis =="
  rm -rf build/ghidra-projects
  bash tools/ghidra/run-headless.sh shandalar orig/Shandalar.exe   >/dev/null 2>&1 &
  p1=$!
  bash tools/ghidra/run-headless.sh magic      orig/Magic.exe      >/dev/null 2>&1 &
  p2=$!
  bash tools/ghidra/run-headless.sh libs \
      orig/Deckdll.dll orig/Drawcardlib.dll orig/Cardartlib.dll orig/Gcconn.dll \
      orig/Cdtools.dll orig/Statwin.dll orig/Manalinkinterface.dll orig/Magvid.dll \
      orig/Magsnd.dll orig/Deck.exe orig/Facemaker.exe orig/Facemaker_sub.exe \
      orig/Manalink.exe >/dev/null 2>&1 &
  p3=$!
  wait $p1 $p2 $p3
  echo "   done"
fi

echo "== verifying originals are untouched =="
if [ -f orig/SHA256SUMS.txt ]; then
  ( cd orig && sha256sum -c SHA256SUMS.txt --quiet ) \
    && echo "   originals OK" \
    || { echo "   !! ORIGINALS CHANGED -- investigate before trusting any metric" >&2; exit 2; }
fi

echo "== refreshing progress database =="
"$PY" tools/progress/progress.py scan
"$PY" tools/progress/progress.py report
"$PY" tools/progress/progress.py dash

echo "== emitting objdiff report =="
"$PY" tools/progress/objdiff_report.py -o reports/report.json
# decomp.dev's GitHub path looks for report.json / <version>_report.json / progress.json
# inside a workflow-artifact zip, so publish under the version name too.
cp reports/report.json reports/shandalar_report.json

# Publish into the local decomp.dev instance, if it is set up. Skipped silently
# otherwise -- decomp.dev is a nice-to-have view, not a dependency of the metrics.
# See docs/05-decomp-dev.md.
DDLOCAL="${DECOMP_DEV_LOCAL:-/c/Users/bo1026/decomp-target/debug/decomp-dev-local.exe}"
DDDIR="${DECOMP_DEV_DIR:-/c/Users/bo1026/Desktop/RE Project/decomp.dev}"
if [ -x "$DDLOCAL" ] && [ -f "$DDDIR/config.yml" ]; then
  echo "== publishing to local decomp.dev =="
  # Two constraints from the importer:
  #  - it derives its commit id from the report BYTES, so any report whose content
  #    was ever published before collides on report_report_units -- including a
  #    revert back to an earlier state. So we always pass an explicit --commit.
  #  - decomp.dev keeps one history datapoint per commit, so publishing an
  #    unchanged report would just spam the burn-up chart. Hence the skip.
  NEWSUM="$(sha256sum reports/report.json | cut -d' ' -f1)"
  OLDSUM="$(cat progress/.last-published 2>/dev/null || echo none)"
  if [ "$NEWSUM" = "$OLDSUM" ]; then
    echo "   unchanged since last publish -- skipped"
  else
    # Unique, monotonic-ish 40-hex commit id: content hash mixed with the clock.
    COMMIT="$(printf '%s%s' "$NEWSUM" "$(date -u +%s%N)" | sha256sum | cut -c1-40)"
    # Build the message BEFORE the subshell -- inside it, cwd is decomp.dev and
    # a relative path would not resolve.
    # cygpath -m: Python is a native Windows binary and cannot open /c/... paths.
    PROGJSON="$(cygpath -m "$ROOT/reports/progress.json" 2>/dev/null || echo "$ROOT/reports/progress.json")"
    MSG="$("$PY" -c "
import json
m = json.load(open(r'$PROGJSON'))['totals']
print('%.3f%% code (%d/%d bytes), %d/%d functions' % (
    m['pct_bytes'], m['done_bytes'], m['bytes'],
    m['done_functions'], m['functions']))" 2>/dev/null)"
    [ -n "$MSG" ] || MSG="progress update"
    # Must run with decomp.dev as cwd: config.yml (and thus db.sqlite) is
    # resolved relative to the working directory.
    REPORT_WIN="$(cygpath -w "$ROOT/reports/report.json" 2>/dev/null || echo "$ROOT/reports/report.json")"
    if ( cd "$DDDIR" && "$DDLOCAL" import \
          --file "$REPORT_WIN" --commit "$COMMIT" --message "$MSG" \
          --owner mtg-re --repo shandalar --platform win32 --version shandalar \
          --name "Magic: The Gathering - Shandalar" --short-name Shandalar ) >/dev/null 2>&1; then
      echo "$NEWSUM" > progress/.last-published
      echo "   http://localhost:3000/mtg-re/shandalar  (commit ${COMMIT:0:8})"
    else
      echo "   (import failed -- see docs/05-decomp-dev.md)"
    fi
  fi
fi

date -u +%Y-%m-%dT%H:%M:%SZ > progress/.last-report
rm -f progress/.refresh-needed

echo
"$PY" tools/progress/progress.py stats

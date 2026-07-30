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

echo "== emitting objdiff report for decomp.dev =="
"$PY" tools/progress/objdiff_report.py -o reports/report.json
# decomp.dev looks for report.json / <version>_report.json / progress.json inside a
# GitHub Actions artifact zip, so publish under the version name too.
cp reports/report.json reports/shandalar_report.json

date -u +%Y-%m-%dT%H:%M:%SZ > progress/.last-report
rm -f progress/.refresh-needed

echo
"$PY" tools/progress/progress.py stats

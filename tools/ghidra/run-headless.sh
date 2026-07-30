#!/usr/bin/env bash
# Headless Ghidra import + auto-analysis + inventory export.
# Usage: run-headless.sh <project-name> <binary> [<binary>...]
set -uo pipefail

export JAVA_HOME="${JAVA_HOME:-/c/Users/bo1026/Desktop/jdk-21.0.12+8}"
export PATH="$JAVA_HOME/bin:$PATH"

GHIDRA="${GHIDRA_HOME:-/c/Users/bo1026/Desktop/ghidra_12.1.2_PUBLIC}"
ROOT="/c/Users/bo1026/Desktop/RE Project/mtg-re"
PROJDIR="$ROOT/build/ghidra-projects"
OUTDIR="$ROOT/reports/ghidra"
SCRIPTS="$ROOT/tools/ghidra"

PROJ="$1"; shift
mkdir -p "$PROJDIR" "$OUTDIR"

# Convert to Windows paths for the .bat launcher.
winpath() { printf '%s' "$1" | sed -e 's|^/c/|C:/|' -e 's|/|\\|g'; }

ARGS=()
for f in "$@"; do ARGS+=("$(winpath "$f")"); done

"$GHIDRA/support/analyzeHeadless.bat" \
  "$(winpath "$PROJDIR")" "$PROJ" \
  -import "${ARGS[@]}" \
  -scriptPath "$(winpath "$SCRIPTS")" \
  -postScript ExportFunctionInventory.java "$(winpath "$OUTDIR")" \
  -analysisTimeoutPerFile 3600 \
  -loader PeLoader \
  2>&1
echo "EXIT=$?"

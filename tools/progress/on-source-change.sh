#!/usr/bin/env bash
# Fast PostToolUse guard. Runs after every Edit/Write, so it must stay cheap.
#
# Two jobs:
#   1. Fail loudly if anything wrote into orig/ or the real game install.
#   2. Flag that source changed, so the Stop hook knows to recompute metrics.
#
# Real metric recomputation lives in refresh.sh and runs at Stop, not here.
set -uo pipefail

ROOT="${CLAUDE_PROJECT_DIR:-$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)}"
cd "$ROOT" 2>/dev/null || exit 0

# --- 1. tripwire on the pristine copies -------------------------------------
if [ -f orig/SHA256SUMS.txt ]; then
  if ! ( cd orig && sha256sum -c SHA256SUMS.txt --quiet 2>/dev/null ); then
    echo "BLOCKED: files under orig/ no longer match SHA256SUMS.txt." >&2
    echo "The pristine binaries must never be modified. Restore them before continuing." >&2
    exit 2   # non-zero surfaces the message back to the agent
  fi
fi

# --- 2. mark that a refresh is due ------------------------------------------
if [ -d src ] && [ -n "$(find src -newer progress/.last-report -type f 2>/dev/null | head -1)" ]; then
  : > progress/.refresh-needed
fi

exit 0

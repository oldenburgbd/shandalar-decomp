# ADR-0004 — Progress metric definition and publishing

**Status:** accepted · **Date:** 2026-07-30

## Context

A matching decompilation runs for a long time at a low percentage. The progress metric is the
only steering signal, so it has to be (a) honest, (b) automatic, and (c) legible to a human who
checks in occasionally. A metric that can be inflated by good intentions is worse than none.

The user asked specifically for progress to be reported through a **local decomp.dev instance**
(<https://github.com/encounter/decomp.dev>), which is the decompilation community's standard
progress site.

## Decision

### The metric

**Headline figure = matched code bytes ÷ total code bytes.**

Only two statuses count as matched:

- `matching` — compiles to byte-identical code.
- `library` — CRT or compiler-helper code, matched by linking the correct library rather than
  by hand-decompilation.

Everything else scores **zero**, including `equivalent` (compiles, behaves identically, bytes
differ). Bytes rather than function count is the headline because function sizes span
three orders of magnitude here (median 301 bytes, max 14,765), so a function-count metric would
misrepresent remaining effort.

Function count is still reported as a secondary figure.

### Publishing

Progress is emitted in the **objdiff report format** (`objdiff.report.Report`, schema version 2,
snake_case JSON) by `tools/progress/objdiff_report.py`, mapping:

- objdiff *unit* → one translation unit, or a `<binary>/UNASSIGNED` bucket until TUs are assigned
- objdiff *function* → one function row
- objdiff *category* → one per binary and one per subsystem, so progress can be sliced both ways
- objdiff *section* → not emitted; data/rodata matching is not yet tracked

`fuzzy_match_percent` is 100 only for genuinely matched functions. It is deliberately **not**
used to award partial credit for "looks close" — partial percentages will only be populated once
a real build exists to measure actual byte similarity against.

## Rationale

Using the ecosystem's report format rather than a bespoke one means the numbers are directly
comparable with other decompilation projects, and it makes decomp.dev, the objdiff GUI, and any
future CI integration work without custom glue.

Keeping a local SQLite database as the source of truth — with the objdiff report as a
*rendering* of it — means the project keeps richer state (translation unit, subsystem, notes,
status history) than the report format can carry, without being constrained by it.

## Consequences

- `tools/progress/progress.py` enforces a forward-only status ladder; moving a function backwards
  requires `--force`, which makes accidental inflation visible in the audit trail.
- `refresh.sh` regenerates the database, the Markdown report, the local HTML dashboard, and the
  objdiff report in one pass, and is wired to a `Stop` hook so it runs at the end of every agent
  session.
- decomp.dev ingests reports from **GitHub Actions artifacts** containing a file named
  `report.*`, `<version>_report.*`, or `progress.*`. A purely local project therefore needs
  either a repository with a workflow that publishes the artifact, or direct insertion into the
  local instance's database. The chosen path is recorded in `docs/05-decomp-dev.md`.
- A pre-commit or CI check should fail if `matching` is claimed for a function whose bytes do not
  in fact match. Until a build exists, the status is set by hand and the honesty rule in
  `CLAUDE.md` is the only enforcement — this is a known gap, closed in Phase 3.

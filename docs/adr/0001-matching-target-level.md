# ADR-0001 — What "byte-for-byte" means for this project

**Status:** accepted · **Date:** 2026-07-30

## Context

The stated goal is "near 100% byte-for-byte RE". That phrase covers at least five distinct
technical targets of very different difficulty, and picking the wrong one either makes the
project impossible or makes the progress metric meaningless.

For a Windows PE built in 1996, whole-file reproduction depends on things that are not
properties of the source code at all: the PE timestamp, import-table ordering, the resource
directory layout, section padding bytes, and the exact linker build. Some of these are
recoverable only by reproducing the original build *environment*, not the original *source*.

## Decision

Define five levels and target **L4**, tracking **L1** as the working metric.

| Level | Claim | Verified by |
|---|---|---|
| L1 | Function code bytes match after masking relocation-dependent fields | normalised per-function hash |
| L2 | Function bytes match exactly, including absolute addresses and branch targets | raw per-function hash |
| L3 | Each produced `.obj` is byte-identical to the original's contribution | per-object comparison |
| **L4** | The linked `.text` / `.rdata` / `.data` are byte-identical | whole-section hash |
| L5 | The whole PE file is byte-identical | `sha256sum` vs `orig/` |

L5 is pursued opportunistically, not as an acceptance gate.

## Rationale

- **L1 is the right working metric.** It gives a per-function signal during the long middle of
  the project, when nothing links yet. Masking rel32 branch targets and in-image absolute
  addresses removes exactly the differences that come from layout rather than from code.
- **L4 is the right acceptance gate.** It is the strongest claim that is purely a function of
  source correctness plus correct link order. Once `.text` matches byte for byte, the
  decompilation *is* the program.
- **L5 is linker archaeology.** It is worth chasing at the end, but gating on it early would
  block on questions that have nothing to do with understanding the software.

## Consequences

- `tools/verify/funcbytes.py` implements the L1 normalisation with a real x86 length decoder, so
  instruction boundaries — and therefore which bytes are displacement fields — are correct.
- The progress metric counts only `matching` and `library`. `equivalent` (behaviourally the same,
  bytes differ) is tracked but scores zero, because counting it would let the headline number
  drift away from the actual goal.
- Reaching L4 requires solving data and `.rdata` ordering, which is a *declaration order* problem
  in the source. That work is scheduled explicitly rather than discovered late.

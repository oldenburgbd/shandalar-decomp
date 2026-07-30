# ADR-0002 — Target MicroProse's 1998 build, model Mok's patch separately

**Status:** accepted · **Date:** 2026-07-30

## Context

The installed `Shandalar.exe` and `Magic.exe` are not clean compiler output.

Measured evidence:

- Their PE COFF timestamps say **1998-03-24**, but their filesystem mtimes are **2001-07-11**.
- `Magic.exe` carries a section named **`.mok`** at RVA `0x55A000`, 4,096 bytes, with
  characteristics `0xE0000040` — **read + write + execute**. No 1996 MSVC linker emits an RWX
  section for normal code. It holds 5 functions / 400 bytes and references
  `User32.dll!SystemParametersInfoA`.
- `Readme132.txt` in the install identifies the tree as patched by **Mok's unofficial ManaLink
  update v1.3.2 (2001)**, whose author states he "failed to get MtG source code from MPS".

So the shipped binaries are third-party *binary patches* applied on top of MicroProse's 1998
ManaLink 1.3 build. No source code — not even the original MicroProse source — compiles to them.

## Decision

- **Primary target:** MicroProse's **1998 ManaLink 1.3 build**. Byte-matching this is a coherent
  goal because it is a real build product of a real source tree.
- **Secondary target:** Mok's v1.3.2 patch, modelled explicitly as a **post-link binary
  transformation** — a patch script plus a hand-written assembly stub — with its own separate
  verification, not folded into the decompilation metric.

## Rationale

Trying to make C source compile to a binary that has been hand-patched after linking is not
possible in general, and pretending otherwise would put a permanent ceiling on the match
percentage with no way to tell "we have not decompiled this yet" apart from "this can never
match".

Separating the two also makes the `.mok` code easier to reason about: it is hand-written
assembly with hooks into `.text`, and it should be documented as such rather than reverse
engineered into C that could never reproduce it.

## Consequences

- We need the **pristine 1998 ManaLink 1.3 `Shandalar.exe` / `Magic.exe`** as the real reference.
  The current `orig/` copies are the *patched* 2001 versions. Obtaining clean 1.3 binaries is a
  tracked prerequisite; until then, `.mok` hook sites must be identified and excluded from
  match accounting so they do not silently count as failures.
- Every `.mok` hook site — the places where Mok redirected control out of `.text` — must be
  located and documented. Those bytes differ from the 1998 original by construction.
- Any statement of the form "X% matched" must say which target it refers to.
- `Manalink.exe` and the DLLs appear untouched by the patch (no added sections, PE timestamps
  consistent with their 1997–98 build dates), so they do not have this problem. This is worth
  re-verifying per module rather than assumed.

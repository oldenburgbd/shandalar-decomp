# 2026-07-30 — CRT library matching (0% → 6.8%) and salvage validation

## 1. Library matching: 267,263 bytes marked `library`

New tool: `tools/libmatch/libmatch.py` + `tools/libmatch/mkimport.py`.

Method: parse the MSVC 4.2 `.LIB` archives (`C:\Tools\msvc42\MSDEV\LIB`), extract every
function symbol's code bytes from each COFF member together with its relocation table,
then byte-compare against every non-thunk function Ghidra inventoried, masking exactly
the 4-byte fields the linker rewrites (DIR32 / REL32) on both sides. A match is an
L1-strength claim: **exact bytes modulo link-time fields**, which is the evidence
standard for `library` status.

```bash
PY="$HOME/AppData/Local/Programs/Python/Python312/python.exe"
PYTHONIOENCODING=utf-8 "$PY" tools/libmatch/libmatch.py all     # -> reports/libmatch/<bin>.json
PYTHONIOENCODING=utf-8 "$PY" tools/libmatch/mkimport.py         # -> reports/libmatch/import.json
"$PY" tools/progress/progress.py import reports/libmatch/import.json
"$PY" tools/progress/progress.py report
```

Raw matcher output: 1,181 functions / 273,159 bytes. Recorded after filtering:
**927 functions / 267,263 bytes → 6.816% of the product.**

### Measured false-positive classes (why `mkimport.py` filters)

1. **The /Od trivial-function shell.** LIBCD's `___init_collate` is 18 reloc-free bytes
   (`55 8B EC 53 56 57 33 C0 E9 00000000 5F 5E 5B C9 C3`) — the identical bytes MSVC /Od
   emits for *any* `return 0;` function that saves ebx/esi/edi. `Deckdll.dll` has a run of
   28 of them mid-`.text` (app territory; Deckdll is /MD and would not link LIBCD's locale
   stubs at all). All rejected.
2. **Small forwarding wrappers.** `_strtoul` (38 B, one masked call) matched at 4 scattered
   addresses in one EXE. The linker includes `strtol.obj` once, so at least 3 are app
   wrappers with coincidentally identical bytes. All rejected via the duplicate cap +
   cluster rule.

Acceptance rules (see `mkimport.py` docstring): anchors (≥64 unmasked bytes or ≥4 relocs);
hand-written-asm member whitelist at ≥24; 1 KiB cluster growth from accepted code; at most
one accepted copy per distinct source member per binary. Rejected-but-plausible leads stay
in `reports/libmatch/*.json`.

### What the distribution says about the build

- `Cardartlib` / `Cdtools` / `Drawcardlib` are ~60–96% **static debug CRT (LIBCD)** by bytes
  — as the inventory predicted. Cdtools is 96% CRT: its real code is only ~2 KB.
- `Manalink.exe` and `Manalinkinterface.dll` statically link the **release** CRT
  (LIBC/LIBCMT flavor, 49K/20K bytes) — they are *not* /MD like the big EXEs.
- The /MD EXEs contain only `crtexew.obj` startup (`_WinMainCRTStartup`, `__onexit`) plus
  `chkstk`/`llshl` helpers. `Magic.exe`'s startup did not match `crtexew.obj` while
  Shandalar/Deck/Facemaker's did — un-explained, worth a look.
- `Gcconn.dll` (MSVC 5.0) matched only the hand-asm string/int64 primitives, which are
  byte-identical between the 4.2 and 5.0 CRTs.

## 2. Salvage validation — the archived numbers reproduce exactly

Asked-for check: does `salvage/decomp` (the archived first attempt) still build and score
what it recorded?

- **Build:** `salvage/decomp/build.cmd` with `C:\Tools\msvc42\MSDEV` converges (2 NMAKE
  passes), warnings only → `build/shandalar_decomp.dll` + `.pdb`.
- **SHANDALAR:** reccmp 0.1.6 `--paths orig/Shandalar.exe … --no-color` reports
  785 annotated, aggregate **85.94%**. Against the final recorded state
  (`progress/history.jsonl` sha `5f19457`): **784/784 scored functions identical, zero
  drift; byte-exact 194 = 194.**
- **FACEMAKER:** the target's pinned sha256 is `orig/Facemaker_sub.exe` (the `Facemaker/`
  variant). reccmp in `--paths` mode derives the marker module from the *filename*, so the
  file must be presented as `Facemaker.exe` (copy outside the repo). Result: 37/37,
  **88.14%** aggregate — consistent with the archived 87.71% + subsequent commits.
- `progress/coverage.json` is a **stale snapshot** (59 Shandalar / 33 Facemaker functions,
  predates the final commits). Vs that snapshot: Shandalar spot-checks and 26/33 Facemaker
  scores still reproduce exactly; the 7 drifts are all in TUs edited after it was taken.

Verdict: the salvage archive is trustworthy. Its per-function claims are reproducible on
this machine with our toolchain, so `ARCHIVED-CLAUDE.md`'s measured codegen findings apply
directly, as assumed.

## Progress after this session

`6.816% bytes (267,263/3,921,020)` — all `library`; `matching` is still 0 and the
hand-decompilation front is untouched. Next levers, in expected value order:

1. Re-derive the salvage's game-layer functions in `src/` (784 scored, 194 byte-exact in
   the archive; the byte-exact ones are near-free `matching` credit once re-derived).
2. `__CRT_INIT@12`-style startup in the remaining DLLs; the rejected-leads files.
3. Explain `Magic.exe`'s non-matching CRT startup.

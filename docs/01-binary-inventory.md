# Binary Inventory

Everything here was established mechanically from the shipped files on 2026-07-30 and is
reproducible with the tools in `tools/`. Nothing in this document is inferred from memory
or from community lore; anything uncertain is labelled.

Source install: `../Shandalar/MtG_DotP_SotA/`
Pristine copies: `orig/` (hashes in `orig/SHA256SUMS.txt`)

## What this software actually is

MicroProse **Magic: The Gathering** (1997) — the *Shandalar* single-player campaign — together
with the **ManaLink** multiplayer module. The installed tree is a *Duels of the Planeswalkers*
era file set that has been patched by **Mok's unofficial ManaLink update v1.3.2 (2001)**
(`Readme132.txt`).

This matters for a byte-accurate project: **`Shandalar.exe` and `Magic.exe` as shipped are not
pristine MicroProse output.** Their PE timestamps say 1998-03-24, but their filesystem mtimes are
2001-07-11 and `Magic.exe` carries an extra `.mok` section. They are third-party *binary-patched*
1998 binaries. See ADR-0002.

## Executables

| Binary | Size | Linker | PE timestamp | Functions | Code bytes | Exec coverage |
|---|---:|---|---|---:|---:|---:|
| `Shandalar.exe` | 1,753,088 | 4.20 | 1998-03-24 | 2,234 | 1,545,627 | 98.8% |
| `Magic.exe` | 1,604,096 | 4.20 | 1998-03-24 | 1,973 | 1,416,479 | 98.7% |
| `Manalink.exe` | 306,176 | 4.20 | 1998-03-17 | 515 | 168,172 | 93.3% |
| `Facemaker.exe` | 76,288 | 4.20 | 1997-09-02 | 157 | 40,377 | 96.1% |
| `Facemaker/Facemaker.exe` | 89,088 | 4.20 | 1997-08-04 | 170 | 42,299 | 82.3% |
| `Deck.exe` | 16,429 | 4.20 | 1997-03-21 | 9 | 541 | 9.6% |

`Deck.exe`'s low coverage is a Ghidra artefact, not a real finding — it is a tiny launcher whose
body is mostly CRT startup. It also ships a **CodeView NB10 debug record**, the only one in the
whole tree.

## Libraries

| DLL | Size | Linker | PE timestamp | Functions | Code bytes | Exports |
|---|---:|---|---|---:|---:|---:|
| `Deckdll.dll` | 269,824 | 4.20 | 1998-03-24 | 393 | 188,532 | 6 |
| `Gcconn.dll` | 188,416 | **5.00** | 1997-12-19 | 930 | 135,422 | 46 |
| `Drawcardlib.dll` | 178,176 | 4.20 | 1998-03-24 | 359 | 119,803 | 10 |
| `Cardartlib.dll` | 139,264 | 4.20 | 1998-03-24 | 325 | 103,283 | 16 |
| `Cdtools.dll` | 75,264 | 4.20 | 1998-03-24 | 171 | 49,816 | 6 |
| `Statwin.dll` | 55,808 | 4.20 | 1997-05-01 | 155 | 32,843 | 3 |
| `Manalinkinterface.dll` | 46,080 | 4.20 | 1998-03-17 | 137 | 23,075 | 18 |
| `Magvid.dll` | 45,056 | 4.20 | 1997-04-11 | 236 | 36,458 | 20 |
| `Magsnd.dll` | 25,088 | 4.20 | 1997-04-11 | 71 | 19,291 | 27 |

`Msvcrt.dll` and `Msvcrtd.dll` also ship in the tree but are **stock Microsoft redistributables**
(their embedded PDB paths are `E:\8797\vc98\...`). They are out of scope — they are not
MicroProse code.

## Totals

**7,668 non-thunk functions, 3,921,020 bytes of code** across the 15 in-scope modules.

## Compiler identification

Every in-scope module reports **linker version 4.20** except `Gcconn.dll` (5.00).
No module has a Rich header, which is itself consistent with the pre-VC5 toolchain.

- Linker 4.20 → **Microsoft Visual C++ 4.2** (1996)
- Linker 5.00 → **Microsoft Visual C++ 5.0** — `Gcconn.dll` is also the only C++ module
  (`packet.cpp`, `wsstream.cpp`, …) and is the third-party **TEN** (Total Entertainment Network)
  client library, not MicroProse code.

CRT linkage splits cleanly:

- **EXEs** import `MSVCRT.dll` dynamically (`/MD`).
- **`Drawcardlib`, `Cardartlib`, `Cdtools`, `Gcconn`** statically link the **debug** CRT — they
  contain `dbgheap.c`, `dbgrpt.c`, `dbgdel.cpp` assert strings and import no CRT DLL. MicroProse
  shipped debug-CRT builds of these. `Deck.exe`'s PDB path likewise says `\Debug\`.

That static debug CRT is *good news*: a large fraction of those DLLs is stock CRT code that
should be matched by linking the right library, never by hand-decompilation.

## Recovered original source tree

From `assert()` `__FILE__` strings embedded in the binaries. These are literal build-machine
paths, so this is the real 1997 layout, not a guess:

```
D:\NewMagic\
├── sources\
│   ├── sidlib\          Fileio.c  Pcxw.c  lib.c  sprite.c  text.c
│   ├── NedCard\         Catalog.c  Palette.c  haar.c
│   ├── deckexe\         → Deck.exe   (PDB: D:\NEWMAGIC\sources\deckexe\Debug\deck.pdb)
│   └── 1player\FaceMaker\code\   test.c
├── multiplayer\
│   ├── sid\             Ai.c  glue.c  Minit.c  Test.c  iniFile.c
│   ├── NedCard\         Catalog.c  Palette.c  haar.c
│   └── sidlib\          Pcxw.c
├── 1player\FaceMaker\code\       Test.c   (PDB: ...\new___Wi\FaceMaker.pdb)
└── FamiliarWS\          Familiar.c  FamCallbacks.c  FamDialogProc.c  FamImage.c
                         FamMessage.c  FamThreads.c  FamWindowProc.c  MPwinsock.c
```

Reading of the codenames (inference, but well supported):

- **`sidlib`** — the low-level engine library: file I/O, PCX-derived image loading, sprites, text.
- **`NedCard`** — the card art / catalog / palette subsystem.
- **`sid`** — the game itself, including `Ai.c` (the duel AI) and `glue.c`.
- **`FamiliarWS`** — the ManaLink "Familiar" multiplayer client → `Manalink.exe`.

Only files containing `assert()` appear here, so this is a **lower bound** on the real file list.

## Module dependency graph

```
Shandalar.exe ──► DrawCardLib ──► CardArtLib
              ├─► DeckDll ──────► DrawCardLib
              ├─► CdTools
              └─► CardArtLib
                                            (+ KERNEL32 USER32 GDI32 COMDLG32 ADVAPI32
                                               SHELL32 WINMM COMCTL32 MSVFW32 MSVCRT)

Magic.exe     ──► ManalinkInterface ──► (IPC to Manalink.exe)
              ├─► DrawCardLib, DeckDll, CdTools
              └─► VERSION, MSVFW32, …

Manalink.exe  ──► gcconn (TEN network client)
              └─► DPLAYX, WSOCK32, COMCTL32
```

Both EXEs also **export** symbols (11 and 12 respectively) — the DLLs call back into the
executable. `_OpponFace`, `_PlayerFace`, `deck`, `Gold`, `Scards` appear as exports from several
modules, which means they are *shared global variables*, not functions. That is a significant
structural constraint: these modules share mutable global state across the DLL boundary.

## The `MPS_CODE` section

Present in `Shandalar.exe`, `Magic.exe`, `Facemaker.exe`, `Deckdll.dll`, `Drawcardlib.dll`.
Exactly **1,192 bytes virtual / 1,536 raw** in every module. Marked read/write **and not
executable** in the section characteristics (`0xC0000040`), yet it contains genuine x86 —
it begins `55 8B EC 53 56 57 33 C0` (`push ebp; mov ebp,esp; push ebx; push esi; push edi;
xor eax,eax`). Ghidra recovers 6 functions / 1,007 bytes from it.

It compares 16-bit markers `"M0"` / `"M1"` and reads through a pointer at `0x00982540`.
"MPS" = MicroProse. Working hypothesis: an anti-piracy / self-modifying / CD-check stub, related
to `Cdtools.dll`'s `CheckOriginalCD` / `CheckSotaCD` / `CheckDoPCD`. **Not yet confirmed** —
see the open item in `docs/04-plan.md`.

## The `.mok` section

`Magic.exe` only. 4,096 bytes at RVA `0x55A000`, characteristics `0xE0000040` — **read + write +
execute**, which no 1996 MSVC linker would emit for a normal section. 5 functions / 400 bytes.
Contains the literals `User32.dll` and `SystemParametersInfoA`.

This is Mok's v1.3.2 patch payload. It is the concrete proof that the shipped `Magic.exe` is not
a clean build product, and it is why "byte-for-byte reproduce the shipped file" and
"byte-for-byte reproduce MicroProse's 1998 build" are two different goals.

## Shared code between `Shandalar.exe` and `Magic.exe`

Measured two ways with `tools/verify/funcbytes.py`, which masks rel32 branch targets and
in-image absolute addresses using a real x86 length decoder (so only genuine code differences
remain).

| Measure | Result |
|---|---|
| Functions byte-identical after normalisation | **954 functions, 345,124 bytes** (22.3% of Shandalar) |
| 24-byte code windows of Shandalar also present in Magic | **69.1%** |
| 24-byte code windows of Magic also present in Shandalar | **78.8%** |
| Jaccard similarity of code content | **58.2%** |

Read this carefully, because the two numbers say different things:

- Only 22% of *whole functions* are bit-identical.
- But ~70–79% of the actual *code content* is common.

The two executables were built from one source tree in two configurations. Most functions are the
"same function" but differ by a handful of bytes, so exact whole-function matching misses them
while content matching catches them.

**Planning consequence:** the two EXEs together are roughly **1.2× the work of one**, not 2×.
Unique code across both is ≈ **1.84 MB**, against 2.96 MB if they were treated as independent
targets. Decompiling the shared core once serves both.

## Reproducing these facts

```bash
# PE headers, sections, imports/exports, Rich header, debug records
node  <scratch>/peinfo.js orig/*.exe orig/*.dll

# Ghidra headless import + auto-analysis + function inventory
bash  tools/ghidra/run-headless.sh <projname> orig/<binary>

# Normalised function hashing and cross-binary overlap
python tools/verify/funcbytes.py compare \
    orig/Shandalar.exe reports/ghidra/Shandalar.exe.functions.csv \
    orig/Magic.exe     reports/ghidra/Magic.exe.functions.csv
```

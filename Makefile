# NMake makefile for the mtg-re reconstruction.
#
# Built with MSVC 4.2 only (CL 10.20.6166 / LINK 4.20.6164) -- the original
# toolchain. Flags are the measured-correct set from the archived first
# attempt (salvage/ARCHIVED-CLAUDE.md): /MD because the originals import
# msvcrt.dll, /G5 scheduling, /Od for the game layer, /O2 for sidlib,
# /INCREMENTAL:NO because incremental linking routes intra-image calls
# through jump thunks the originals do not have.
#
# The object list and the per-TU compile rules live in Makefile.gen, which is
# GENERATED: the reconstruction lands in waves, and a hand-maintained list
# breaks the build every time a translation unit appears or is still in
# flight. Regenerate it with:
#
#     python tools/progress/genmake.py
#
# genmake.py also decides /O2 vs /Od per TU and derives each TU's header
# prerequisites from its own #include lines.

!IFNDEF MSVC42
MSVC42 = C:\Tools\msvc42\MSDEV
!ENDIF

CC      = $(MSVC42)\BIN\CL.EXE
LINK    = $(MSVC42)\BIN\LINK.EXE

OUTDIR  = build\decomp
TARGET  = $(OUTDIR)\shandalar_recon.dll
PDB     = $(OUTDIR)\shandalar_recon.pdb

CFLAGS    = /nologo /c /O2 /G5 /W3 /Zi /MD /Isrc\include /Fd$(PDB) /D_WINDOWS /DWIN32
CFLAGS_OD = /nologo /c /Od /G5 /W3 /Zi /MD /Isrc\include /Fd$(PDB) /D_WINDOWS /DWIN32
SYSLIBS = kernel32.lib user32.lib gdi32.lib
LFLAGS  = /nologo /DLL /DEBUG /DEBUGTYPE:CV /INCREMENTAL:NO /PDB:$(PDB) /OUT:$(TARGET)

all: $(OUTDIR) $(TARGET)

$(OUTDIR):
	@if not exist $(OUTDIR) mkdir $(OUTDIR)

!INCLUDE Makefile.gen

$(TARGET): $(OBJS)
	$(LINK) $(LFLAGS) $(OBJS) $(SYSLIBS)

clean:
	@if exist $(OUTDIR) rmdir /s /q $(OUTDIR)

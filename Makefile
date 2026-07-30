# NMake makefile for the mtg-re reconstruction.
#
# Built with MSVC 4.2 only (CL 10.20.6166 / LINK 4.20.6164) -- the original
# toolchain. Flags are the measured-correct set from the archived first
# attempt (salvage/ARCHIVED-CLAUDE.md): /MD because the originals import
# msvcrt.dll, /G5 scheduling, /Od for the game layer, /O2 for sidlib.
# /INCREMENTAL:NO because incremental linking routes intra-image calls
# through thunks the originals do not have.

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

OBJS = $(OUTDIR)\cards43.obj $(OUTDIR)\stubs.obj

all: $(OUTDIR) $(TARGET)

$(OUTDIR):
	@if not exist $(OUTDIR) mkdir $(OUTDIR)

$(OUTDIR)\cards43.obj: src\game\cards43.c src\include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\cards43.c

$(OUTDIR)\stubs.obj: src\stubs.c src\include\game\gameslot.h
	$(CC) $(CFLAGS) /Fo$@ src\stubs.c

$(TARGET): $(OBJS)
	$(LINK) $(LFLAGS) $(OBJS) $(SYSLIBS)

clean:
	@if exist $(OUTDIR) rmdir /s /q $(OUTDIR)

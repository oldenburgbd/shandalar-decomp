# NMake makefile for the Shandalar decompilation.
#
# Built with the ORIGINAL toolchain: Microsoft Visual C++ 4.2 (linker 4.20),
# the same compiler that produced Shandalar.exe. Using anything else makes the
# reccmp accuracy numbers meaningless.
#
#   build.cmd            build
#   build.cmd test       also build the differential test driver
#   build.cmd clean      remove intermediates
#
# Flags mirror a 1996 release build: /O2, no frame-pointer omission control
# beyond the default, C89. /Zi + /DEBUGTYPE:CV emit the CodeView PDB that
# reccmp reads to locate our functions.

!IFNDEF MSVC42
MSVC42 = C:\Tools\msvc42\MSDEV
!ENDIF

CC      = $(MSVC42)\BIN\CL.EXE
LINK    = $(MSVC42)\BIN\LINK.EXE

OUTDIR  = build
TARGET  = $(OUTDIR)\shandalar_decomp.dll
PDB     = $(OUTDIR)\shandalar_decomp.pdb

# /MD links the DLL CRT (msvcrt.dll), which is what the original imports. With
# the static CRT the compiler inlines things like _fileno and every CRT call
# diffs against the original's import thunks.
#
# /G5 schedules for the Pentium's dual pipelines, and the original was built
# that way. It is not a tuning preference: it is worth ~7 aggregate points and
# takes several functions to exactly 100%. Its visible effects are stray
# single-byte NOPs used as padding, and loads hoisted into the slot between a
# cmp/test and its branch. /G3, /G4 and the default /GB all produce a
# byte-identical object; only /G5 differs.
CFLAGS  = /nologo /c /O2 /G5 /W3 /Zi /MD /Iinclude /Fd$(PDB) /D_WINDOWS /DWIN32
# Unoptimised variant, for translation units the original built that way.
# /G5 is a measured no-op here (scheduling is an optimiser pass) and is kept
# only so the processor target does not silently vary between our objects.
CFLAGS_OD = /nologo /c /Od /G5 /W3 /Zi /MD /Iinclude /Fd$(PDB) /D_WINDOWS /DWIN32
# The original imports only these; keep the list minimal so an accidental new
# dependency shows up as a link error rather than silently appearing.
SYSLIBS = kernel32.lib user32.lib gdi32.lib

# /INCREMENTAL:NO is essential, not a preference. /DEBUG turns incremental
# linking on by default, which routes every call to another of our functions
# through a jump thunk. The original calls directly, so every such call diffs.
LFLAGS  = /nologo /DLL /DEBUG /DEBUGTYPE:CV /INCREMENTAL:NO /PDB:$(PDB) /OUT:$(TARGET)

COREOBJS = $(OUTDIR)\sprite.obj $(OUTDIR)\lib.obj $(OUTDIR)\text.obj \
       $(OUTDIR)\fileio.obj $(OUTDIR)\pcxw.obj $(OUTDIR)\palette.obj $(OUTDIR)\pred466fe6.obj $(OUTDIR)\slots.obj $(OUTDIR)\eventlog.obj $(OUTDIR)\pred46c6cf.obj $(OUTDIR)\reset.obj $(OUTDIR)\cards43.obj $(OUTDIR)\cards48.obj $(OUTDIR)\cards49.obj $(OUTDIR)\cards4b.obj $(OUTDIR)\helpers40.obj $(OUTDIR)\helpers41.obj $(OUTDIR)\helpers42.obj $(OUTDIR)\helpers44.obj $(OUTDIR)\helpers46.obj $(OUTDIR)\helpers48.obj $(OUTDIR)\helpers49.obj $(OUTDIR)\helpers4a.obj $(OUTDIR)\helpers4b.obj $(OUTDIR)\helpers4c.obj $(OUTDIR)\helpers4d.obj $(OUTDIR)\helpers4e.obj $(OUTDIR)\helpers51.obj $(OUTDIR)\helpers52.obj $(OUTDIR)\helpers55.obj $(OUTDIR)\helpers56.obj \
       $(OUTDIR)\cards4a.obj $(OUTDIR)\cards4c.obj $(OUTDIR)\cards4d.obj $(OUTDIR)\cards4e.obj $(OUTDIR)\cards4f.obj $(OUTDIR)\cards50.obj $(OUTDIR)\cards51.obj $(OUTDIR)\cards52.obj $(OUTDIR)\cards55.obj \
       $(OUTDIR)\deck.obj $(OUTDIR)\exports.obj $(OUTDIR)\game40.obj $(OUTDIR)\game41.obj $(OUTDIR)\game42.obj $(OUTDIR)\game43.obj $(OUTDIR)\game44.obj $(OUTDIR)\game45.obj $(OUTDIR)\game46.obj $(OUTDIR)\game4a.obj $(OUTDIR)\game4b.obj $(OUTDIR)\game4c.obj $(OUTDIR)\game4e.obj $(OUTDIR)\game4f.obj $(OUTDIR)\game50.obj $(OUTDIR)\game52.obj $(OUTDIR)\game53.obj $(OUTDIR)\game54.obj $(OUTDIR)\game55.obj $(OUTDIR)\game56.obj $(OUTDIR)\game57.obj \
       $(OUTDIR)\game47.obj $(OUTDIR)\game48.obj $(OUTDIR)\game51.obj \
       $(OUTDIR)\window.obj

# stubs.obj is kept out of COREOBJS so the host EXE can link its own build of
# src\stubs.c -- the one with SHANDALAR_HOST defined, which drops the MPS_CODE
# stubs in favour of the working ones in src\host\mps.c. Everything else is
# shared between the DLL and the EXE, object for object, so the program that
# runs is built from the same code that gets scored.
OBJS = $(COREOBJS) $(OUTDIR)\stubs.obj

all: $(OUTDIR) $(TARGET)

# The differential test driver. Links the SAME objects the DLL is built from,
# so it exercises the reconstruction rather than a copy of it. Only functions
# that touch no dangerous globals are reachable from it -- see decomp/test.
TESTPROBE = $(OUTDIR)\sprprobe.exe

test: $(OUTDIR) $(TESTPROBE)

# text.obj is in the link line because lib.c's Sid_OpenArchive calls
# Sid_LoadFontFile, which moved out of stubs.c when it was reconstructed.
# Linking an object is not the same as reaching its code: the driver still
# calls only sprite functions, and Sid_CreateScreenPage stays unreachable
# (quirk Q-009). Expect to add objects here as more stubs become real.
$(TESTPROBE): test\sprprobe.c $(OUTDIR)\sprite.obj $(OUTDIR)\lib.obj $(OUTDIR)\text.obj $(OUTDIR)\stubs.obj
	$(CC) $(CFLAGS) /Fo$(OUTDIR)\sprprobe.obj test\sprprobe.c
	$(LINK) /nologo /DEBUG /DEBUGTYPE:CV /INCREMENTAL:NO /PDB:$(OUTDIR)\sprprobe.pdb /OUT:$(TESTPROBE) $(OUTDIR)\sprprobe.obj $(OUTDIR)\sprite.obj $(OUTDIR)\lib.obj $(OUTDIR)\text.obj $(OUTDIR)\stubs.obj $(SYSLIBS)

$(OUTDIR):
	@if not exist $(OUTDIR) mkdir $(OUTDIR)

$(OUTDIR)\sprite.obj: src\sidlib\sprite.c include\sidlib\sidsprite.h include\sidlib\sidcore.h
	$(CC) $(CFLAGS) /Fo$@ src\sidlib\sprite.c

$(OUTDIR)\lib.obj: src\sidlib\lib.c include\sidlib\sidgfx.h include\sidlib\sidcore.h
	$(CC) $(CFLAGS) /Fo$@ src\sidlib\lib.c

$(OUTDIR)\text.obj: src\sidlib\text.c include\sidlib\sidtext.h include\sidlib\sidcore.h
	$(CC) $(CFLAGS) /Fo$@ src\sidlib\text.c

$(OUTDIR)\palette.obj: src\sidlib\palette.c include\sidlib\sidpalette.h
	$(CC) $(CFLAGS) /Fo$@ src\sidlib\palette.c

$(OUTDIR)\pcxw.obj: src\sidlib\pcxw.c include\sidlib\sidfileio.h
	$(CC) $(CFLAGS) /Fo$@ src\sidlib\pcxw.c

$(OUTDIR)\fileio.obj: src\sidlib\fileio.c include\sidlib\sidfileio.h include\sidlib\sidcore.h
	$(CC) $(CFLAGS) /Fo$@ src\sidlib\fileio.c

# The game layer is built /Od, not /O2 -- every large function in it has a
# frame pointer, spills its locals, and uses long-form conditional jumps over
# single-instruction bodies. That is the opposite of sidlib.
$(OUTDIR)\pred46c6cf.obj: src\game\pred46c6cf.c include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\pred46c6cf.c

$(OUTDIR)\helpers40.obj: src\game\helpers40.c include\game\helpers.h include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\helpers40.c

$(OUTDIR)\helpers41.obj: src\game\helpers41.c include\game\helpers.h include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\helpers41.c

$(OUTDIR)\helpers42.obj: src\game\helpers42.c include\game\helpers.h include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\helpers42.c

$(OUTDIR)\helpers44.obj: src\game\helpers44.c include\game\helpers.h include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\helpers44.c

$(OUTDIR)\helpers46.obj: src\game\helpers46.c include\game\helpers.h include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\helpers46.c

$(OUTDIR)\helpers48.obj: src\game\helpers48.c include\game\helpers.h include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\helpers48.c

$(OUTDIR)\helpers49.obj: src\game\helpers49.c include\game\helpers.h include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\helpers49.c

$(OUTDIR)\helpers4a.obj: src\game\helpers4a.c include\game\helpers.h include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\helpers4a.c

$(OUTDIR)\helpers4b.obj: src\game\helpers4b.c include\game\helpers.h include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\helpers4b.c

$(OUTDIR)\helpers4c.obj: src\game\helpers4c.c include\game\helpers.h include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\helpers4c.c

$(OUTDIR)\helpers4d.obj: src\game\helpers4d.c include\game\helpers.h include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\helpers4d.c

$(OUTDIR)\helpers4e.obj: src\game\helpers4e.c include\game\helpers.h include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\helpers4e.c

$(OUTDIR)\helpers51.obj: src\game\helpers51.c include\game\helpers.h include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\helpers51.c

$(OUTDIR)\helpers52.obj: src\game\helpers52.c include\game\helpers.h include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\helpers52.c

$(OUTDIR)\helpers55.obj: src\game\helpers55.c include\game\helpers.h include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\helpers55.c

$(OUTDIR)\helpers56.obj: src\game\helpers56.c include\game\helpers.h include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\helpers56.c

$(OUTDIR)\cards4a.obj: src\game\cards4a.c include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\cards4a.c

$(OUTDIR)\cards4c.obj: src\game\cards4c.c include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\cards4c.c

$(OUTDIR)\cards4d.obj: src\game\cards4d.c include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\cards4d.c

$(OUTDIR)\cards4e.obj: src\game\cards4e.c include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\cards4e.c

$(OUTDIR)\cards4f.obj: src\game\cards4f.c include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\cards4f.c

$(OUTDIR)\cards50.obj: src\game\cards50.c include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\cards50.c

$(OUTDIR)\cards51.obj: src\game\cards51.c include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\cards51.c

$(OUTDIR)\cards52.obj: src\game\cards52.c include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\cards52.c

$(OUTDIR)\cards55.obj: src\game\cards55.c include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\cards55.c

$(OUTDIR)\deck.obj: src\deck\deck.c
	$(CC) $(CFLAGS_OD) /Fo$@ src\deck\deck.c

$(OUTDIR)\exports.obj: src\game\exports.c include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\exports.c

$(OUTDIR)\game40.obj: src\game\game40.c include\game\gamefns.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\game40.c

$(OUTDIR)\game41.obj: src\game\game41.c include\game\gamefns.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\game41.c

$(OUTDIR)\game42.obj: src\game\game42.c include\game\gamefns.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\game42.c

$(OUTDIR)\game43.obj: src\game\game43.c include\game\gamefns.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\game43.c

$(OUTDIR)\game44.obj: src\game\game44.c include\game\gamefns.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\game44.c

$(OUTDIR)\game45.obj: src\game\game45.c include\game\gamefns.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\game45.c

$(OUTDIR)\game46.obj: src\game\game46.c include\game\gamefns.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\game46.c

$(OUTDIR)\game4a.obj: src\game\game4a.c include\game\gamefns.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\game4a.c

$(OUTDIR)\game4b.obj: src\game\game4b.c include\game\gamefns.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\game4b.c

$(OUTDIR)\game4c.obj: src\game\game4c.c include\game\gamefns.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\game4c.c

$(OUTDIR)\game4e.obj: src\game\game4e.c include\game\gamefns.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\game4e.c

$(OUTDIR)\game4f.obj: src\game\game4f.c include\game\gamefns.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\game4f.c

$(OUTDIR)\game50.obj: src\game\game50.c include\game\gamefns.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\game50.c

$(OUTDIR)\game52.obj: src\game\game52.c include\game\gamefns.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\game52.c

$(OUTDIR)\game53.obj: src\game\game53.c include\game\gamefns.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\game53.c

$(OUTDIR)\game54.obj: src\game\game54.c include\game\gamefns.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\game54.c

$(OUTDIR)\game55.obj: src\game\game55.c include\game\gamefns.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\game55.c

$(OUTDIR)\game56.obj: src\game\game56.c include\game\gamefns.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\game56.c

$(OUTDIR)\game57.obj: src\game\game57.c include\game\gamefns.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\game57.c

$(OUTDIR)\game47.obj: src\game\game47.c include\game\gamefns.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\game47.c

$(OUTDIR)\game48.obj: src\game\game48.c include\game\gamefns.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\game48.c

$(OUTDIR)\game51.obj: src\game\game51.c include\game\gamefns.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\game51.c

$(OUTDIR)\cards43.obj: src\game\cards43.c include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\cards43.c

$(OUTDIR)\cards48.obj: src\game\cards48.c include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\cards48.c

$(OUTDIR)\cards49.obj: src\game\cards49.c include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\cards49.c

$(OUTDIR)\cards4b.obj: src\game\cards4b.c include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\cards4b.c

$(OUTDIR)\reset.obj: src\game\reset.c include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\reset.c

$(OUTDIR)\eventlog.obj: src\game\eventlog.c include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\eventlog.c

$(OUTDIR)\slots.obj: src\game\slots.c include\game\gameslot.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\slots.c

$(OUTDIR)\pred466fe6.obj: src\game\pred466fe6.c
	$(CC) $(CFLAGS_OD) /Fo$@ src\game\pred466fe6.c

# window.c is the one translation unit we know is NOT optimised. Building it
# with /O2 like everything else drops it to near-zero similarity.
$(OUTDIR)\window.obj: src\window.c include\sidlib\sidwindow.h
	$(CC) $(CFLAGS_OD) /Fo$@ src\window.c

$(OUTDIR)\stubs.obj: src\stubs.c include\sidlib\sidcore.h include\sidlib\sidgfx.h \
                 include\sidlib\sidtext.h include\sidlib\sidsprite.h \
                 include\sidlib\sidfileio.h include\sidlib\sidwindow.h
	$(CC) $(CFLAGS) /Fo$@ src\stubs.c

$(TARGET): $(OBJS)
	$(LINK) $(LFLAGS) $(OBJS) $(SYSLIBS)

# --- the runnable host -------------------------------------------------------
#
#   build.cmd host      ->  build\Shandalar.exe
#
# A program that actually runs the reconstruction, as opposed to the DLL, which
# exists only to be measured. It links the SAME objects -- COREOBJS, unchanged
# -- so what it draws is produced by the scored code and not by a parallel
# implementation. Two objects differ:
#
#   stubs_host.obj   src\stubs.c with SHANDALAR_HOST, dropping the two MPS_CODE
#                    stubs so mps.obj can supply working ones.
#   hostmps/hostshell  src\host\*.c, which carry no reccmp markers at all.
#
# Deliberately NOT part of `all`. The measured loop is build.cmd with no
# arguments, and this target must never lengthen or perturb it.
#
# Linked without /DEBUG on purpose: the COREOBJS were compiled with /Fd pointing
# at the DLL's PDB, and pulling them into a second image with its own PDB makes
# the linker reconcile two of them for no benefit. Nothing here is scored, so
# there is nothing to reconcile them for.
HOSTEXE  = $(OUTDIR)\Shandalar.exe
HOSTOBJS = $(COREOBJS) $(OUTDIR)\stubs_host.obj $(OUTDIR)\hostmps.obj \
           $(OUTDIR)\hostshell.obj

host: $(OUTDIR) $(HOSTEXE)

$(OUTDIR)\stubs_host.obj: src\stubs.c include\sidlib\sidcore.h include\sidlib\sidgfx.h \
                 include\sidlib\sidtext.h include\sidlib\sidsprite.h \
                 include\sidlib\sidfileio.h include\sidlib\sidwindow.h
	$(CC) $(CFLAGS) /DSHANDALAR_HOST /Fo$@ src\stubs.c

$(OUTDIR)\hostmps.obj: src\host\mps.c include\sidlib\sidfileio.h include\sidlib\sidcore.h
	$(CC) $(CFLAGS) /Fo$@ src\host\mps.c

$(OUTDIR)\hostshell.obj: src\host\shell.c include\sidlib\sidgfx.h \
                 include\sidlib\sidfileio.h include\sidlib\sidcore.h
	$(CC) $(CFLAGS) /Fo$@ src\host\shell.c

$(HOSTEXE): $(HOSTOBJS)
	$(LINK) /nologo /SUBSYSTEM:WINDOWS /INCREMENTAL:NO /MAP:$(OUTDIR)\host.map /OUT:$(HOSTEXE) $(HOSTOBJS) $(SYSLIBS)

clean:
	@if exist $(OUTDIR) rmdir /s /q $(OUTDIR)

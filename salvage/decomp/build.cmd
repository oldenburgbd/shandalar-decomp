@echo off
rem Build the decompilation with the original MSVC 4.2 toolchain.
rem
rem VC++ 4.2 predates the modern vcvars layout and chokes on paths containing
rem spaces, so we set INCLUDE/LIB/PATH here rather than calling VCVARS32.BAT.
rem
rem WHY THIS RUNS NMAKE IN A LOOP
rem
rem NMAKE 1.x snapshots file timestamps when it starts and does not re-stat a
rem target's prerequisites after rebuilding them. So the run that recompiles
rem sprite.obj prints "'build\shandalar_decomp.dll' is up-to-date" and skips
rem the link -- the new object is not in the DLL until you build a SECOND time.
rem
rem This is not occasional. It happens on every source edit, and it silently
rem feeds reccmp the previous build: a change measures as a no-op, and the run
rem after an unrelated change picks up its delta instead. One +35-point change
rem was written off as worthless this way before the cause was found.
rem
rem Reproduce it with:
rem   nmake /f Makefile build\shandalar_decomp.dll     (after touching a .c)
rem
rem So: keep running NMAKE until it invokes no tools. Convergence is bounded by
rem the depth of the dependency graph, which is two (.c -> .obj -> .dll), but
rem the loop tests the real condition rather than assuming that depth.

if "%MSVC42%"=="" set MSVC42=C:\Tools\msvc42\MSDEV

if not exist "%MSVC42%\BIN\CL.EXE" (
    echo ERROR: MSVC 4.2 not found at %%MSVC42%% = %MSVC42%
    echo Set MSVC42 to the MSDEV directory containing BIN\CL.EXE.
    exit /b 1
)

set PATH=%MSVC42%\BIN;%PATH%
set INCLUDE=%MSVC42%\INCLUDE
set LIB=%MSVC42%\LIB

set _log=%TEMP%\shandalar_nmake.log
set _pass=0

:pass
set /a _pass+=1
"%MSVC42%\BIN\NMAKE.EXE" /nologo /f Makefile %* > "%_log%" 2>&1
set _rc=%ERRORLEVEL%
type "%_log%"
if not "%_rc%"=="0" exit /b %_rc%

rem Every command the Makefile issues names a tool under %MSVC42%\BIN. No such
rem line means NMAKE found nothing to do, which is the fixed point we want.
rem
rem The search string must not END in a backslash: findstr parses its argv with
rem C runtime rules, so /C:"\BIN\" arrives as /C:\BIN" -- it silently matches
rem nothing and the loop runs exactly once, which is the bug this loop exists
rem to fix.
findstr /C:"\BIN" "%_log%" >nul
if errorlevel 1 exit /b 0

if %_pass% GEQ 5 (
    echo ERROR: build did not converge after %_pass% NMAKE passes.
    exit /b 1
)
goto pass

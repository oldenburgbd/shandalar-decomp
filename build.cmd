@echo off
rem Build the reconstruction with MSVC 4.2.
rem
rem NMAKE 1.x snapshots timestamps at start and does not re-stat prerequisites
rem after rebuilding them, so the run that recompiles a .c SKIPS the link and
rem reccmp silently scores the previous DLL. Loop until NMAKE invokes no tool.
rem (Kept from the validated archived build -- see salvage/decomp/build.cmd
rem and mtg-re CLAUDE.md. Do not simplify this loop away.)

if "%MSVC42%"=="" set MSVC42=C:\Tools\msvc42\MSDEV

if not exist "%MSVC42%\BIN\CL.EXE" (
    echo ERROR: MSVC 4.2 not found at %%MSVC42%% = %MSVC42%
    exit /b 1
)

set PATH=%MSVC42%\BIN;%PATH%
set INCLUDE=%MSVC42%\INCLUDE
set LIB=%MSVC42%\LIB

set _log=%TEMP%\mtgre_nmake.log
set _pass=0

:pass
set /a _pass+=1
"%MSVC42%\BIN\NMAKE.EXE" /nologo /f Makefile %* > "%_log%" 2>&1
set _rc=%ERRORLEVEL%
type "%_log%"
if not "%_rc%"=="0" exit /b %_rc%

rem A line naming a tool under %MSVC42%\BIN means NMAKE did work this pass.
rem The findstr string must not end in a backslash (argv parsing eats it).
findstr /C:"\BIN" "%_log%" >nul
if errorlevel 1 exit /b 0

if %_pass% GEQ 5 (
    echo ERROR: build did not converge after %_pass% NMAKE passes.
    exit /b 1
)
goto pass

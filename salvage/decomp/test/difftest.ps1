# Differential test: the reconstructed C against the C# oracle, on real game data.
#
# reccmp proves our machine code resembles the original's. It says nothing about
# behaviour, and for every function below 100% that leaves no evidence of
# correctness at all. This is the second axis, and unlike a reccmp score it does
# not drift when an unrelated header changes.
#
#   .\test\difftest.ps1              compare, report disagreements
#   .\test\difftest.ps1 -Verbose     also list every disagreeing frame
#
# Requires SHANDALAR_GAME_DIR (or the default install path below). The game
# directory is only ever read.

param([switch]$VerboseDiff)

$ErrorActionPreference = "Stop"
$decomp = Split-Path -Parent $PSScriptRoot
$repo   = Split-Path -Parent $decomp

if (-not $env:SHANDALAR_GAME_DIR) {
    $env:SHANDALAR_GAME_DIR = "c:\Users\Ben\Desktop\Shandalar\MtG_DotP_SotA"
}
$game = $env:SHANDALAR_GAME_DIR
if (-not (Test-Path $game)) { throw "Game directory not found: $game" }

# --- build the reconstructed C driver -------------------------------------
if (-not $env:MSVC42) { $env:MSVC42 = "C:\Tools\msvc42\MSDEV" }
$env:PATH    = "$env:MSVC42\BIN;$env:PATH"
$env:INCLUDE = "$env:MSVC42\INCLUDE"
$env:LIB     = "$env:MSVC42\LIB"

Push-Location $decomp
& "$env:MSVC42\BIN\NMAKE.EXE" /nologo /f Makefile all test | Out-Null
if ($LASTEXITCODE -ne 0) { Pop-Location; throw "decomp build failed" }

$sprFiles = Get-ChildItem "$game\*.spr" | Sort-Object Name | ForEach-Object { $_.FullName }
$cOut = & ".\build\sprprobe.exe" @sprFiles
Pop-Location

# --- run the C# oracle over the same files --------------------------------
Push-Location $repo
dotnet build src/Shandalar.Cli -c Release --nologo -v q | Out-Null
if ($LASTEXITCODE -ne 0) { Pop-Location; throw "C# build failed" }
$csOut = dotnet run --project src/Shandalar.Cli -c Release --no-build -- sprprobe --all
Pop-Location

# --- compare ---------------------------------------------------------------
function Read-Rows($lines) {
    $h = @{}
    foreach ($ln in $lines) {
        $f = $ln -split '\s+'
        if ($f.Count -ne 10) { continue }
        $h["$($f[0].ToLower())#$($f[1])"] = $f[2..9]
    }
    return $h
}

$c  = Read-Rows $cOut
$cs = Read-Rows $csOut
$cols = @("width", "height", "u1", "u2", "emptyLines", "field14", "minX", "maxX")

$common = $c.Keys | Where-Object { $cs.ContainsKey($_) } | Sort-Object
Write-Host ("frames: C={0}  C#={1}  compared={2}" -f $c.Count, $cs.Count, $common.Count)

$bad = @{}; foreach ($n in $cols) { $bad[$n] = 0 }
$rows = @()
foreach ($k in $common) {
    $mismatch = $false
    for ($i = 0; $i -lt $cols.Count; $i++) {
        if ($c[$k][$i] -ne $cs[$k][$i]) { $bad[$cols[$i]]++; $mismatch = $true }
    }
    if ($mismatch) { $rows += $k }
}

foreach ($n in $cols) {
    $mark = if ($bad[$n] -eq 0) { "ok  " } else { "DIFF" }
    Write-Host ("  {0} {1,-11} {2}" -f $mark, $n, $bad[$n])
}

if ($VerboseDiff -and $rows.Count -gt 0) {
    Write-Host "`ndisagreeing frames:"
    foreach ($k in $rows) {
        Write-Host ("  {0}  C[{1}]  C#[{2}]" -f $k, ($c[$k] -join ","), ($cs[$k] -join ","))
    }
}

if ($rows.Count -eq 0) {
    Write-Host "`nPASS - reconstructed C agrees with the oracle on every frame."
    exit 0
}

# Known, understood disagreements are listed here so a NEW one is visible.
# See docs/formats/spr.md: these three frames carry a field14 smaller than
# height - emptyLines, and the original's extent scanner stops early on them.
$known = @("icons.spr#22", "icons.spr#23", "tsprite2.spr#29")
$new = $rows | Where-Object { $known -notcontains $_ }
if ($new.Count -eq 0) {
    Write-Host ("`nPASS - {0} disagreeing frames, all known and documented." -f $rows.Count)
    exit 0
}

Write-Host ("`nFAIL - {0} NEW disagreeing frames: {1}" -f $new.Count, ($new -join ", "))
exit 1

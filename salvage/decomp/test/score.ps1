# score.ps1 -- build and report reccmp scores.
#
# The measurement loop, wrapped so an experiment is one command instead of
# four. Every claim about a change to decomp/ has to be backed by a number
# (CLAUDE.md rule 7), and the friction of producing that number is what makes
# it tempting to skip.
#
#   .\test\score.ps1                      all functions + aggregate
#   .\test\score.ps1 -Filter Spr_          only matching names, plus aggregate
#   .\test\score.ps1 -Diff 0x57c450        assembly diff for one function
#   .\test\score.ps1 -Save before          write the run to test\.scores\before.txt
#   .\test\score.ps1 -Against before       diff this run's scores against a saved one
#   .\test\score.ps1 -Target FACEMAKER     score against Facemaker.exe instead
#
# -Against is the important one: it prints only what MOVED. MSVC 4.2 re-rolls
# register allocation across a whole translation unit when anything in it
# changes, so a change aimed at one function routinely shifts its neighbours,
# and the aggregate alone hides which way.
#
# -Target picks which linked image to score the SAME objects against. The two
# are not interchangeable: an address passed to -Diff has to belong to the
# target it is being looked up in, and a saved run is stored per target so a
# Facemaker baseline can never be compared against a Shandalar one.

[CmdletBinding()]
param(
    [string]$Filter,
    [string]$Diff,
    [string]$Save,
    [string]$Against,
    [ValidateSet("SHANDALAR", "FACEMAKER")]
    [string]$Target = "SHANDALAR",
    [switch]$NoBuild
)

$ErrorActionPreference = "Stop"
$decomp = Split-Path -Parent $PSScriptRoot
$repo   = Split-Path -Parent $decomp
$reccmp = Join-Path $repo "..\ghidra-work\venv\Scripts\reccmp-reccmp.exe"
$dll    = Join-Path $decomp "build\shandalar_decomp.dll"
$scores = Join-Path $PSScriptRoot ".scores"
# Saved runs are namespaced by target. Comparing a Facemaker run against a
# Shandalar baseline would report every function as moved, which reads as a
# catastrophe rather than as the category error it is.
$prefix = if ($Target -eq "SHANDALAR") { "" } else { "$Target." }

if (-not (Test-Path $reccmp)) { Write-Error "not found: $reccmp"; exit 1 }

Push-Location $decomp
try {
    if (-not $NoBuild) {
        # A silently skipped relink leaves reccmp measuring the PREVIOUS DLL,
        # which has already turned a +35-point change into an apparent no-op.
        # Grepping the build log for LINK.EXE was the obvious check and it was
        # wrong twice over, so compare the artefact's timestamp instead: that
        # cannot be fooled by how NMAKE happens to phrase its output.
        $before = if (Test-Path $dll) { (Get-Item $dll).LastWriteTimeUtc } else { [datetime]::MinValue }

        $build = & cmd /c ".\build.cmd 2>&1"
        if ($LASTEXITCODE -ne 0) {
            $build | ForEach-Object { Write-Host $_ }
            Write-Error "build failed"
            exit 1
        }

        $after = if (Test-Path $dll) { (Get-Item $dll).LastWriteTimeUtc } else { [datetime]::MinValue }
        if ($after -le $before) {
            Write-Host "note: DLL not rewritten -- measuring the previous build" -ForegroundColor DarkYellow
        }
        # C4xxx warnings are worth seeing: dead locals and implicit
        # declarations both move scores, usually downward.
        $warn = $build | Select-String -Pattern "warning C\d+" -SimpleMatch:$false
        if ($warn) { $warn | ForEach-Object { Write-Host $_ -ForegroundColor Yellow } }
    }

    # reccmp writes progress chatter to stderr. Redirecting it inside
    # PowerShell 5.1 wraps every line in an ErrorRecord and trips
    # $ErrorActionPreference = "Stop", so hand the redirect to cmd instead.
    # --target resolves the original binary, the recompiled DLL and the PDB
    # from reccmp-project/build/user.yml, so the two targets cannot drift out
    # of step with each other the way three hand-passed --paths arguments can.
    $cmd = '"{0}" --target {1} --no-color' -f $reccmp, $Target
    if ($Diff) { $cmd += " -v $Diff" }
    $out = & cmd /c "$cmd 2>nul"
} finally {
    Pop-Location
}

if ($Diff) { $out | ForEach-Object { Write-Host $_ }; exit 0 }

# "  Name (0xaddr) is NN.NN% similar to the original" / "... is a stub."
$rows = @{}
foreach ($line in $out) {
    if ($line -match '^\s+(\S+) \((0x[0-9a-f]+)\) is (?:([\d.]+)% similar.*|a (stub)\.)$') {
        $rows[$matches[1]] = [pscustomobject]@{
            Name    = $matches[1]
            Address = $matches[2]
            Score   = if ($matches[4]) { $null } else { [double]$matches[3] }
        }
    }
}

if ($Against) {
    $path = Join-Path $scores "$prefix$Against.txt"
    if (-not (Test-Path $path)) { Write-Error "no saved run: $path"; exit 1 }
    $old = @{}
    foreach ($line in Get-Content $path) {
        $f = $line -split '\t'
        if ($f.Count -ge 2) { $old[$f[0]] = $(if ($f[1] -eq "stub") { $null } else { [double]$f[1] }) }
    }
    $moved = 0
    foreach ($name in ($rows.Keys | Sort-Object)) {
        $now = $rows[$name].Score
        if (-not $old.ContainsKey($name)) {
            Write-Host ("  + {0,-32} {1,7}" -f $name, $(if ($null -eq $now) { "stub" } else { "{0:N2}%" -f $now })) -ForegroundColor Cyan
            $moved++
            continue
        }
        $was = $old[$name]
        if ($now -ne $was) {
            $d = $now - $was
            $c = if ($d -gt 0) { "Green" } else { "Red" }
            Write-Host ("  {0,-32} {1,7:N2}% -> {2,7:N2}%  {3,8:+0.00;-0.00}" -f $name, $was, $now, $d) -ForegroundColor $c
            $moved++
        }
    }
    if ($moved -eq 0) { Write-Host "  no function moved" -ForegroundColor DarkGray }
    Write-Host ""
}

if (-not $Against) {
    foreach ($name in ($rows.Keys | Sort-Object)) {
        if ($Filter -and $name -notmatch $Filter) { continue }
        $s = $rows[$name].Score
        $text = if ($null -eq $s) { "  stub" } else { "{0:N2}%" -f $s }
        $c = if ($null -eq $s) { "DarkGray" }
             elseif ($s -eq 100) { "Green" }
             elseif ($s -ge 90) { "White" }
             elseif ($s -ge 60) { "Yellow" }
             else { "Red" }
        Write-Host ("  {0,-32} {1,10}  {2}" -f $name, $text, $rows[$name].Address) -ForegroundColor $c
    }
}

$out | Where-Object { $_ -match '^(Implemented|Accuracy|Progress):' } | ForEach-Object { Write-Host $_ }

$exact = ($rows.Values | Where-Object { $_.Score -eq 100 }).Count
$scored = ($rows.Values | Where-Object { $null -ne $_.Score }).Count
Write-Host ("Byte-exact:   {0} / {1}" -f $exact, $scored)

if ($Save) {
    if (-not (Test-Path $scores)) { New-Item -ItemType Directory -Path $scores | Out-Null }
    $lines = foreach ($name in ($rows.Keys | Sort-Object)) {
        $s = $rows[$name].Score
        "{0}`t{1}" -f $name, $(if ($null -eq $s) { "stub" } else { $s })
    }
    $lines | Out-File -FilePath (Join-Path $scores "$prefix$Save.txt") -Encoding utf8
    Write-Host "saved -> test\.scores\$prefix$Save.txt" -ForegroundColor DarkGray
}

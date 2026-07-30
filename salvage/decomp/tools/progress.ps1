# progress.ps1 -- coverage report for the Shandalar decompilation.
#
#   .\tools\progress.ps1              measure, refresh coverage, re-render
#   .\tools\progress.ps1 -Render      re-render from stored data, no build
#   .\tools\progress.ps1 -Inventory   re-scan the binaries (slow; Ghidra)
#   .\tools\progress.ps1 -Open        open the report
#
# EVERY NUMBER IS MATCHED BYTES OVER TOTAL BYTES, at whatever scope you are
# looking at. That is the one metric that cannot mislead: it only moves up, and
# it moves in proportion to the work.
#
# The report deliberately does NOT show accuracy averaged over annotated
# functions. That number falls whenever a new function is reconstructed below
# the current average -- so it punishes exactly the work that makes progress,
# and six of the first twenty-six commits show a dip for that reason alone.
# It is still measured per function (a function's own accuracy is meaningful);
# it is just never aggregated.
#
# Scope is a drill-down: the whole product -> one binary -> one function.
#
# progress/history.jsonl is still written, and -Backfill still populates it. It
# is a data record, not a report input: nothing in the page reads it.

[CmdletBinding()]
param(
    [switch]$Render,
    [switch]$Backfill,
    [switch]$Inventory,
    [switch]$Open,
    [switch]$NoBuild
)

$ErrorActionPreference = "Stop"
$decomp  = Split-Path -Parent $PSScriptRoot
$repo    = Split-Path -Parent $decomp
$orig    = Join-Path $repo "..\ghidra-work\bin\Shandalar.exe"
$reccmp  = Join-Path $repo "..\ghidra-work\venv\Scripts\reccmp-reccmp.exe"
$progDir = Join-Path $decomp "progress"
$histPath = Join-Path $progDir "history.jsonl"
$htmlPath = Join-Path $progDir "index.html"
$invPath  = Join-Path $progDir "inventory.json"
$ghScripts = Join-Path $repo "..\ghidra-work\scripts"
$ghPython  = Join-Path $repo "..\ghidra-work\venv\Scripts\python.exe"

if (-not (Test-Path $progDir)) { New-Item -ItemType Directory -Path $progDir | Out-Null }

function Invoke-GitQuiet {
    param([string[]]$Arguments)
    $line = ($Arguments | ForEach-Object { if ($_ -match '\s') { """$_""" } else { $_ } }) -join ' '
    & cmd /c "git -C ""$repo"" $line >nul 2>nul"
    return $LASTEXITCODE
}

# ---------------------------------------------------------------- measurement

function Get-FunctionFiles {
    param([string]$Root)
    $map = @{}
    $srcDir = Join-Path $Root "src"
    if (-not (Test-Path $srcDir)) { return $map }
    foreach ($f in Get-ChildItem $srcDir -Recurse -Filter *.c) {
        $lines = Get-Content $f.FullName
        foreach ($l in $lines) {
            if ($l -match '^\s*//\s*(FUNCTION|STUB):\s*SHANDALAR\s+(0x[0-9a-fA-F]+)') {
                $map[("0x{0:x}" -f [Convert]::ToInt64($matches[2], 16))] = $f.Name
            }
        }
    }
    return $map
}

function Invoke-Build {
    param([string]$Dir)
    for ($pass = 1; $pass -le 4; $pass++) {
        Push-Location $Dir
        try { $out = & cmd /c ".\build.cmd 2>&1"; $rc = $LASTEXITCODE }
        finally { Pop-Location }
        if ($rc -ne 0) { return @{ ok = $false; log = ($out -join "`n") } }
        if (-not ($out -match "\\BIN\\")) { return @{ ok = $true } }
    }
    return @{ ok = $true }
}

function Measure-Tree {
    param([string]$Dir, [switch]$SkipBuild)

    $dll = Join-Path $Dir "build\shandalar_decomp.dll"
    $pdb = Join-Path $Dir "build\shandalar_decomp.pdb"

    if (-not $SkipBuild) {
        $b = Invoke-Build -Dir $Dir
        if (-not $b.ok) { return $null }
    }
    if (-not (Test-Path $dll)) { return $null }

    $cmd = '"{0}" --paths "{1}" "{2}" "{3}" . --no-color' -f $reccmp, $orig, $dll, $pdb
    Push-Location $Dir
    try { $out = & cmd /c "$cmd 2>nul" } finally { Pop-Location }

    $files = Get-FunctionFiles -Root $Dir
    $fns = @{}
    foreach ($line in $out) {
        if ($line -match '^\s+(\S+) \((0x[0-9a-f]+)\) is (?:([\d.]+)% similar.*|a (stub)\.)$') {
            $addr = $matches[2]
            $fns[$matches[1]] = [ordered]@{
                addr  = $addr
                score = $(if ($matches[4]) { $null } else { [double]$matches[3] })
                file  = $(if ($files.ContainsKey($addr)) { $files[$addr] } else { "?" })
            }
        }
    }
    if ($fns.Count -eq 0) { return $null }
    $scored = @($fns.Values | Where-Object { $null -ne $_.score })
    return [ordered]@{
        scored    = $scored.Count
        total     = $fns.Count
        byteExact = @($scored | Where-Object { $_.score -eq 100 }).Count
        functions = $fns
    }
}

# ------------------------------------------------------------------ inventory

function Build-Inventory {
    param($Measured)

    # funcs.py is the only slow step and its answer never changes -- it reads a
    # binary that is read-only by policy. Cache it next to the report and
    # regenerate only on -Inventory, so refreshing coverage after every commit
    # costs a fraction of a second instead of a Ghidra headless run.
    $csv = Join-Path $progDir "funcs.csv"
    if ($Inventory -or -not (Test-Path $csv)) {
        Write-Host "dumping Shandalar functions (Ghidra headless, ~2 min)..." -ForegroundColor Cyan
        & cmd /c """$ghPython"" ""$(Join-Path $ghScripts 'funcs.py')"" ""$csv"" 2>nul" |
            ForEach-Object { Write-Host "  $_" -ForegroundColor DarkGray }
        if (-not (Test-Path $csv)) { Write-Error "funcs.py produced nothing"; return }
    }

    $sizes = @{}
    foreach ($r in (Import-Csv $csv)) { $sizes[[Convert]::ToInt64($r.address, 16)] = [int]$r.size }

    # Ghidra only makes a function where something calls it, so anything reached
    # solely through a function pointer is missing from the inventory -- and
    # without a size its bytes would count towards nothing at all. Bound those
    # by the next known start and trim the int3 padding.
    $missing = @()
    foreach ($n in $Measured.functions.Keys) {
        if ($n -eq '__DllMainCRTStartup@12') { continue }
        $a = [Convert]::ToInt64($Measured.functions[$n].addr, 16)
        if (-not $sizes.ContainsKey($a)) { $missing += ("0x{0:x}" -f $a) }
    }
    if ($missing.Count) {
        $args = $missing -join ' '
        $out = & cmd /c """$ghPython"" ""$(Join-Path $ghScripts 'xdis.py')"" ""$orig"" --extent ""$csv"" $args 2>nul"
        $sized = 0
        foreach ($line in $out) {
            $f = $line -split ','
            if ($f.Count -eq 2 -and [int]$f[1] -gt 0) {
                $sizes[[Convert]::ToInt64($f[0], 16)] = [int]$f[1]
                $sized++
            }
        }
        # The rest are addresses outside .text -- the MPS_CODE stubs -- which
        # have no extent to compute and are meant to be left out.
        Write-Host ("sized {0} of {1} function(s) Ghidra did not identify" -f $sized, $missing.Count) -ForegroundColor DarkGray
    }

    # The annotated set, with sizes and current accuracy, is what coverage.py
    # projects into the other binaries.
    $annCsv = Join-Path $progDir "annotated.csv"
    $rows = @()
    foreach ($n in $Measured.functions.Keys) {
        $a = [Convert]::ToInt64($Measured.functions[$n].addr, 16)
        if (-not $sizes.ContainsKey($a)) { continue }
        if ($n -eq '__DllMainCRTStartup@12') { continue }
        $s = $Measured.functions[$n].score
        $rows += [pscustomobject]@{
            name = $n; address = ("0x{0:x}" -f $a); size = $sizes[$a]
            score = $(if ($null -eq $s) { "" } else { $s })
        }
    }
    $rows | Export-Csv $annCsv -NoTypeInformation

    Write-Host "matching into every shipped binary..." -ForegroundColor Cyan
    $gameDir = $env:SHANDALAR_GAME_DIR
    if (-not $gameDir) { $gameDir = "c:\Users\Ben\Desktop\Shandalar\MtG_DotP_SotA" }
    # Deduplicate by CONTENT, not by name. The install ships the same DLL in
    # several directories, which must be counted once -- but it also ships two
    # genuinely different Facemaker.exe builds, one in the root and one in
    # Facemaker\, and grouping by filename dropped the smaller one from the
    # denominator entirely.
    $bins = @(Get-ChildItem $gameDir -Include *.exe, *.dll -Recurse |
              Where-Object { $_.Name -notmatch '^(Msvcrt|Msvcrtd)\.dll$' } |
              Group-Object { (Get-FileHash $_.FullName -Algorithm SHA256).Hash } |
              ForEach-Object { ($_.Group | Sort-Object FullName)[0].FullName })

    $q = '"'
    $binArgs = ($bins | ForEach-Object { $q + $_ + $q }) -join ' '
    $covScript = Join-Path $ghScripts 'coverage.py'
    $covCmd = '"{0}" "{1}" "{2}" "{3}" {4}' -f $ghPython, $covScript, $orig, $annCsv, $binArgs
    $covJson = & cmd /c "$covCmd 2>nul"
    $cov = ($covJson -join "") | ConvertFrom-Json

    # Shandalar gets its complete function list; every other binary carries only
    # the functions we could prove are byte-identical to our reference copy. The
    # rest of each is a single "not yet inventoried" remainder, which is honest:
    # we have its size from the PE header and nothing else.
    $scoreByAddr = @{}
    foreach ($n in $Measured.functions.Keys) {
        $scoreByAddr[[Convert]::ToInt64($Measured.functions[$n].addr, 16)] = @{
            name = $n; score = $Measured.functions[$n].score
        }
    }

    # Two binaries with the same basename need distinguishable labels or the
    # report shows two rows called "Facemaker.exe" and no way to tell which is
    # which.
    $dupNames = @($cov.binaries | Group-Object name | Where-Object { $_.Count -gt 1 } |
                  ForEach-Object { $_.Name })

    $binaries = @()
    foreach ($b in $cov.binaries) {
        $label = $b.name
        if ($dupNames -contains $b.name) { $label = "$($b.dir)\$($b.name)" }
        $entry = [ordered]@{
            name = $label; linker = $b.linker; textBytes = $b.textBytes
            isRef = [bool]$b.isRef; inventory = "matched-only"; fns = @()
        }
        if ($b.isRef) {
            $entry.inventory = "full"
            $list = New-Object System.Collections.Generic.List[object]
            foreach ($r in (Import-Csv $csv)) {
                $a = [Convert]::ToInt64($r.address, 16)
                $sz = [int]$r.size
                if ($sz -le 0) { continue }
                $sc = -1.0; $nm = $null
                if ($scoreByAddr.ContainsKey($a)) {
                    $nm = $scoreByAddr[$a].name
                    $sc = $(if ($null -eq $scoreByAddr[$a].score) { -2.0 } else { [double]$scoreByAddr[$a].score })
                }
                $list.Add(@($a, $sz, $sc, $nm))
            }
            $entry.fns = $list.ToArray()
        }
        else {
            $list = New-Object System.Collections.Generic.List[object]
            foreach ($f in $b.functions) { $list.Add(@([int64]$f.va, [int]$f.size, [double]$f.score, $f.name)) }
            $entry.fns = $list.ToArray()
        }
        $binaries += $entry
    }

    $inv = [ordered]@{
        generated  = (Get-Date).ToString("s")
        textBytes  = $cov.gameTextBytes
        binaryCount = $cov.gameCount
        binaries   = $binaries
    }
    [System.IO.File]::WriteAllText($invPath, ($inv | ConvertTo-Json -Depth 8 -Compress),
                                   (New-Object System.Text.UTF8Encoding($false)))
    Write-Host ("wrote {0} -- {1} binaries, {2:N0} bytes" -f $invPath, $cov.gameCount, $cov.gameTextBytes) -ForegroundColor Green
}

# ---------------------------------------------------------------- history I/O

function Read-History {
    if (-not (Test-Path $histPath)) { return @() }
    $rows = @()
    foreach ($line in Get-Content $histPath) { if ($line.Trim()) { $rows += ($line | ConvertFrom-Json) } }
    return $rows
}

function Write-History {
    param($Rows)
    ($Rows | ForEach-Object { $_ | ConvertTo-Json -Depth 6 -Compress }) |
        Out-File -FilePath $histPath -Encoding utf8
}

# ---------------------------------------------------------------- HTML render

function HtmlEncode { param([string]$s) return [System.Net.WebUtility]::HtmlEncode($s) }

function Render-Report {
    if (-not (Test-Path $invPath)) { Write-Error "no inventory; run -Inventory first"; return }
    $inv = Get-Content $invPath -Raw | ConvertFrom-Json
    $inv2 = [Globalization.CultureInfo]::InvariantCulture

    # Everything the page needs, and nothing it does not.
    $payload = $inv | ConvertTo-Json -Depth 8 -Compress
    $generated = (Get-Date).ToString("yyyy-MM-dd HH:mm")
    $mb = ($inv.textBytes / 1048576.0).ToString('0.00', $inv2)

    $html = @"
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Shandalar decompilation &mdash; coverage</title>
<style>
  :root {
    --bg:#0d1117; --panel:#161b22; --line:#26303d; --fg:#e6edf3; --dim:#8b949e;
    --done:#3fb950; --high:#58a6ff; --mid:#d29922; --low:#f85149; --stub:#6e7681;
    --tm-none:#212b38; --tm-edge:#0d1117;
  }
  @media (prefers-color-scheme: light) {
    :root { --bg:#ffffff; --panel:#f6f8fa; --line:#d0d7de; --fg:#1f2328; --dim:#636c76;
            --done:#1a7f37; --high:#0969da; --mid:#9a6700; --low:#cf222e; --stub:#8c959f;
            --tm-none:#c9d1d9; --tm-edge:#ffffff; }
  }
  * { box-sizing:border-box; }
  body { margin:0; background:var(--bg); color:var(--fg);
         font:14px/1.5 -apple-system,BlinkMacSystemFont,"Segoe UI",Helvetica,Arial,sans-serif; }
  .wrap { max-width:1080px; margin:0 auto; padding:26px 20px 64px; }
  h1 { font-size:21px; margin:0 0 2px; font-weight:600; }
  .sub { color:var(--dim); font-size:13px; margin:0 0 20px; }
  .mono { font-family:ui-monospace,SFMono-Regular,Menlo,Consolas,monospace; font-size:12.5px; }
  .dim { color:var(--dim); }

  .crumbs { display:flex; align-items:center; gap:7px; flex-wrap:wrap; margin:0 0 14px; font-size:13px; }
  .crumbs button { background:none; border:none; color:var(--high); cursor:pointer;
                   font:inherit; padding:2px 3px; border-radius:4px; }
  .crumbs button:hover { background:var(--panel); text-decoration:underline; }
  .crumbs span.sep { color:var(--dim); }
  .crumbs span.cur { color:var(--fg); font-weight:600; }

  .hero { background:var(--panel); border:1px solid var(--line); border-radius:8px; padding:20px 22px; }
  .heroTop { display:flex; align-items:baseline; gap:12px; flex-wrap:wrap; }
  .big { font-size:40px; font-weight:600; letter-spacing:-.02em; line-height:1; }
  .stack { display:flex; height:16px; border-radius:5px; overflow:hidden; background:var(--line); margin-top:16px; }
  .stack i { display:block; height:100%; }
  .s-done { background:var(--done); }
  .s-part { background:var(--high); background-image:repeating-linear-gradient(135deg,
            rgba(255,255,255,.28) 0 5px, rgba(255,255,255,0) 5px 10px); }
  .s-none { background:var(--tm-none); }
  .key { display:flex; gap:16px; align-items:center; margin:11px 0 0; color:var(--dim);
         font-size:12.5px; flex-wrap:wrap; }
  .key i { display:inline-block; width:11px; height:11px; border-radius:3px; margin-right:6px;
           vertical-align:-1px; }

  h2 { font-size:15px; margin:30px 0 10px; font-weight:600; }
  .panel { background:var(--panel); border:1px solid var(--line); border-radius:8px; padding:14px; }
  #tmWrap { position:relative; }
  #tm { width:100%; display:block; border-radius:5px; background:var(--bg); }
  #tm rect.f { stroke:var(--tm-edge); stroke-width:.5; }
  #tm rect.f.clickable { cursor:pointer; }
  #tm rect.hl { fill:none; stroke:var(--fg); stroke-width:2.5; pointer-events:none; }
  #tmTip { position:absolute; pointer-events:none; background:var(--fg); color:var(--bg);
           padding:5px 9px; border-radius:5px; font-size:12px; white-space:nowrap;
           transform:translate(-50%,-135%); box-shadow:0 3px 12px rgba(0,0,0,.35); z-index:5; }

  .tableWrap { overflow-x:auto; border:1px solid var(--line); border-radius:8px; }
  table { border-collapse:collapse; width:100%; min-width:620px; background:var(--panel); }
  th,td { text-align:left; padding:7px 12px; border-bottom:1px solid var(--line); vertical-align:middle; }
  th { color:var(--dim); font-weight:600; font-size:12px; text-transform:uppercase; letter-spacing:.03em; }
  tr:last-child td { border-bottom:none; }
  tr.drill { cursor:pointer; }
  tr.drill:hover td { background:rgba(128,128,128,.09); }
  .num { text-align:right; white-space:nowrap; }
  .barcell { width:30%; min-width:110px; }
  .bar { position:relative; height:9px; background:var(--line); border-radius:5px; overflow:hidden; }
  .bar i { display:block; height:100%; background:var(--high); border-radius:5px; }
  .bar i.done { background:var(--done); }
  .pct { font-family:ui-monospace,Menlo,Consolas,monospace; font-size:12.5px; }
  .tag { font-size:11px; padding:1px 6px; border-radius:10px; margin-left:6px;
         color:var(--dim); background:rgba(128,128,128,.16); }
  .note { color:var(--dim); font-size:12.5px; margin:9px 2px 0; }
  .empty { color:var(--dim); padding:14px; }
</style>
</head>
<body>
<div class="wrap">

  <h1>Shandalar decompilation</h1>
  <p class="sub">Magic: The Gathering &mdash; Duels of the Planeswalkers v3.0 (MicroProse, 1998)
     &middot; MSVC 4.2 + reccmp &middot; generated $generated</p>

  <div class="crumbs" id="crumbs"></div>

  <div class="hero">
    <div class="heroTop">
      <div class="big" id="heroPct">&mdash;</div>
      <div class="dim" id="heroSub"></div>
    </div>
    <div class="stack" id="heroBar"></div>
    <div class="key">
      <span><i class="s-done"></i>matching</span>
      <span><i class="s-part"></i>reconstructed, not yet exact</span>
      <span><i class="s-none"></i>not started</span>
    </div>
  </div>

  <h2 id="mapTitle">Coverage map</h2>
  <div class="panel">
    <div id="tmWrap"><svg id="tm" role="img" aria-label="Coverage treemap"></svg>
      <div id="tmTip" hidden></div>
    </div>
  </div>
  <p class="note" id="mapNote"></p>

  <h2 id="listTitle">Breakdown</h2>
  <div id="listWrap"></div>

  <p class="note">Every percentage is <b>matching bytes over total bytes</b> at the scope shown, so it
  only ever moves up. A function's percentage is reccmp's instruction-level comparison of our
  recompiled code against the original's; a byte is counted as matching in proportion to that.
  Accuracy averaged over annotated functions is deliberately absent &mdash; it falls whenever a new
  function is reconstructed below the current average, which punishes the work that makes progress.</p>

</div>
<script id="invdata" type="application/json">$payload</script>
</body>
</html>
"@

    $script = @'
<script>
(function () {
  var DATA = JSON.parse(document.getElementById("invdata").textContent);
  var NS = "http://www.w3.org/2000/svg";
  var svg = document.getElementById("tm"), tip = document.getElementById("tmTip");
  var CSS = {};
  function css(n) {
    if (!(n in CSS)) CSS[n] = getComputedStyle(document.documentElement).getPropertyValue(n).trim();
    return CSS[n];
  }
  function fmtBytes(n) {
    if (n >= 1048576) return (n / 1048576).toFixed(2) + " MB";
    if (n >= 1024) return (n / 1024).toFixed(1) + " kB";
    return Math.round(n) + " B";
  }
  function pct(m, t) { return t > 0 ? (100 * m / t) : 0; }

  // score: >=0 accuracy, -1 not started, -2 stubbed (annotated, not written)
  //
  // Two different scales, on purpose. A function is either being worked on or
  // it is not, so a 40% one should read as clearly partial -- hence the floor.
  // A whole binary at 0.03% must NOT read as a quarter done, so its scale has
  // no floor and is square-rooted: 0% is exactly the untouched colour, and
  // small-but-real coverage is still visible without being overstated.
  function colour(s, floor) {
    if (s === -2) return css("--stub");
    if (s < 0) return css("--tm-none");
    if (s >= 100) return css("--done");
    if (s <= 0) return css("--tm-none");
    var t = floor ? Math.max(0.25, s / 100) : Math.sqrt(s / 100);
    return "color-mix(in srgb, " + css("--high") + " " + Math.round(t * 100) + "%, " + css("--tm-none") + ")";
  }

  // ---- scope model -------------------------------------------------------
  // level 0: the product, children are binaries
  // level 1: one binary, children are functions
  var scope = { level: 0, bin: null };

  function binStats(b) {
    var matched = 0, touched = 0;
    for (var i = 0; i < b.fns.length; i++) {
      var s = b.fns[i][2];
      if (s >= 0) { matched += b.fns[i][1] * s / 100; touched += b.fns[i][1]; }
    }
    return { matched: matched, touched: touched, total: b.textBytes };
  }

  function currentStats() {
    if (scope.level === 0) {
      var m = 0, t = 0, tot = 0;
      DATA.binaries.forEach(function (b) {
        var s = binStats(b); m += s.matched; t += s.touched; tot += b.textBytes;
      });
      return { matched: m, touched: t, total: tot };
    }
    return binStats(scope.bin);
  }

  // Children as uniform tiles: {label, value, matched, touched, score, drill, meta}
  function children() {
    if (scope.level === 0) {
      return DATA.binaries.map(function (b) {
        var s = binStats(b);
        return { label: b.name, value: b.textBytes, matched: s.matched, touched: s.touched,
                 score: null, drill: b, meta: "linker " + b.linker, bin: b };
      }).sort(function (a, b) { return b.value - a.value; });
    }
    var b = scope.bin;
    var kids = b.fns.map(function (f) {
      return { label: f[3] || ("0x" + f[0].toString(16).padStart(8, "0")),
               value: f[1],
               matched: f[2] >= 0 ? f[1] * f[2] / 100 : 0,
               touched: f[2] >= 0 ? f[1] : 0,
               score: f[2], drill: null,
               meta: "0x" + f[0].toString(16).padStart(8, "0") };
    });
    // Binaries we have not inventoried contribute one honest remainder block.
    if (b.inventory !== "full") {
      var known = kids.reduce(function (a, k) { return a + k.value; }, 0);
      var rest = b.textBytes - known;
      if (rest > 0) {
        kids.push({ label: "not yet inventoried", value: rest, matched: 0, touched: 0,
                    score: -1, drill: null, meta: "no function list extracted for this binary" });
      }
    }
    return kids.sort(function (a, b) { return b.value - a.value; });
  }

  // ---- squarified treemap ------------------------------------------------
  function squarify(items, x, y, w, h, out) {
    var total = 0, i;
    for (i = 0; i < items.length; i++) total += items[i].value;
    if (total <= 0 || w <= 0 || h <= 0) return;
    var scale = (w * h) / total, idx = 0;
    while (idx < items.length) {
      var vertical = w >= h, side = vertical ? h : w;
      var row = [], rowSum = 0, best = Infinity;
      while (idx + row.length < items.length) {
        var cand = items[idx + row.length].value * scale;
        var sum2 = rowSum + cand, thick = sum2 / side, worst = 0;
        var vals = row.concat([cand]);
        for (i = 0; i < vals.length; i++) {
          var len = vals[i] / thick;
          worst = Math.max(worst, Math.max(thick / len, len / thick));
        }
        if (row.length && worst > best) break;
        row.push(cand); rowSum = sum2; best = worst;
      }
      var thickness = rowSum / side, off = 0;
      for (i = 0; i < row.length; i++) {
        var l = row[i] / thickness;
        if (vertical) out(items[idx + i], x, y + off, thickness, l);
        else out(items[idx + i], x + off, y, l, thickness);
        off += l;
      }
      if (vertical) { x += thickness; w -= thickness; } else { y += thickness; h -= thickness; }
      idx += row.length;
    }
  }

  var hl = null;
  function clearHl() { if (hl) { hl.remove(); hl = null; } tip.hidden = true; }

  function drawMap(kids) {
    var W = svg.clientWidth || 900, H = Math.round(W * (scope.level === 0 ? 0.42 : 0.52));
    svg.setAttribute("viewBox", "0 0 " + W + " " + H);
    svg.setAttribute("height", H);
    while (svg.firstChild) svg.removeChild(svg.firstChild);
    hl = null;

    squarify(kids, 0, 0, W, H, function (k, x, y, w, h) {
      if (w < 0.4 || h < 0.4) return;
      var r = document.createElementNS(NS, "rect");
      r.setAttribute("class", "f" + (k.drill ? " clickable" : ""));
      r.setAttribute("x", x.toFixed(2)); r.setAttribute("y", y.toFixed(2));
      r.setAttribute("width", w.toFixed(2)); r.setAttribute("height", h.toFixed(2));
      // At product level a binary is shaded by how much of it we match; at
      // function level by that function's own accuracy.
      r.setAttribute("fill", scope.level === 0
        ? colour(k.touched > 0 ? pct(k.matched, k.value) : -1, false)
        : colour(k.score, true));
      r.__k = k;
      svg.appendChild(r);
    });
  }

  svg.addEventListener("mousemove", function (ev) {
    var t = ev.target;
    if (!t.__k) { clearHl(); return; }
    var k = t.__k;
    if (hl) hl.remove();
    hl = document.createElementNS(NS, "rect");
    hl.setAttribute("class", "hl");
    ["x", "y", "width", "height"].forEach(function (a) { hl.setAttribute(a, t.getAttribute(a)); });
    svg.appendChild(hl);
    var state;
    if (scope.level === 0) state = pct(k.matched, k.value).toFixed(2) + "% matching";
    else if (k.score === -2) state = "stubbed";
    else if (k.score < 0) state = "not started";
    else state = k.score.toFixed(2) + "% matching";
    tip.textContent = k.label + " \u2022 " + fmtBytes(k.value) + " \u2022 " + state;
    var box = svg.getBoundingClientRect();
    tip.style.left = (ev.clientX - box.left) + "px";
    tip.style.top = (ev.clientY - box.top) + "px";
    tip.hidden = false;
  });
  svg.addEventListener("mouseleave", clearHl);
  svg.addEventListener("click", function (ev) {
    if (ev.target.__k && ev.target.__k.drill) { scope = { level: 1, bin: ev.target.__k.drill }; render(); scrollTo(0, 0); }
  });

  // ---- tables ------------------------------------------------------------
  function bar(p, done) {
    return '<div class="bar"><i class="' + (done ? "done" : "") +
           '" style="width:' + Math.max(0, Math.min(100, p)).toFixed(3) + '%"></i></div>';
  }

  function renderList(kids) {
    var wrap = document.getElementById("listWrap");
    if (!kids.length) { wrap.innerHTML = '<div class="panel empty">Nothing to show.</div>'; return; }

    var head, rows = "", shown = kids.length, hidden = 0;
    if (scope.level === 0) {
      head = "<tr><th>Binary</th><th>Linker</th><th class='num'>Code</th><th>Matching</th><th class='num'>%</th></tr>";
      kids.forEach(function (k, i) {
        var p = pct(k.matched, k.value);
        rows += "<tr class='drill' data-i='" + i + "'>" +
          "<td class='mono'>" + esc(k.label) + (k.bin && k.bin.isRef ? "<span class='tag'>reference</span>" : "") + "</td>" +
          "<td class='dim mono'>" + esc(k.bin ? k.bin.linker : "") + "</td>" +
          "<td class='num'>" + fmtBytes(k.value) + "</td>" +
          "<td class='barcell'>" + bar(p, p >= 100) + "</td>" +
          "<td class='num pct'>" + p.toFixed(2) + "%</td></tr>";
      });
    } else {
      head = "<tr><th>Function</th><th>Address</th><th class='num'>Size</th><th>Matching</th><th class='num'>%</th></tr>";
      // Worst first among what we have touched -- that is the working queue --
      // then stubs, then untouched largest first, which is the opportunity
      // list. Capped: a binary with 2,210 functions makes an unreadable page,
      // and the tail is all "not started, 11 bytes".
      var ordered = kids.slice().sort(function (a, b) {
        var ra = a.score >= 0 ? a.score : 1000 + (a.score === -2 ? 0 : 1);
        var rb = b.score >= 0 ? b.score : 1000 + (b.score === -2 ? 0 : 1);
        if (ra !== rb) return ra - rb;
        return b.value - a.value;
      });
      var CAP = 200;
      shown = Math.min(CAP, ordered.length);
      hidden = ordered.length - shown;
      ordered.slice(0, CAP).forEach(function (k) {
        var state = k.score === -2 ? "stub" : (k.score < 0 ? "&mdash;" : k.score.toFixed(2) + "%");
        rows += "<tr>" +
          "<td class='mono'>" + esc(k.label) + "</td>" +
          "<td class='mono dim'>" + esc(k.meta) + "</td>" +
          "<td class='num'>" + fmtBytes(k.value) + "</td>" +
          "<td class='barcell'>" + (k.score >= 0 ? bar(k.score, k.score >= 100) : "") + "</td>" +
          "<td class='num pct'>" + state + "</td></tr>";
      });
    }
    wrap.innerHTML = '<div class="tableWrap"><table><thead>' + head + '</thead><tbody>' + rows + '</tbody></table></div>' +
      (hidden > 0 ? '<p class="note">Showing ' + shown + ' of ' + (shown + hidden) +
                    ' functions — worst-matching first, then the largest untouched. ' +
                    'The remaining ' + hidden + ' are untouched and smaller.</p>' : '');

    wrap.querySelectorAll("tr.drill").forEach(function (tr) {
      tr.addEventListener("click", function () {
        var k = kids[parseInt(tr.dataset.i, 10)];
        if (k && k.drill) { scope = { level: 1, bin: k.drill }; render(); scrollTo(0, 0); }
      });
    });
  }

  function esc(s) { var d = document.createElement("div"); d.textContent = s; return d.innerHTML; }

  // ---- chrome ------------------------------------------------------------
  function renderCrumbs() {
    var c = document.getElementById("crumbs");
    if (scope.level === 0) { c.innerHTML = '<span class="cur">Whole product</span>'; return; }
    c.innerHTML = '<button id="up">Whole product</button><span class="sep">/</span>' +
                  '<span class="cur">' + esc(scope.bin.name) + '</span>';
    document.getElementById("up").addEventListener("click", function () {
      scope = { level: 0, bin: null }; render(); scrollTo(0, 0);
    });
  }

  function render() {
    var st = currentStats(), kids = children();
    var p = pct(st.matched, st.total);
    var partial = pct(st.touched - st.matched, st.total);

    document.getElementById("heroPct").textContent = p.toFixed(p < 1 ? 3 : 2) + "%";
    document.getElementById("heroSub").innerHTML = scope.level === 0
      ? "of " + fmtBytes(st.total) + " of code across " + DATA.binaryCount + " shipped binaries"
      : "of " + fmtBytes(st.total) + " of code in " + esc(scope.bin.name);
    document.getElementById("heroBar").innerHTML =
      '<i class="s-done" style="width:' + p.toFixed(3) + '%"></i>' +
      '<i class="s-part" style="width:' + partial.toFixed(3) + '%"></i>' +
      '<i class="s-none" style="width:' + Math.max(0, 100 - p - partial).toFixed(3) + '%"></i>';

    document.getElementById("mapTitle").textContent =
      scope.level === 0 ? "Coverage map \u2014 every shipped binary" : "Coverage map \u2014 " + scope.bin.name;
    document.getElementById("mapNote").innerHTML = scope.level === 0
      ? "Each tile is one shipped binary, sized by its code and shaded by how much of it we match. Click to open it. A binary's coverage counts every function we can prove byte-identical to our reference copy, so reconstructing <span class='mono'>sidlib</span> once credits each binary that links the same build of it &mdash; in practice the two <span class='mono'>Facemaker.exe</span> builds, which are different binaries shipped under the same name and are listed separately. <span class='mono'>Magic.exe</span> looks unrelated here and is not: it is a second build of the same program, sharing 595 functions byte-exact with <span class='mono'>Shandalar.exe</span> (190 kB, 12.4% of its code) and another 237 kB at 90% or better. Only SIX of those 595 are in <span class='mono'>sidlib</span> &mdash; the library is the one part the two builds do not share, which is why everything reconstructed so far credits <span class='mono'>Facemaker</span> and not <span class='mono'>Magic</span>. Game code reconstructed from here counts twice, and those two binaries are 75% of the product. <span class='mono'>Manalink.exe</span> shares nothing at all and is a separate product."
      : (scope.bin.inventory === "full"
         ? "Each tile is one function, sized by its byte length and shaded by its own accuracy."
         : "Only the functions we have proved byte-identical to our reference copy are listed. The remainder is this binary's code with no function list extracted yet \u2014 its size is known from the PE header, nothing more.");
    document.getElementById("listTitle").textContent = scope.level === 0 ? "Shipped binaries" : "Functions";

    renderCrumbs();
    drawMap(kids);
    renderList(kids);
  }

  render();
  var t0; addEventListener("resize", function () { clearTimeout(t0); t0 = setTimeout(render, 140); });
})();
</script>
'@

    $html = $html.Replace("</body>", $script + "`n</body>")
    [System.IO.File]::WriteAllText($htmlPath, $html, (New-Object System.Text.UTF8Encoding($false)))
    Write-Host "wrote $htmlPath" -ForegroundColor Green
}

# ---------------------------------------------------------------------- modes

if ($Render) {
    Render-Report
}
elseif ($Backfill) {
    Write-Host "-Backfill populates progress/history.jsonl only; the report does not read it." -ForegroundColor DarkYellow
    Write-Error "backfill was removed when the report moved to product scope; use git history if needed"
}
else {
    $m = Measure-Tree -Dir $decomp -SkipBuild:$NoBuild
    if ($null -eq $m) { Write-Error "measurement failed"; exit 1 }

    $sha = (& git -C $repo log -1 --format="%h").Trim()
    $date = (& git -C $repo log -1 --format="%ad" --date=short).Trim()
    $subject = (& git -C $repo log -1 --format="%s").Trim()
    $hist = @(Read-History | Where-Object { $_.sha -ne $sha })
    $hist += [ordered]@{
        sha = $sha; date = $date; subject = $subject
        scored = $m.scored; total = $m.total; byteExact = $m.byteExact; functions = $m.functions
    }
    Write-History $hist

    # Always rebuild. The old fast path only refreshed the score of functions
    # already listed, so a newly reconstructed function never entered any
    # binary but the reference one -- Sid_OpenFileCore's 500-odd matched bytes
    # showed up in Shandalar.exe and were invisible in Facemaker.exe. With
    # funcs.csv cached the full rebuild is a fraction of a second.
    Build-Inventory -Measured $m
    Render-Report
}

if ($Open) { Start-Process $htmlPath }

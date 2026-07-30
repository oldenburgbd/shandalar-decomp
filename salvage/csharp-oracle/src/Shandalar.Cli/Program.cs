using Shandalar.Core;
using Shandalar.Core.Formats;

if (args.Length == 0)
{
    PrintUsage();
    return 1;
}

var install = GameInstall.FromEnvironment();
if (install is null)
{
    Console.Error.WriteLine($"Set {GameInstall.EnvVar} to your game directory first.");
    return 2;
}

switch (args[0])
{
    case "scan":
        return Scan(install);
    case "cards":
        return Cards(install, args.Skip(1).ToArray());
    case "card":
        return CardDetail(install, args.Skip(1).ToArray());
    case "deck":
        return DeckCmd(install, args.Skip(1).ToArray());
    case "art":
        return ArtCmd(install, args.Skip(1).ToArray());
    case "extract":
        return ExtractCmd(install, args.Skip(1).ToArray());
    case "pic":
        return PicCmd(install, args.Skip(1).ToArray());
    case "spr":
        return SprCmd(install, args.Skip(1).ToArray());
    case "sprprobe":
        return SprProbeCmd(install, args.Skip(1).ToArray());
    default:
        PrintUsage();
        return 1;
}

static void PrintUsage()
{
    Console.WriteLine("""
        usage: shandalar <command>

          scan             Inventory the game install (file types, counts, sizes)
          cards            Dump the card database from Master.csv
          cards <query>    Search cards by name fragment or exact numeric ID
          card <id|name>   Show full card detail (Master.csv text + Cards.dat stats)
          deck <name>      Show a deck (Decks/ or Playdeck/): main deck + sideboards
          deck             List all deck files
          art <id|name> [small|med] [out.bmp]  Decode a card's art from the .cat archives
          extract [outDir]  Extract all decodable assets to PNG/JSON + SHA-256 manifest
                            (default outDir: ./extracted; add --pics/--sprites/--art/
                             --decks/--cards to limit scope)
          pic <name> [pal] [out]  Decode a .pic to a BMP; pal (.tr/.plogpal/.pic) is
                                  required for external-palette files (e.g. Duel.plogpal)
          spr <name> [pal] [out]  Decode a .spr sheet to a BMP grid (pal: .tr/.plogpal/.pic,
                                  default Winbak01.pic; transparent shown as magenta)
          sprprobe <name.spr|--all>  Emit one machine-readable line per frame for the
                                  differential test against the reconstructed C. Columns:
                                  file frame width height u1 u2 emptyLines field14 minX maxX
        """);
}

/* Emits per-frame facts in a fixed column format so the same numbers can be produced by the
   reconstructed C in decomp/test and diffed. minX/maxX are the bounding box of non-transparent
   pixels, maxX exclusive, matching what Spr_GetFrameExtent (0x0057c450) computes. field14 is
   read straight from the frame header because the decoder does not currently use it. */
static int SprProbeCmd(GameInstall install, string[] args)
{
    if (args.Length == 0)
    {
        Console.Error.WriteLine("usage: shandalar sprprobe <name.spr|--all>");
        return 1;
    }

    var files = new List<string>();
    if (args[0] == "--all")
        files.AddRange(Directory.GetFiles(install.Root, "*.spr").OrderBy(f => f, StringComparer.OrdinalIgnoreCase));
    else
    {
        string n = args[0].EndsWith(".spr", StringComparison.OrdinalIgnoreCase) ? args[0] : args[0] + ".spr";
        files.Add(File.Exists(n) ? n : install.PathTo(n));
    }

    foreach (string path in files)
    {
        byte[] raw;
        IReadOnlyList<SprFrame> frames;
        try
        {
            raw = File.ReadAllBytes(path);
            frames = Spr.Decode(raw);
        }
        catch (Exception ex)
        {
            Console.WriteLine($"{Path.GetFileName(path)} ERROR {ex.GetType().Name}");
            continue;
        }

        // Walk the raw frame chain in parallel so header fields the decoder drops stay available.
        int off = 0;
        for (int i = 0; i < frames.Count; i++)
        {
            uint size = BitConverter.ToUInt32(raw, off);
            int emptyLines = BitConverter.ToUInt16(raw, off + 12);
            int field14 = BitConverter.ToUInt16(raw, off + 14);
            SprFrame f = frames[i];

            int minX = int.MaxValue, maxX = 0;
            for (int y = 0; y < f.Height; y++)
                for (int x = 0; x < f.Width; x++)
                    if (!f.Transparent[y * f.Width + x])
                    {
                        if (x < minX) minX = x;
                        if (x + 1 > maxX) maxX = x + 1;
                    }

            Console.WriteLine($"{Path.GetFileName(path)} {i} {f.Width} {f.Height} {f.Unknown1} {f.Unknown2} {emptyLines} {field14} {minX} {maxX}");
            off += (int)size;
        }
    }

    return 0;
}

static int SprCmd(GameInstall install, string[] args)
{
    if (args.Length == 0)
    {
        Console.Error.WriteLine("usage: shandalar spr <name.spr> [palette.tr|.pic] [out.bmp]");
        return 1;
    }

    string name = args[0].EndsWith(".spr", StringComparison.OrdinalIgnoreCase) ? args[0] : args[0] + ".spr";
    string sprPath = File.Exists(name) ? name : install.PathTo(name);
    if (!File.Exists(sprPath))
    {
        Console.Error.WriteLine($"Not found: {sprPath}");
        return 2;
    }

    string palName = args.Length > 1 ? args[1] : "Winbak01.pic";
    string palPath = File.Exists(palName) ? palName : install.PathTo(palName);
    Palette palette = Palette.FromFile(palPath);

    var frames = Spr.Decode(File.ReadAllBytes(sprPath));
    Console.WriteLine($"{Path.GetFileName(sprPath)}: {frames.Count} frames, palette {Path.GetFileName(palPath)}");

    int fw = frames.Max(f => f.Width), fh = frames.Max(f => f.Height);
    int cols = Math.Max(1, Math.Min(frames.Count, 1024 / fw));
    int rows = (frames.Count + cols - 1) / cols;
    int sw = fw * cols, sh = fh * rows;

    var sheet = new byte[sw * sh * 4];
    for (int i = 0; i < sheet.Length; i += 4) { sheet[i] = 255; sheet[i + 2] = 255; sheet[i + 3] = 255; } // magenta bg

    for (int fi = 0; fi < frames.Count; fi++)
    {
        var f = frames[fi];
        byte[] rgba = f.ToRgba(palette);
        int ox = (fi % cols) * fw, oy = (fi / cols) * fh;
        for (int y = 0; y < f.Height; y++)
            for (int x = 0; x < f.Width; x++)
            {
                int sp = ((oy + y) * sw + (ox + x)) * 4;
                int fp = (y * f.Width + x) * 4;
                if (rgba[fp + 3] == 0) continue; // transparent -> keep background
                sheet[sp] = rgba[fp]; sheet[sp + 1] = rgba[fp + 1];
                sheet[sp + 2] = rgba[fp + 2]; sheet[sp + 3] = 255;
            }
    }

    string outPath = args.Length > 2 ? args[2] : Path.ChangeExtension(Path.GetFileName(sprPath), ".bmp");
    Bmp.WriteRgba(outPath, sw, sh, sheet);
    Console.WriteLine($"wrote {outPath} ({sw}x{sh}, {cols}x{rows} grid)");
    return 0;
}

static int PicCmd(GameInstall install, string[] args)
{
    if (args.Length == 0)
    {
        Console.Error.WriteLine("usage: shandalar pic <name.pic> [palette.tr|.plogpal|.pic] [out.bmp]");
        return 1;
    }

    string name = args[0].EndsWith(".pic", StringComparison.OrdinalIgnoreCase) ? args[0] : args[0] + ".pic";
    string path = File.Exists(name) ? name : install.PathTo(name);
    if (!File.Exists(path))
    {
        Console.Error.WriteLine($"Not found: {path}");
        return 2;
    }

    // Args after the name: an optional palette (has an extension) and/or an output path.
    string? palArg = args.Skip(1).FirstOrDefault(IsPaletteArg);
    string? outArg = args.Skip(1).FirstOrDefault(a => !IsPaletteArg(a));

    PicImage img = Pic.Decode(File.ReadAllBytes(path));
    Console.WriteLine($"{Path.GetFileName(path)}: {img.Width}x{img.Height}, " +
        $"palette={(img.Palette is null ? "external" : img.Palette.WasSixBit ? "embedded 6-bit" : "embedded 8-bit")}");

    Palette? palette = img.Palette;
    if (palArg is not null)
        palette = Palette.FromFile(File.Exists(palArg) ? palArg : install.PathTo(palArg));

    if (palette is null)
    {
        Console.Error.WriteLine(
            "This file uses an external palette; pass one, e.g.: shandalar pic " +
            $"{Path.GetFileNameWithoutExtension(path)} Duel.plogpal out.bmp");
        return 3;
    }

    string outPath = outArg ?? Path.ChangeExtension(Path.GetFileName(path), ".bmp");
    Bmp.WriteRgba(outPath, img.Width, img.Height, img.ToRgba(palette));
    Console.WriteLine($"wrote {outPath}");
    return 0;

    static bool IsPaletteArg(string a) =>
        a.EndsWith(".tr", StringComparison.OrdinalIgnoreCase) ||
        a.EndsWith(".plogpal", StringComparison.OrdinalIgnoreCase) ||
        a.EndsWith(".pic", StringComparison.OrdinalIgnoreCase);
}

static int Scan(GameInstall install)
{
    Console.WriteLine($"Game install: {install.Root}\n");
    var files = new DirectoryInfo(install.Root).GetFiles("*", SearchOption.AllDirectories);
    var byExt = files
        .GroupBy(f => f.Extension.ToLowerInvariant() is "" ? "(none)" : f.Extension.ToLowerInvariant())
        .OrderByDescending(g => g.Sum(f => f.Length));

    Console.WriteLine($"{"ext",-12} {"count",7} {"bytes",14}");
    foreach (var g in byExt)
        Console.WriteLine($"{g.Key,-12} {g.Count(),7} {g.Sum(f => f.Length),14:N0}");
    Console.WriteLine($"\nTotal: {files.Length} files, {files.Sum(f => f.Length):N0} bytes");
    return 0;
}

static int Cards(GameInstall install, string[] args)
{
    var db = install.LoadCardDatabase();
    var results = args.Length == 0
        ? db.Cards
        : int.TryParse(args[0], out var id)
            ? (db.Find(id) is { } c ? [c] : Enumerable.Empty<Shandalar.Core.Cards.CardRecord>())
            : db.Search(string.Join(' ', args));

    int count = 0;
    foreach (var card in results)
    {
        Console.WriteLine($"{card.Id:D4}  {card.Name,-32} {card.TypeLine}");
        count++;
    }
    Console.WriteLine($"\n{count} card(s)");
    return count > 0 ? 0 : 3;
}

static int CardDetail(GameInstall install, string[] args)
{
    if (args.Length == 0)
    {
        Console.Error.WriteLine("usage: shandalar card <id|name>");
        return 1;
    }

    var stats = install.LoadCardStats();
    var db = install.LoadCardDatabase();

    Shandalar.Core.Cards.CardStats? stat = int.TryParse(args[0], out int id)
        ? stats.FirstOrDefault(s => s.Id == id)
        : stats.FirstOrDefault(s => s.Name.Contains(string.Join(' ', args), StringComparison.OrdinalIgnoreCase));

    if (stat is null)
    {
        Console.Error.WriteLine("No card with gameplay stats matched. (IDs 999+ are effect-only.)");
        return 3;
    }

    var text = db.Find(stat.Id);
    Console.WriteLine($"#{stat.Id:D4}  {stat.Name}");
    if (text is not null && text.Name != stat.Name)
        Console.WriteLine($"  display name : {text.Name}   (Master.csv)");
    Console.WriteLine($"  type         : {stat.Category}  ({text?.TypeLine})");
    Console.WriteLine($"  colour       : {stat.ColorCode}");
    Console.WriteLine($"  mana         : {FormatMana(stat.Mana)}  (CMC {stat.Mana.ConvertedManaCost})");
    if (stat.IsCreature)
        Console.WriteLine($"  power/tough  : {stat.Power}/{stat.Toughness}");
    Console.WriteLine($"  abilities    : flags 0x{stat.AbilityFlags:x}{(stat.HasFlying ? " (flying)" : "")}");
    Console.WriteLine($"  art file     : {stat.ArtFile}");
    if (!string.IsNullOrWhiteSpace(text?.RulesText))
        Console.WriteLine($"  rules        : {text!.RulesText}");
    if (!string.IsNullOrWhiteSpace(text?.FlavorText) && text!.FlavorText != "None")
        Console.WriteLine($"  flavour      : {text.FlavorText}");
    return 0;

    static string FormatMana(Shandalar.Core.Cards.ManaCost m)
    {
        var sb = new System.Text.StringBuilder();
        if (m.IsVariable) sb.Append('X');
        if (m.Generic > 0) sb.Append(m.Generic);
        sb.Append('W', m.White).Append('U', m.Blue).Append('B', m.Black).Append('R', m.Red).Append('G', m.Green);
        return sb.Length == 0 ? "(none)" : sb.ToString();
    }
}

static int ExtractCmd(GameInstall install, string[] args)
{
    string outDir = args.FirstOrDefault(a => !a.StartsWith("--")) ?? "extracted";

    var scope = Shandalar.Core.Extraction.ExtractScope.All;
    var flags = args.Where(a => a.StartsWith("--")).ToList();
    if (flags.Count > 0)
    {
        scope = 0;
        foreach (var f in flags)
            scope |= f switch
            {
                "--pics" => Shandalar.Core.Extraction.ExtractScope.Pics,
                "--sprites" => Shandalar.Core.Extraction.ExtractScope.Sprites,
                "--art" => Shandalar.Core.Extraction.ExtractScope.CardArt,
                "--decks" => Shandalar.Core.Extraction.ExtractScope.Decks,
                "--cards" => Shandalar.Core.Extraction.ExtractScope.CardDb,
                _ => 0,
            };
        if (scope == 0)
        {
            Console.Error.WriteLine($"Unknown scope flags: {string.Join(' ', flags)}");
            return 1;
        }
    }

    var sw = System.Diagnostics.Stopwatch.StartNew();
    var extractor = new Shandalar.Core.Extraction.AssetExtractor(install, outDir, Console.WriteLine);
    extractor.Run(scope);
    Console.WriteLine($"Extracted {extractor.OutputCount} files to {Path.GetFullPath(outDir)} in {sw.Elapsed.TotalSeconds:F1}s");
    return 0;
}

static int ArtCmd(GameInstall install, string[] args)
{
    if (args.Length == 0)
    {
        Console.Error.WriteLine("usage: shandalar art <cardId|name> [small|med] [out.bmp]");
        return 1;
    }

    string sizeArg = args.Skip(1).FirstOrDefault(a => a is "small" or "med") ?? "med";
    string? outArg = args.Skip(1).FirstOrDefault(a => a is not ("small" or "med"));

    int cardId;
    string cardName;
    if (int.TryParse(args[0], out cardId))
    {
        cardName = install.LoadCardStats().FirstOrDefault(s => s.Id == cardId)?.Name ?? $"card {cardId}";
    }
    else
    {
        var stat = install.LoadCardStats()
            .FirstOrDefault(s => s.Name.Contains(args[0], StringComparison.OrdinalIgnoreCase));
        if (stat is null)
        {
            Console.Error.WriteLine($"No card matched '{args[0]}'.");
            return 2;
        }
        cardId = stat.Id;
        cardName = stat.Name;
    }

    var archive = sizeArg == "small" ? install.LoadSmallArt() : install.LoadMediumArt();
    var entry = archive.FindByCardId(cardId);
    if (entry is null)
    {
        Console.Error.WriteLine($"No {sizeArg} art for card {cardId} ({cardName}).");
        return 3;
    }

    WvlImage img = entry.Decode();
    string outPath = outArg ?? $"{cardId:D4}_{sizeArg}.bmp";
    Bmp.WriteRgba(outPath, img.Width, img.Height, img.Rgba);
    Console.WriteLine($"{cardName} (#{cardId}, {entry.Name}): {img.Width}x{img.Height} -> {outPath}");
    return 0;
}

static int DeckCmd(GameInstall install, string[] args)
{
    var paths = install.DeckPaths().ToList();

    if (args.Length == 0)
    {
        foreach (var p in paths)
            Console.WriteLine($"  {Path.GetFileName(Path.GetDirectoryName(p))}/{Path.GetFileName(p)}");
        Console.WriteLine($"\n{paths.Count} deck(s)");
        return 0;
    }

    string query = string.Join(' ', args);
    string? path = File.Exists(query) ? query
        : paths.FirstOrDefault(p => Path.GetFileNameWithoutExtension(p)
            .Equals(query, StringComparison.OrdinalIgnoreCase))
        ?? paths.FirstOrDefault(p => Path.GetFileName(p)
            .Contains(query, StringComparison.OrdinalIgnoreCase));

    if (path is null)
    {
        Console.Error.WriteLine($"No deck matched '{query}'.");
        return 2;
    }

    var deck = install.LoadDeck(path);
    Console.WriteLine($"{deck.Name}   ({Path.GetFileName(path)})");
    foreach (var c in deck.Comments.Skip(1))
        Console.WriteLine($"  ; {c}");
    Console.WriteLine($"\nMain deck ({deck.MainCardCount} cards):");
    foreach (var e in deck.Main)
        Console.WriteLine($"  {e.Count,3}x  {e.Name}  (#{e.CardId})");

    foreach (var (colour, entries) in deck.Sideboards.OrderBy(kv => kv.Key))
    {
        Console.WriteLine($"\nvs {colour} ({entries.Sum(e => e.Count)} cards):");
        foreach (var e in entries)
            Console.WriteLine($"  {e.Count,3}x  {e.Name}  (#{e.CardId})");
    }
    return 0;
}

/// <summary>Minimal uncompressed 32-bit BMP writer (top-to-bottom RGBA input).</summary>
static class Bmp
{
    public static void WriteRgba(string path, int w, int h, byte[] rgba)
    {
        int imgSize = w * h * 4;
        using var bw = new BinaryWriter(File.Create(path));
        bw.Write((byte)'B'); bw.Write((byte)'M');
        bw.Write(54 + imgSize); bw.Write(0); bw.Write(54);
        bw.Write(40); bw.Write(w); bw.Write(-h); // negative height = top-down
        bw.Write((short)1); bw.Write((short)32);
        bw.Write(0); bw.Write(imgSize); bw.Write(2835); bw.Write(2835); bw.Write(0); bw.Write(0);
        for (int i = 0; i < w * h; i++)
        {
            int o = i * 4;
            bw.Write(rgba[o + 2]); // B
            bw.Write(rgba[o + 1]); // G
            bw.Write(rgba[o]);     // R
            bw.Write(rgba[o + 3]); // A
        }
    }
}

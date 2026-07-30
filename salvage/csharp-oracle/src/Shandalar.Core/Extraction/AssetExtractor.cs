using System.Security.Cryptography;
using System.Text.Json;
using Shandalar.Core.Cards;
using Shandalar.Core.Formats;

namespace Shandalar.Core.Extraction;

/// <summary>Which asset groups to extract.</summary>
[Flags]
public enum ExtractScope
{
    Pics = 1,
    Sprites = 2,
    CardArt = 4,
    Decks = 8,
    CardDb = 16,
    All = Pics | Sprites | CardArt | Decks | CardDb,
}

/// <summary>
/// Extracts decodable game assets to PNG/JSON files plus a manifest of source SHA-256 hashes so
/// re-runs are diffable (issue #8). Output goes outside the repo — game-derived content is never
/// committed. External-palette .pic files use Duel.plogpal for Creatures* and Winbak01.pic's
/// palette otherwise (see docs/formats/palettes.md); sprites render through Winbak01.pic.
/// </summary>
public sealed class AssetExtractor
{
    private readonly GameInstall _install;
    private readonly string _outDir;
    private readonly Action<string>? _progress;
    private readonly List<object> _sources = new();
    private readonly HashSet<string> _sourcePaths = new();
    private readonly List<object> _outputs = new();

    public int OutputCount => _outputs.Count;

    public AssetExtractor(GameInstall install, string outDir, Action<string>? progress = null)
    {
        _install = install;
        _outDir = outDir;
        _progress = progress;
    }

    public void Run(ExtractScope scope)
    {
        Directory.CreateDirectory(_outDir);

        if (scope.HasFlag(ExtractScope.Pics))
            ExtractPics();
        if (scope.HasFlag(ExtractScope.Sprites))
            ExtractSprites();
        if (scope.HasFlag(ExtractScope.CardArt))
            ExtractCardArt();
        if (scope.HasFlag(ExtractScope.Decks))
            ExtractDecks();
        if (scope.HasFlag(ExtractScope.CardDb))
            ExtractCardDb();

        WriteJson("manifest.json", new
        {
            generatedUtc = DateTime.UtcNow.ToString("o"),
            gameDir = _install.Root,
            sources = _sources,
            outputs = _outputs,
        }, track: false);
    }

    private void ExtractPics()
    {
        Palette fallback = Pic.Decode(ReadSource(_install.PathTo("Winbak01.pic"))).Palette!;
        Palette duel = Palette.FromFile(_install.PathTo("Duel.plogpal"));

        foreach (var path in Directory.EnumerateFiles(_install.Root, "*.pic").OrderBy(p => p))
        {
            string name = Path.GetFileNameWithoutExtension(path);
            try
            {
                PicImage img = Pic.Decode(ReadSource(path));
                Palette pal = img.Palette
                    ?? (name.StartsWith("Creatures", StringComparison.OrdinalIgnoreCase) ? duel : fallback);
                WritePng(Path.Combine("pics", name + ".png"), img.Width, img.Height, img.ToRgba(pal), path);
            }
            catch (Exception ex)
            {
                _progress?.Invoke($"  SKIP {name}.pic: {ex.Message}");
            }
        }
        _progress?.Invoke("pics done");
    }

    private void ExtractSprites()
    {
        Palette pal = Pic.Decode(File.ReadAllBytes(_install.PathTo("Winbak01.pic"))).Palette!;

        var files = Directory.EnumerateFiles(_install.Root, "*.spr", SearchOption.AllDirectories);
        foreach (var path in files.OrderBy(p => p))
        {
            string rel = Path.GetRelativePath(_install.Root, path);
            string name = rel.Replace(Path.DirectorySeparatorChar, '_')[..^4];
            try
            {
                var frames = Spr.Decode(ReadSource(path));
                var sheet = Rendering.SpriteSheet.Build(frames, pal);
                WritePng(Path.Combine("sprites", name + ".png"), sheet.Width, sheet.Height, sheet.Pixels, path);
            }
            catch (Exception ex)
            {
                _progress?.Invoke($"  SKIP {rel}: {ex.Message}");
            }
        }
        _progress?.Invoke("sprites done");
    }

    private void ExtractCardArt()
    {
        foreach (var (file, folder) in new[] { ("Smallart.cat", "art-small"), ("Medart.cat", "art-med") })
        {
            string path = _install.PathTo(Path.Combine("Cardart", file));
            var archive = CatArchive.Parse(ReadSource(path));
            foreach (var entry in archive.Entries)
            {
                WvlImage img = entry.Decode();
                string name = entry.CardId >= 0 ? entry.CardId.ToString("D4") : entry.Name;
                WritePng(Path.Combine(folder, name + ".png"), img.Width, img.Height, img.Rgba, path);
            }
            _progress?.Invoke($"{folder} done ({archive.Entries.Count} entries)");
        }
    }

    private void ExtractDecks()
    {
        var decks = new List<object>();
        foreach (var path in _install.DeckPaths().OrderBy(p => p))
        {
            ReadSource(path);
            DeckList deck = _install.LoadDeck(path);
            decks.Add(new
            {
                file = Path.GetRelativePath(_install.Root, path).Replace('\\', '/'),
                name = deck.Name,
                comments = deck.Comments,
                main = deck.Main.Select(e => new { id = e.CardId, count = e.Count, name = e.Name }),
                sideboards = deck.Sideboards.ToDictionary(
                    kv => kv.Key.ToString(),
                    kv => kv.Value.Select(e => new { id = e.CardId, count = e.Count, name = e.Name })),
            });
        }
        WriteJson("decks.json", decks);
        _progress?.Invoke($"decks done ({decks.Count})");
    }

    private void ExtractCardDb()
    {
        ReadSource(_install.PathTo("Master.csv"));
        ReadSource(_install.PathTo("Cards.dat"));
        ReadSource(_install.PathTo("Info.csv"));

        var master = _install.LoadCardDatabase();
        var stats = _install.LoadCardStats().ToDictionary(s => s.Id);
        var info = _install.LoadInfo().ToDictionary(i => i.Id);

        var cards = master.Cards.Select(c => new
        {
            id = c.Id,
            displayName = c.Name,
            canonicalName = stats.GetValueOrDefault(c.Id)?.Name,
            typeLine = c.TypeLine,
            rulesText = c.RulesText,
            flavorText = c.FlavorText,
            artist = c.Artist,
            stats = stats.TryGetValue(c.Id, out var s) ? new
            {
                color = s.ColorCode.ToString(),
                category = s.Category.ToString(),
                mana = new { s.Mana.Generic, s.Mana.White, s.Mana.Blue, s.Mana.Black, s.Mana.Red, s.Mana.Green, x = s.Mana.IsVariable },
                s.Power,
                s.Toughness,
                flying = s.HasFlying,
                artFile = s.ArtFile,
            } : null,
            set = info.GetValueOrDefault(c.Id)?.Set,
            rarity = info.GetValueOrDefault(c.Id)?.Rarity,
            subtype = info.GetValueOrDefault(c.Id)?.Subtype,
        });
        WriteJson("cards.json", cards);
        _progress?.Invoke("card db done");
    }

    // ---------------------------------------------------------------- helpers

    private byte[] ReadSource(string path)
    {
        byte[] data = File.ReadAllBytes(path);
        string rel = Path.GetRelativePath(_install.Root, path).Replace('\\', '/');
        if (_sourcePaths.Add(rel))
            _sources.Add(new
            {
                path = rel,
                sha256 = Convert.ToHexString(SHA256.HashData(data)).ToLowerInvariant(),
                bytes = data.Length,
            });
        return data;
    }

    private void WritePng(string relPath, int width, int height, byte[] rgba, string sourcePath)
    {
        byte[] png = PngWriter.EncodeRgba(width, height, rgba);
        Write(relPath, png, Path.GetRelativePath(_install.Root, sourcePath).Replace('\\', '/'));
    }

    private void WriteJson(string relPath, object value, bool track = true)
    {
        byte[] json = JsonSerializer.SerializeToUtf8Bytes(value,
            new JsonSerializerOptions { WriteIndented = true });
        Write(relPath, json, source: null, track: track);
    }

    private void Write(string relPath, byte[] data, string? source, bool track = true)
    {
        string full = Path.Combine(_outDir, relPath);
        Directory.CreateDirectory(Path.GetDirectoryName(full)!);
        File.WriteAllBytes(full, data);
        if (track)
            _outputs.Add(new { path = relPath.Replace('\\', '/'), source, bytes = data.Length });
    }
}

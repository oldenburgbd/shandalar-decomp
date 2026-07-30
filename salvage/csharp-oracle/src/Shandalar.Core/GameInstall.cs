namespace Shandalar.Core;

/// <summary>
/// Locates and validates an original Duels of the Planeswalkers v3.0 install.
/// Game files are never committed to this repo; all tooling resolves them through here.
/// </summary>
public sealed class GameInstall
{
    public const string EnvVar = "SHANDALAR_GAME_DIR";

    /// <summary>Files that must exist for a directory to count as a game install.</summary>
    public static readonly string[] RequiredFiles =
        ["Magic.exe", "Shandalar.exe", "Master.csv", "Cards.dat"];

    public string Root { get; }

    private GameInstall(string root) => Root = root;

    public string PathTo(string relative) => Path.Combine(Root, relative);

    public Cards.CardDatabase LoadCardDatabase() =>
        Cards.CardDatabase.LoadMasterCsv(PathTo("Master.csv"));

    /// <summary>Gameplay stats for card IDs 0..998, decoded from Cards.dat (see #4).</summary>
    public IReadOnlyList<Cards.CardStats> LoadCardStats() =>
        Cards.CardsDat.Load(PathTo("Cards.dat"));

    /// <summary>Paths of all shipped .dck deck files (Decks/ theme decks + Playdeck/ AI decks).</summary>
    public IEnumerable<string> DeckPaths()
    {
        foreach (var folder in new[] { "Decks", "Playdeck" })
        {
            string dir = PathTo(folder);
            if (Directory.Exists(dir))
                foreach (var file in Directory.EnumerateFiles(dir, "*.dck"))
                    yield return file;
        }
    }

    public Cards.DeckList LoadDeck(string path) => Cards.DeckFile.Load(path);

    /// <summary>Per-card display/deck-builder data from Info.csv, all 1,250 IDs. See #7.</summary>
    public IReadOnlyList<Cards.InfoRecord> LoadInfo() => Cards.InfoCsv.Load(PathTo("Info.csv"));

    /// <summary>Extra effect/legacy card text from Legacy.csv, IDs 0-999. See #7.</summary>
    public IReadOnlyList<Cards.LegacyRecord> LoadLegacy() =>
        Cards.AuxCsvs.LoadLegacy(PathTo("Legacy.csv"));

    /// <summary>The duel opponents from Rogues.csv. See #7.</summary>
    public IReadOnlyList<Cards.RogueRecord> LoadRogues() =>
        Cards.AuxCsvs.LoadRogues(PathTo("Rogues.csv"));

    /// <summary>The small card-art archive (Cardart/Smallart.cat, ~144x116 images). See #6.</summary>
    public Formats.CatArchive LoadSmallArt() =>
        Formats.CatArchive.Load(PathTo(Path.Combine("Cardart", "Smallart.cat")));

    /// <summary>The medium card-art archive (Cardart/Medart.cat, ~288x232 images). See #6.</summary>
    public Formats.CatArchive LoadMediumArt() =>
        Formats.CatArchive.Load(PathTo(Path.Combine("Cardart", "Medart.cat")));

    public static GameInstall Open(string root)
    {
        var missing = RequiredFiles.Where(f => !File.Exists(Path.Combine(root, f))).ToList();
        if (missing.Count > 0)
            throw new DirectoryNotFoundException(
                $"'{root}' is not a game install (missing: {string.Join(", ", missing)})");
        return new GameInstall(root);
    }

    /// <summary>Opens the install pointed to by SHANDALAR_GAME_DIR, or null if unset.</summary>
    public static GameInstall? FromEnvironment()
    {
        var dir = Environment.GetEnvironmentVariable(EnvVar);
        return string.IsNullOrWhiteSpace(dir) ? null : Open(dir);
    }
}

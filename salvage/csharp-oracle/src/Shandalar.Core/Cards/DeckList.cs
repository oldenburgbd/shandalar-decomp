namespace Shandalar.Core.Cards;

/// <summary>The opponent-colour a sideboard section tunes against (see docs/formats/dck.md).</summary>
public enum SideboardColor
{
    None,
    Black,
    Blue,
    Green,
    Red,
    White,
}

/// <summary>One deck line: <paramref name="Count"/> copies of card <paramref name="CardId"/>.</summary>
public sealed record DeckEntry(int CardId, int Count, string Name);

/// <summary>
/// A parsed <c>.dck</c> file: the main deck plus optional per-opponent-colour sideboard groups.
/// </summary>
public sealed class DeckList
{
    public string Name { get; }

    /// <summary>Raw metadata comment lines (Playdeck header), leading ';' stripped.</summary>
    public IReadOnlyList<string> Comments { get; }

    public IReadOnlyList<DeckEntry> Main { get; }

    /// <summary>Sideboard groups keyed by opponent colour (empty for most decks).</summary>
    public IReadOnlyDictionary<SideboardColor, IReadOnlyList<DeckEntry>> Sideboards { get; }

    public DeckList(
        string name,
        IReadOnlyList<string> comments,
        IReadOnlyList<DeckEntry> main,
        IReadOnlyDictionary<SideboardColor, IReadOnlyList<DeckEntry>> sideboards)
    {
        Name = name;
        Comments = comments;
        Main = main;
        Sideboards = sideboards;
    }

    /// <summary>Total copies in the main deck.</summary>
    public int MainCardCount => Main.Sum(e => e.Count);

    /// <summary>All entries across the main deck and every sideboard group.</summary>
    public IEnumerable<DeckEntry> AllEntries =>
        Main.Concat(Sideboards.Values.SelectMany(e => e));
}

namespace Shandalar.Core.Cards;

/// <summary>
/// Reader for the plain-text <c>.dck</c> deck format. See docs/formats/dck.md.
/// </summary>
public static class DeckFile
{
    public static DeckList Load(string path)
    {
        using var reader = new StreamReader(path);
        return Parse(reader, Path.GetFileNameWithoutExtension(path));
    }

    public static DeckList Parse(TextReader reader, string? nameFallback = null)
    {
        string? name = null;
        var comments = new List<string>();
        var main = new List<DeckEntry>();
        var sideboards = new Dictionary<SideboardColor, List<DeckEntry>>();

        List<DeckEntry> current = main; // cards go to the main deck until a .v marker

        for (string? line = reader.ReadLine(); line is not null; line = reader.ReadLine())
        {
            if (line.Length == 0)
                continue;

            if (line[0] == ';')
            {
                string comment = line[1..].Trim();
                comments.Add(comment);
                name ??= comment; // first comment is the deck name (Playdeck style)
            }
            else if (line.StartsWith(".v", StringComparison.OrdinalIgnoreCase))
            {
                var colour = ParseColour(line[2..].Trim());
                current = sideboards.TryGetValue(colour, out var existing)
                    ? existing
                    : sideboards[colour] = new List<DeckEntry>();
            }
            else if (line[0] == '.' && line.Length > 1 && char.IsDigit(line[1]))
            {
                if (TryParseCard(line, out var entry))
                    current.Add(entry);
            }
            else
            {
                name ??= line.Trim(); // title line (Decks style)
            }
        }

        return new DeckList(
            name ?? nameFallback ?? "",
            comments,
            main,
            sideboards.ToDictionary(kv => kv.Key, kv => (IReadOnlyList<DeckEntry>)kv.Value));
    }

    private static bool TryParseCard(string line, out DeckEntry entry)
    {
        entry = default!;
        // ".<id>\t<count>\t<name>"
        var parts = line[1..].Split('\t');
        if (parts.Length < 3)
            return false;
        if (!int.TryParse(parts[0], out int id) || !int.TryParse(parts[1], out int count))
            return false;
        entry = new DeckEntry(id, count, parts[2]);
        return true;
    }

    private static SideboardColor ParseColour(string token) => token.ToLowerInvariant() switch
    {
        "black" => SideboardColor.Black,
        "blue" => SideboardColor.Blue,
        "green" => SideboardColor.Green,
        "red" => SideboardColor.Red,
        "white" => SideboardColor.White,
        _ => SideboardColor.None,
    };
}

namespace Shandalar.Core.Formats;

/// <summary>
/// Reads MicroProse <c>.tr</c> text palettes (Todpal.tr, Duelpal.tr, Advpal.tr, …). Each line is
/// <c>index - R G B - (extra…)</c> with 6-bit VGA RGB values (0-63), scaled here to 8-bit. Extra
/// trailing numbers and unspecified indices are left black. See issue #3 for asset→palette pairing.
/// </summary>
public static class TrPalette
{
    public static Palette Load(string path)
    {
        using var reader = new StreamReader(path);
        return Load(reader);
    }

    /// <summary>Loads a <c>.tr</c> as a standalone palette; entries it doesn't define stay black.</summary>
    public static Palette Load(TextReader reader)
    {
        var entries = new Rgb[Palette.EntryCount];
        foreach (var (index, colour) in ReadEntries(reader))
            entries[index] = colour;
        return new Palette(entries, wasSixBit: true);
    }

    /// <summary>
    /// Loads a partial <c>.tr</c> as an overlay on top of <paramref name="baseline"/> — the way the
    /// game applies per-screen palettes (e.g. Advpal.tr defines only ~11 entries). See issue #3.
    /// </summary>
    public static Palette LoadOverlay(string path, Palette baseline)
    {
        using var reader = new StreamReader(path);
        return baseline.WithOverrides(ReadEntries(reader));
    }

    /// <summary>The index→colour entries a <c>.tr</c> file explicitly defines (6-bit RGB, scaled).</summary>
    public static IReadOnlyList<(int Index, Rgb Colour)> ReadEntries(TextReader reader)
    {
        var result = new List<(int, Rgb)>();
        for (string? line = reader.ReadLine(); line is not null; line = reader.ReadLine())
        {
            var tokens = line.Replace('-', ' ')
                .Split((char[]?)null, StringSplitOptions.RemoveEmptyEntries);
            if (tokens.Length < 4)
                continue;
            if (!int.TryParse(tokens[0], out int index) || index is < 0 or > 255)
                continue;
            if (!int.TryParse(tokens[1], out int r) ||
                !int.TryParse(tokens[2], out int g) ||
                !int.TryParse(tokens[3], out int b))
                continue;

            result.Add((index, new Rgb(
                Palette.ScaleSixToEight((byte)r),
                Palette.ScaleSixToEight((byte)g),
                Palette.ScaleSixToEight((byte)b))));
        }
        return result;
    }
}

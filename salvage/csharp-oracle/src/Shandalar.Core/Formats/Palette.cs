namespace Shandalar.Core.Formats;

/// <summary>A single 8-bit-per-channel RGB colour.</summary>
public readonly record struct Rgb(byte R, byte G, byte B);

/// <summary>
/// A 256-entry colour table. Values are stored already scaled to 8 bits per channel
/// (a 6-bit VGA palette from an <c>M0</c> block is scaled on load), so entries are display-ready.
/// </summary>
public sealed class Palette
{
    public const int EntryCount = 256;

    private readonly Rgb[] _entries;

    /// <summary>True if this palette came from a 6-bit VGA source and was scaled to 8-bit.</summary>
    public bool WasSixBit { get; }

    public Palette(Rgb[] entries, bool wasSixBit)
    {
        if (entries.Length != EntryCount)
            throw new ArgumentException($"Palette needs {EntryCount} entries", nameof(entries));
        _entries = entries;
        WasSixBit = wasSixBit;
    }

    public Rgb this[int index] => _entries[index];

    /// <summary>Scales a 6-bit VGA DAC value (0-63) to 8-bit (0-255): 63 → 255, 0 → 0.</summary>
    public static byte ScaleSixToEight(byte v) => (byte)((v << 2) | (v >> 4));

    /// <summary>
    /// Returns a copy of this palette with the given index→colour overrides applied. Used to layer
    /// a partial <c>.tr</c> override on top of a base palette, as the game does per screen (#3).
    /// </summary>
    public Palette WithOverrides(IEnumerable<(int Index, Rgb Colour)> overrides)
    {
        var copy = (Rgb[])_entries.Clone();
        foreach (var (index, colour) in overrides)
            copy[index] = colour;
        return new Palette(copy, WasSixBit);
    }

    /// <summary>
    /// Loads a palette from any supported palette file: <c>.tr</c> (text override table),
    /// <c>.plogpal</c> (binary duel palette), or a self-contained <c>.pic</c>'s embedded palette.
    /// </summary>
    public static Palette FromFile(string path)
    {
        if (path.EndsWith(".tr", StringComparison.OrdinalIgnoreCase))
            return TrPalette.Load(path);
        if (path.EndsWith(".plogpal", StringComparison.OrdinalIgnoreCase))
            return PlogPalette.Load(path);
        if (path.EndsWith(".pic", StringComparison.OrdinalIgnoreCase))
            return Pic.Decode(File.ReadAllBytes(path)).Palette
                ?? throw new InvalidDataException($"{path} has no embedded palette");
        throw new NotSupportedException($"Unrecognised palette file: {path}");
    }
}

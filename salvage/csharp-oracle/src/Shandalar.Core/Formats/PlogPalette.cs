namespace Shandalar.Core.Formats;

/// <summary>
/// Reads MicroProse <c>.plogpal</c> binary palettes (Duel.plogpal — the duel creature/card-art
/// palette). Layout: a 4-byte header (byte 1 = 3, then a uint16 LE entry count of 256) followed by
/// 256 index-ordered entries of [R, G, B, flag]; RGB is already 8-bit. The per-entry flag is 0 for
/// the reserved index 0 and 1 otherwise (kept out of the colour). See docs/formats/palettes.md.
/// </summary>
public static class PlogPalette
{
    public const int HeaderSize = 4;

    public static Palette Load(string path) => Parse(File.ReadAllBytes(path));

    public static Palette Parse(byte[] data)
    {
        int need = HeaderSize + Palette.EntryCount * 4;
        if (data.Length < need)
            throw new InvalidDataException(
                $".plogpal too small: {data.Length} bytes, need at least {need}");

        var entries = new Rgb[Palette.EntryCount];
        for (int i = 0; i < Palette.EntryCount; i++)
        {
            int o = HeaderSize + i * 4;
            entries[i] = new Rgb(data[o], data[o + 1], data[o + 2]);
        }
        return new Palette(entries, wasSixBit: false);
    }
}

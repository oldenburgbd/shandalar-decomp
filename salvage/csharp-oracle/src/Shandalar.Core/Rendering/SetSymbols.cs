using Shandalar.Core.Formats;

namespace Shandalar.Core.Rendering;

/// <summary>
/// Cardart/Cardsets.pic: a 330x15 strip of five 66px cells holding the expansion symbols
/// drawn on the type line — in x order: The Dark (moon), Legends (column), Arabian Nights
/// (scimitar), Antiquities (anvil), Astral (star). Core-set ("Magic") cards carry no symbol,
/// matching real cards of the era. Within each cell the symbol is trimmed to its occupied
/// columns so it can be right-aligned tightly.
/// </summary>
public sealed class SetSymbols
{
    public const int CellWidth = 66;

    /// <summary>Info.csv "Card Set" values in the strip's left-to-right cell order.</summary>
    private static readonly string[] CellOrder =
        ["Dark", "Legends", "Arabian", "Antiquities", "Astral"];

    private readonly PicImage _pic;
    private readonly Palette _palette;
    private readonly byte _transparentIndex;
    // Per set: trimmed x-range within the strip [left, right).
    private readonly Dictionary<string, (int Left, int Right)> _bounds = new(StringComparer.OrdinalIgnoreCase);

    public SetSymbols(PicImage pic, Palette palette)
    {
        _pic = pic;
        _palette = palette;
        _transparentIndex = pic.Indices[0];

        for (int cell = 0; cell < CellOrder.Length; cell++)
        {
            int x0 = cell * CellWidth;
            int x1 = Math.Min(x0 + CellWidth, pic.Width);
            int left = int.MaxValue, right = int.MinValue;
            for (int x = x0; x < x1; x++)
                for (int y = 0; y < pic.Height; y++)
                    if (pic.Indices[y * pic.Width + x] != _transparentIndex)
                    {
                        left = Math.Min(left, x);
                        right = Math.Max(right, x + 1);
                        break;
                    }
            if (left <= right)
                _bounds[CellOrder[cell]] = (left, right);
        }
    }

    public int SymbolCount => _bounds.Count;

    public bool Knows(string set) => _bounds.ContainsKey(set);

    /// <summary>
    /// Draws the symbol for <paramref name="set"/> so its right edge sits at (right, y).
    /// No-op for unknown/core sets. Returns whether a symbol was drawn.
    /// </summary>
    public bool TryDraw(RgbaImage dest, string set, int right, int y)
    {
        if (!_bounds.TryGetValue(set, out var b))
            return false;
        int width = b.Right - b.Left;
        for (int sy = 0; sy < _pic.Height; sy++)
        {
            int ty = y + sy;
            if (ty < 0 || ty >= dest.Height)
                continue;
            for (int sx = 0; sx < width; sx++)
            {
                int tx = right - width + sx;
                if (tx < 0 || tx >= dest.Width)
                    continue;
                byte idx = _pic.Indices[sy * _pic.Width + b.Left + sx];
                if (idx == _transparentIndex)
                    continue;
                Rgb c = _palette[idx];
                int d = (ty * dest.Width + tx) * 4;
                dest.Pixels[d] = c.R;
                dest.Pixels[d + 1] = c.G;
                dest.Pixels[d + 2] = c.B;
                dest.Pixels[d + 3] = 255;
            }
        }
        return true;
    }
}

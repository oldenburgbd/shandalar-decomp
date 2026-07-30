using Shandalar.Core.Cards;
using Shandalar.Core.Formats;

namespace Shandalar.Core.Rendering;

/// <summary>
/// Cardart/Manasymbols.pic: a 342x18 strip of 19 18x18 cells — X, numerals 0..10, then
/// White, Red, Blue, Black, Green, tap, and an empty cell. Background (the palette index in
/// the top-left corner) is transparent when drawn.
/// </summary>
public sealed class ManaStrip
{
    public const int CellSize = 18;
    public const int CellX = 0;
    public const int CellWhite = 12;
    public const int CellRed = 13;
    public const int CellBlue = 14;
    public const int CellBlack = 15;
    public const int CellGreen = 16;
    public const int CellTap = 17;

    private readonly PicImage _pic;
    private readonly Palette _palette;
    private readonly byte _transparentIndex;

    public ManaStrip(PicImage pic, Palette palette)
    {
        _pic = pic;
        _palette = palette;
        _transparentIndex = pic.Indices[0];
    }

    /// <summary>Cell holding the numeral <paramref name="n"/> (0..10).</summary>
    public static int CellForNumeral(int n) =>
        n is >= 0 and <= 10 ? n + 1 : throw new ArgumentOutOfRangeException(nameof(n));

    /// <summary>
    /// The cell sequence for a mana cost, left to right: X or generic numeral, then
    /// W/U/B/R/G pips. A cost with no coloured pips and no X shows its numeral even when 0.
    /// </summary>
    public static int[] CostCells(ManaCost mana)
    {
        var cells = new List<int>();
        if (mana.IsVariable)
            cells.Add(CellX);
        else if (mana.Generic > 0 || mana.Colored == 0)
            cells.Add(CellForNumeral(Math.Min(mana.Generic, 10)));
        for (int i = 0; i < mana.White; i++)
            cells.Add(CellWhite);
        for (int i = 0; i < mana.Blue; i++)
            cells.Add(CellBlue);
        for (int i = 0; i < mana.Black; i++)
            cells.Add(CellBlack);
        for (int i = 0; i < mana.Red; i++)
            cells.Add(CellRed);
        for (int i = 0; i < mana.Green; i++)
            cells.Add(CellGreen);
        return [.. cells];
    }

    /// <summary>Draws one cell into <paramref name="dest"/> at (x, y), scaled to <paramref name="size"/>.</summary>
    public void DrawCell(RgbaImage dest, int cell, int x, int y, int size = CellSize)
    {
        int srcX = cell * CellSize;
        for (int dy = 0; dy < size; dy++)
        {
            int ty = y + dy;
            if (ty < 0 || ty >= dest.Height)
                continue;
            int sy = dy * CellSize / size;
            for (int dx = 0; dx < size; dx++)
            {
                int tx = x + dx;
                if (tx < 0 || tx >= dest.Width)
                    continue;
                int sx = srcX + dx * CellSize / size;
                byte idx = _pic.Indices[sy * _pic.Width + sx];
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
    }
}

using Shandalar.Core.Formats;

namespace Shandalar.Core.Rendering;

/// <summary>Lays .spr frames out on a transparent sprite sheet (used by extraction and the viewer).</summary>
public static class SpriteSheet
{
    /// <summary>
    /// Builds a grid sheet: uniform cells sized to the largest frame, at most 1024px wide,
    /// transparent background.
    /// </summary>
    public static RgbaImage Build(IReadOnlyList<SprFrame> frames, Palette palette)
    {
        int fw = frames.Max(f => f.Width), fh = frames.Max(f => f.Height);
        int cols = Math.Max(1, Math.Min(frames.Count, Math.Max(1, 1024 / Math.Max(1, fw))));
        int rows = (frames.Count + cols - 1) / cols;
        int sw = fw * cols, sh = fh * rows;
        var sheet = new byte[sw * sh * 4]; // transparent background

        for (int fi = 0; fi < frames.Count; fi++)
        {
            var f = frames[fi];
            byte[] rgba = f.ToRgba(palette);
            int ox = (fi % cols) * fw, oy = (fi / cols) * fh;
            for (int y = 0; y < f.Height; y++)
                for (int x = 0; x < f.Width; x++)
                {
                    int src = (y * f.Width + x) * 4;
                    if (rgba[src + 3] == 0)
                        continue;
                    int dst = ((oy + y) * sw + (ox + x)) * 4;
                    sheet[dst] = rgba[src];
                    sheet[dst + 1] = rgba[src + 1];
                    sheet[dst + 2] = rgba[src + 2];
                    sheet[dst + 3] = 255;
                }
        }
        return new RgbaImage(sw, sh, sheet);
    }
}

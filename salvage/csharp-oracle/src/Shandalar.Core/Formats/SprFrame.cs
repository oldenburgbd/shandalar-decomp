namespace Shandalar.Core.Formats;

/// <summary>
/// One decoded sprite frame: palette indices plus a parallel transparency mask (both row-major,
/// length Width*Height). The palette is external — supply one to <see cref="ToRgba"/>.
/// <see cref="Unknown1"/>/<see cref="Unknown2"/> are raw header fields, likely a draw anchor.
/// </summary>
public sealed class SprFrame
{
    public int Width { get; }
    public int Height { get; }
    public byte[] Indices { get; }
    public bool[] Transparent { get; }
    public int Unknown1 { get; }
    public int Unknown2 { get; }

    public SprFrame(int width, int height, byte[] indices, bool[] transparent, int u1, int u2)
    {
        Width = width;
        Height = height;
        Indices = indices;
        Transparent = transparent;
        Unknown1 = u1;
        Unknown2 = u2;
    }

    /// <summary>RGBA8888, row-major top-to-bottom; transparent pixels get alpha 0.</summary>
    public byte[] ToRgba(Palette palette)
    {
        var rgba = new byte[Width * Height * 4];
        for (int i = 0; i < Indices.Length; i++)
        {
            int o = i * 4;
            if (Transparent[i])
                continue; // leave (0,0,0,0)
            Rgb c = palette[Indices[i]];
            rgba[o] = c.R;
            rgba[o + 1] = c.G;
            rgba[o + 2] = c.B;
            rgba[o + 3] = 255;
        }
        return rgba;
    }
}

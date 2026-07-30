namespace Shandalar.Core.Formats;

/// <summary>
/// A decoded PIC image: palette indices plus the embedded palette, if the file had one.
/// External-palette files (a lone <c>X0</c> block) decode with <see cref="Palette"/> null;
/// supply a palette from a <c>.tr</c> file to <see cref="ToRgba"/>.
/// </summary>
public sealed class PicImage
{
    public int Width { get; }
    public int Height { get; }

    /// <summary>Palette indices, row-major, length <see cref="Width"/> * <see cref="Height"/>.</summary>
    public byte[] Indices { get; }

    /// <summary>The embedded palette, or null for external-palette files.</summary>
    public Palette? Palette { get; }

    public PicImage(int width, int height, byte[] indices, Palette? palette)
    {
        Width = width;
        Height = height;
        Indices = indices;
        Palette = palette;
    }

    /// <summary>
    /// Produces RGBA8888 pixels (row-major, top-to-bottom), using the embedded palette unless
    /// <paramref name="external"/> is given. Throws if neither is available.
    /// </summary>
    public byte[] ToRgba(Palette? external = null)
    {
        var pal = external ?? Palette
            ?? throw new InvalidOperationException("No palette: this PIC needs an external palette.");
        var rgba = new byte[Width * Height * 4];
        for (int i = 0; i < Indices.Length; i++)
        {
            Rgb c = pal[Indices[i]];
            int o = i * 4;
            rgba[o] = c.R;
            rgba[o + 1] = c.G;
            rgba[o + 2] = c.B;
            rgba[o + 3] = 255;
        }
        return rgba;
    }
}

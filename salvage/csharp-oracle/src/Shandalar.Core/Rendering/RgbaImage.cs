using Shandalar.Core.Formats;

namespace Shandalar.Core.Rendering;

/// <summary>An RGBA8888 bitmap, row-major top-to-bottom. The unit all composition works in.</summary>
public sealed record RgbaImage(int Width, int Height, byte[] Pixels)
{
    public static RgbaImage FromPic(PicImage pic, Palette? external = null) =>
        new(pic.Width, pic.Height, pic.ToRgba(external));

    public static RgbaImage FromWvl(WvlImage wvl) => new(wvl.Width, wvl.Height, wvl.Rgba);

    public RgbaImage Copy() => new(Width, Height, (byte[])Pixels.Clone());

    public byte[] ToPng() => PngWriter.EncodeRgba(Width, Height, Pixels);
}

/// <summary>An integer rectangle within a card face (see <see cref="CardFace"/>).</summary>
public readonly record struct FaceRect(int X, int Y, int Width, int Height);

/// <summary>Nearest-neighbour blitting; matches the original's unfiltered scaling.</summary>
internal static class Blit
{
    /// <summary>Scales <paramref name="src"/> into <paramref name="rect"/> of <paramref name="dest"/>.</summary>
    public static void Scaled(RgbaImage dest, RgbaImage src, FaceRect rect)
    {
        for (int dy = 0; dy < rect.Height; dy++)
        {
            int y = rect.Y + dy;
            if (y < 0 || y >= dest.Height)
                continue;
            int sy = dy * src.Height / rect.Height;
            for (int dx = 0; dx < rect.Width; dx++)
            {
                int x = rect.X + dx;
                if (x < 0 || x >= dest.Width)
                    continue;
                int sx = dx * src.Width / rect.Width;
                int s = (sy * src.Width + sx) * 4;
                int d = (y * dest.Width + x) * 4;
                dest.Pixels[d] = src.Pixels[s];
                dest.Pixels[d + 1] = src.Pixels[s + 1];
                dest.Pixels[d + 2] = src.Pixels[s + 2];
                dest.Pixels[d + 3] = 255;
            }
        }
    }
}

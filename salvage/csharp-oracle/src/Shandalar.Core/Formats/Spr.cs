namespace Shandalar.Core.Formats;

/// <summary>
/// Decoder for the MicroProse SPR sprite-sheet format. See docs/formats/spr.md.
/// A file is a sequence of frames terminated by a 0xFFFFFFFF size field; frame data is not
/// compressed, only a per-row transparency run scheme. Frames carry palette indices; the palette
/// is external.
/// </summary>
public static class Spr
{
    private const uint Terminator = 0xFFFFFFFF;
    private const int HeaderSize = 16;

    public static IReadOnlyList<SprFrame> Decode(byte[] data)
    {
        var frames = new List<SprFrame>();
        int off = 0;

        while (off + 4 <= data.Length)
        {
            uint size = ReadU32(data, off);
            if (size == Terminator)
                break;
            if (size < HeaderSize || off + size > data.Length)
                throw new InvalidDataException(
                    $"SPR frame at offset {off} has invalid size {size}");

            frames.Add(DecodeFrame(data, off, (int)size));
            off += (int)size;
        }

        return frames;
    }

    private static SprFrame DecodeFrame(byte[] data, int start, int size)
    {
        int end = start + size;
        int width = ReadU16(data, start + 4);
        int height = ReadU16(data, start + 6);
        int u1 = ReadU16(data, start + 8);
        int u2 = ReadU16(data, start + 10);
        int emptyLines = ReadU16(data, start + 12);

        var indices = new byte[width * height];
        var transparent = new bool[width * height];

        // Rows above emptyLines are fully transparent.
        int topTransparent = Math.Min(width * emptyLines, transparent.Length);
        for (int i = 0; i < topTransparent; i++)
            transparent[i] = true;

        int p = start + HeaderSize;
        int lastRowEnd = topTransparent;

        for (int y = emptyLines; y < height; y++)
        {
            if (p >= end)
                break;

            // Skip 0xFF padding; the first non-0xFF byte is the leading transparent count.
            int marker = -1;
            while (p < end)
            {
                marker = data[p++];
                if (marker != 0xFF)
                    break;
                marker = -1;
            }
            if (marker < 0 || p >= end)
                break;

            int leadingTransparent = marker;
            int control = data[p++];
            int dataPixels;
            if (control is 0xFE or 0xFF)
            {
                if (p >= end)
                    break;
                dataPixels = data[p++];
            }
            else
            {
                dataPixels = control;
            }

            // Out-of-range counts mean we've run into trailing padding after the last real row.
            // The original renderer draws onto a cleared buffer, so stop here and leave the rest
            // transparent (matches the reference decoder's bail-out; see docs/formats/spr.md).
            if (leadingTransparent > width || dataPixels > width)
                break;

            int rowBase = y * width;
            int col = 0;
            for (int i = 0; i < leadingTransparent && col < width; i++, col++)
                transparent[rowBase + col] = true;
            for (int i = 0; i < dataPixels && col < width && p < end; i++, col++)
            {
                byte v = data[p++];
                indices[rowBase + col] = v;
                if (v == 0x00)
                    transparent[rowBase + col] = true; // embedded transparency
            }
            for (; col < width; col++)
                transparent[rowBase + col] = true;

            lastRowEnd = rowBase + width;
        }

        for (int i = lastRowEnd; i < transparent.Length; i++)
            transparent[i] = true;

        return new SprFrame(width, height, indices, transparent, u1, u2);
    }

    private static uint ReadU32(byte[] d, int o) =>
        (uint)(d[o] | (d[o + 1] << 8) | (d[o + 2] << 16) | (d[o + 3] << 24));

    private static int ReadU16(byte[] d, int o) => d[o] | (d[o + 1] << 8);
}

namespace Shandalar.Core.Tests;

/// <summary>
/// Builds minimal, valid PICv3 byte streams for tests that must run in CI without the real game.
/// The image data is encoded as literal-only LZW (no dictionary references) over an identity RLE
/// stream, which is a valid PIC the production decoder handles like any other. It mirrors the
/// reference packer (lzw.py ints2bytes); the golden tests against the real install are what anchor
/// the decoder to the actual game encoder.
/// </summary>
internal static class SyntheticPic
{
    /// <summary>
    /// Assembles [M0|M1 palette][X0 image]. Pixel indices must avoid the RLE control byte 0x90
    /// (kept small in tests) so the RLE stage is an identity pass. <paramref name="storedPixels"/>
    /// may be shorter than width*height to exercise under-fill padding.
    /// </summary>
    public static byte[] Build(int width, int height, byte[] storedPixels, byte[] paletteRgb,
        bool sixBitPalette)
    {
        var file = new List<byte>();

        // Palette block: tag, u16 length, first, last, 256 RGB triples.
        file.Add((byte)'M');
        file.Add((byte)(sixBitPalette ? '0' : '1'));
        int palLen = 2 + 256 * 3;
        file.Add((byte)(palLen & 0xFF));
        file.Add((byte)(palLen >> 8));
        file.Add(0x00); // first
        file.Add(0xFF); // last
        file.AddRange(paletteRgb);

        // Image block: tag, u16 length (intentionally set to the real remaining length here),
        // u16 width, u16 height, max_bits, then LZW(RLE(pixels)).
        byte[] lz = PackLiterals(storedPixels, maxBits: 11);
        var img = new List<byte>();
        img.Add((byte)(width & 0xFF)); img.Add((byte)(width >> 8));
        img.Add((byte)(height & 0xFF)); img.Add((byte)(height >> 8));
        img.Add(11);
        img.AddRange(lz);

        file.Add((byte)'X');
        file.Add((byte)'0');
        int imgLen = img.Count;
        file.Add((byte)(imgLen & 0xFF));
        file.Add((byte)(imgLen >> 8));
        file.AddRange(img);

        return file.ToArray();
    }

    /// <summary>A 256-entry palette where entry i is (i, i, i) capped to the given max channel.</summary>
    public static byte[] GrayPalette(byte cap = 255)
    {
        var p = new byte[256 * 3];
        for (int i = 0; i < 256; i++)
        {
            byte v = (byte)Math.Min(i, cap);
            p[i * 3] = v; p[i * 3 + 1] = v; p[i * 3 + 2] = v;
        }
        return p;
    }

    /// <summary>
    /// Packs each byte as a literal LZW code using the reference variable-width, LSB-first schedule
    /// (9→11 bits, resetting at max_bits). Verbatim port of lzw.py ints2bytes.
    /// </summary>
    private static byte[] PackLiterals(byte[] values, int maxBits)
    {
        var output = new List<byte>();
        int usableBits = 0, usableBitCount = 0;
        int indicatorLength = 1, nextThreshold = 0x100, dicCounter = 0;
        int idx = 0, remaining = values.Length;

        while (remaining > 0)
        {
            while (usableBitCount < 8 && remaining > 0)
            {
                usableBits |= values[idx++] << usableBitCount;
                remaining--;
                usableBitCount += 8 + indicatorLength;
                if (++dicCounter == nextThreshold)
                {
                    dicCounter = 0;
                    indicatorLength++;
                    nextThreshold <<= 1;
                    if (8 + indicatorLength > maxBits)
                    {
                        dicCounter = 0;
                        indicatorLength = 1;
                        nextThreshold = 0x100;
                    }
                }
            }
            while (usableBitCount >= 8)
            {
                output.Add((byte)(usableBits & 0xFF));
                usableBits >>= 8;
                usableBitCount -= 8;
            }
        }
        if (usableBitCount > 0)
            output.Add((byte)(usableBits & 0xFF));
        return output.ToArray();
    }
}

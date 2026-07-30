namespace Shandalar.Core.Formats;

/// <summary>
/// Decoder for the MicroProse PICv3 image format. See docs/formats/pic.md for the full spec.
/// A .pic file is a sequence of blocks; we collect an optional palette block (M0/M1) and decode
/// the first image block (X0/X1), which is terminal in this game's files.
/// </summary>
public static class Pic
{
    private const byte RleControl = 0x90;

    public static PicImage Decode(byte[] data) => Decode(data.AsSpan());

    public static PicImage Decode(ReadOnlySpan<byte> data)
    {
        Palette? palette = null;
        int off = 0;

        while (off + 4 <= data.Length)
        {
            char t0 = (char)data[off], t1 = (char)data[off + 1];
            int length = data[off + 2] | (data[off + 3] << 8);

            if (t0 == 'M' && (t1 == '0' || t1 == '1'))
            {
                palette = ReadPalette(data.Slice(off + 4), sixBit: t1 == '0');
                // Palette blocks are always < 64 KB, so length is reliable here (see Q-002).
                off += 4 + length;
            }
            else if (t0 == 'X' && (t1 == '0' || t1 == '1'))
            {
                return ReadImage(data.Slice(off + 4), fourBit: t1 == '1', palette);
            }
            else
            {
                throw new InvalidDataException(
                    $"Unknown PIC block tag '{t0}{t1}' at offset {off}");
            }
        }

        throw new InvalidDataException("PIC file contains no image (X0/X1) block");
    }

    private static Palette ReadPalette(ReadOnlySpan<byte> data, bool sixBit)
    {
        int first = data[0], last = data[1];
        var entries = new Rgb[Palette.EntryCount];
        int p = 2;
        for (int i = first; i <= last; i++)
        {
            byte r = data[p], g = data[p + 1], b = data[p + 2];
            p += 3;
            if (sixBit)
            {
                r = Palette.ScaleSixToEight(r);
                g = Palette.ScaleSixToEight(g);
                b = Palette.ScaleSixToEight(b);
            }
            entries[i] = new Rgb(r, g, b);
        }
        return new Palette(entries, sixBit);
    }

    private static PicImage ReadImage(ReadOnlySpan<byte> data, bool fourBit, Palette? palette)
    {
        int width = data[0] | (data[1] << 8);
        int height = data[2] | (data[3] << 8);
        int maxBits = data[4];
        // Ignore any length field: it wraps mod 65536 for >64 KB blocks (Q-002). The compressed
        // stream runs to end-of-file; we bound output by width*height instead.
        ReadOnlySpan<byte> stream = data.Slice(5);

        byte[] unpacked = RleDecode(LzwUnpack(stream, maxBits));

        int pixelCount = width * height;
        var indices = new byte[pixelCount];
        if (fourBit)
        {
            // Two pixels per byte: low nibble = left, high nibble = right; rows byte-padded,
            // so each row consumes ceil(width/2) bytes.
            int src = 0;
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width && src < unpacked.Length; x += 2, src++)
                {
                    byte pair = unpacked[src];
                    indices[y * width + x] = (byte)(pair & 0x0F);
                    if (x + 1 < width)
                        indices[y * width + x + 1] = (byte)(pair >> 4);
                }
            }
        }
        else
        {
            int n = Math.Min(pixelCount, unpacked.Length);
            Array.Copy(unpacked, indices, n); // under-fill leaves trailing index 0 (see spec)
        }

        return new PicImage(width, height, indices, palette);
    }

    /// <summary>
    /// LZW stage: unpacks the variable-width (9→11 bit, LSB-first) code stream and runs the
    /// dictionary decode, returning the RLE-compressed byte stream. Ported from the reference
    /// decoder (benprew/mp_pic_tools lzw.py: bytes2ints + decode); see docs/formats/pic.md.
    /// </summary>
    private static byte[] LzwUnpack(ReadOnlySpan<byte> data, int maxBits)
    {
        // --- bytes2ints: bit-unpack into variable-width codes ---
        var codes = new List<int>(data.Length);
        int usableBits = 0, usableBitCount = 0;
        int indicatorLength = 1, indicatorFlag = 0x001, nextThreshold = 0x100, counter = 0;
        int pos = 0;

        while (pos < data.Length || usableBitCount >= 8 + indicatorLength)
        {
            while (usableBitCount < 8 + indicatorLength && pos < data.Length)
            {
                usableBits |= data[pos++] << usableBitCount;
                usableBitCount += 8;
            }
            if (usableBitCount < 8 + indicatorLength)
                break;

            while (usableBitCount >= 8 + indicatorLength)
            {
                int code = usableBits & (((indicatorFlag << 8) & 0xFF00) | 0x00FF);
                usableBits >>= 8 + indicatorLength;
                usableBitCount -= 8 + indicatorLength;

                if (++counter == nextThreshold)
                {
                    counter = 0;
                    indicatorLength++;
                    indicatorFlag = (indicatorFlag << 1) | 1;
                    nextThreshold <<= 1;
                    if (8 + indicatorLength > maxBits)
                    {
                        counter = 0;
                        indicatorLength = 1;
                        indicatorFlag = 0x001;
                        nextThreshold = 0x100;
                    }
                }
                codes.Add(code);
            }
        }

        // --- decode: LZW dictionary expansion, rebuilding the dictionary when it fills ---
        int dictCap = 1 << maxBits;
        var outp = new List<byte>(codes.Count * 2);
        var table = new List<byte[]>(dictCap);

        int i = 0;
        while (i < codes.Count)
        {
            table.Clear();
            for (int c = 0; c < 256; c++)
                table.Add([(byte)c]);
            table.Add([]); // index 256, reserved
            int curPos = 0x101; // entries start at 257

            byte[] w = [(byte)codes[i]];
            outp.Add((byte)codes[i]);

            while (curPos < dictCap && i < codes.Count - 1)
            {
                i++;
                int k = codes[i];
                byte[] entry;
                if (k < curPos)
                    entry = table[k];
                else if (k == curPos)
                    entry = Append(w, w[0]);
                else
                    throw new InvalidDataException($"Bad LZW code {k} at position {i}");
                if (entry.Length == 0)
                    throw new InvalidDataException($"Empty LZW entry (reserved code) at {i}");

                outp.AddRange(entry);
                if (curPos < dictCap)
                {
                    table.Add(Append(w, entry[0]));
                    curPos++;
                }
                w = entry;
            }
            i++;
        }
        return outp.ToArray();

        static byte[] Append(byte[] a, byte b)
        {
            var r = new byte[a.Length + 1];
            Array.Copy(a, r, a.Length);
            r[^1] = b;
            return r;
        }
    }

    /// <summary>
    /// RLE stage (control byte 0x90). <c>0x90 0x00</c> is a literal 0x90; <c>0x90 CC</c> repeats
    /// the previous byte until the run totals CC bytes; the first byte is emitted literally.
    /// </summary>
    internal static byte[] RleDecode(byte[] coded)
    {
        if (coded.Length == 0)
            return [];
        var outp = new List<byte>(coded.Length * 2) { coded[0] };
        int i = 1;
        while (i < coded.Length)
        {
            if (coded[i] != RleControl)
            {
                outp.Add(coded[i]);
            }
            else
            {
                byte next = i + 1 < coded.Length ? coded[i + 1] : (byte)0;
                if (next == 0x00)
                {
                    outp.Add(RleControl);
                }
                else
                {
                    byte val = outp[^1];
                    for (int r = 0; r < next - 1; r++)
                        outp.Add(val);
                }
                i++;
            }
            i++;
        }
        return outp.ToArray();
    }
}

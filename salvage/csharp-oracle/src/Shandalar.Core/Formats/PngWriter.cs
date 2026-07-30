using System.IO.Compression;

namespace Shandalar.Core.Formats;

/// <summary>
/// Minimal PNG encoder: RGBA8888 in, standard non-interlaced PNG out (IHDR + zlib IDAT + IEND).
/// Pure bytes-to-bytes so extraction output is deterministic and testable.
/// </summary>
public static class PngWriter
{
    private static readonly uint[] CrcTable = BuildCrcTable();

    public static byte[] EncodeRgba(int width, int height, byte[] rgba)
    {
        if (rgba.Length != width * height * 4)
            throw new ArgumentException("rgba length must be width*height*4");

        // Raw scanlines, each prefixed with filter byte 0 (None).
        var raw = new byte[height * (1 + width * 4)];
        for (int y = 0; y < height; y++)
        {
            int src = y * width * 4;
            int dst = y * (1 + width * 4) + 1;
            Array.Copy(rgba, src, raw, dst, width * 4);
        }

        using var idat = new MemoryStream();
        using (var z = new ZLibStream(idat, CompressionLevel.Optimal, leaveOpen: true))
            z.Write(raw);

        var ihdr = new byte[13];
        WriteBE(ihdr, 0, (uint)width);
        WriteBE(ihdr, 4, (uint)height);
        ihdr[8] = 8;   // bit depth
        ihdr[9] = 6;   // colour type: RGBA
        // compression 0, filter 0, interlace 0

        using var outp = new MemoryStream();
        outp.Write([0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A]); // PNG signature
        WriteChunk(outp, "IHDR", ihdr);
        WriteChunk(outp, "IDAT", idat.ToArray());
        WriteChunk(outp, "IEND", []);
        return outp.ToArray();
    }

    private static void WriteChunk(Stream s, string type, byte[] data)
    {
        var len = new byte[4];
        WriteBE(len, 0, (uint)data.Length);
        s.Write(len);

        var typeBytes = System.Text.Encoding.ASCII.GetBytes(type);
        s.Write(typeBytes);
        s.Write(data);

        uint crc = 0xFFFFFFFF;
        foreach (byte b in typeBytes)
            crc = CrcTable[(crc ^ b) & 0xFF] ^ (crc >> 8);
        foreach (byte b in data)
            crc = CrcTable[(crc ^ b) & 0xFF] ^ (crc >> 8);
        var crcBytes = new byte[4];
        WriteBE(crcBytes, 0, crc ^ 0xFFFFFFFF);
        s.Write(crcBytes);
    }

    private static void WriteBE(byte[] b, int o, uint v)
    {
        b[o] = (byte)(v >> 24);
        b[o + 1] = (byte)(v >> 16);
        b[o + 2] = (byte)(v >> 8);
        b[o + 3] = (byte)v;
    }

    private static uint[] BuildCrcTable()
    {
        var table = new uint[256];
        for (uint n = 0; n < 256; n++)
        {
            uint c = n;
            for (int k = 0; k < 8; k++)
                c = (c & 1) != 0 ? 0xEDB88320 ^ (c >> 1) : c >> 1;
            table[n] = c;
        }
        return table;
    }
}

namespace Shandalar.Core.Tests;

/// <summary>Builds minimal, valid SPR byte streams for CI tests (no real game files needed).</summary>
internal static class SyntheticSpr
{
    internal readonly record struct Row(int LeadingTransparent, byte[] Pixels);

    internal sealed record Frame(int Width, int Height, int U1, int U2, int EmptyLines, Row[] Rows);

    public static byte[] Build(params Frame[] frames)
    {
        var file = new List<byte>();
        foreach (var f in frames)
        {
            var body = new List<byte>();
            foreach (var row in f.Rows)
            {
                if (row.Pixels.Length >= 0xFE)
                    throw new ArgumentException("test rows keep data < 0xFE");
                body.Add((byte)row.LeadingTransparent);
                body.Add((byte)row.Pixels.Length); // control byte doubles as the pixel count
                body.AddRange(row.Pixels);
            }

            int size = 16 + body.Count;
            AddU32(file, (uint)size);
            AddU16(file, f.Width);
            AddU16(file, f.Height);
            AddU16(file, f.U1);
            AddU16(file, f.U2);
            AddU16(file, f.EmptyLines);
            AddU16(file, 0); // cutoff_offset_y
            file.AddRange(body);
        }
        AddU32(file, 0xFFFFFFFF); // terminator
        return file.ToArray();
    }

    private static void AddU16(List<byte> l, int v) { l.Add((byte)(v & 0xFF)); l.Add((byte)(v >> 8)); }
    private static void AddU32(List<byte> l, uint v)
    {
        l.Add((byte)(v & 0xFF)); l.Add((byte)((v >> 8) & 0xFF));
        l.Add((byte)((v >> 16) & 0xFF)); l.Add((byte)((v >> 24) & 0xFF));
    }
}

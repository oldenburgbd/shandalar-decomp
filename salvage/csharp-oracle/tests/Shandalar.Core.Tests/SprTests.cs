using Shandalar.Core.Formats;
using Xunit;
using Frame = Shandalar.Core.Tests.SyntheticSpr.Frame;
using Row = Shandalar.Core.Tests.SyntheticSpr.Row;

namespace Shandalar.Core.Tests;

public class SprTests
{
    [Fact]
    public void DecodesMultipleFramesUpToTerminator()
    {
        byte[] file = SyntheticSpr.Build(
            new Frame(4, 3, U1: 11, U2: 22, EmptyLines: 1, Rows:
            [
                new Row(1, [5, 6]),      // y=1: [_,5,6,_]
                new Row(0, [7, 0, 8]),   // y=2: [7,(0=T),8,_]
            ]),
            new Frame(2, 2, U1: 0, U2: 0, EmptyLines: 0, Rows:
            [
                new Row(0, [9, 10]),
                new Row(1, [11]),
            ]));

        var frames = Spr.Decode(file);

        Assert.Equal(2, frames.Count);

        SprFrame f0 = frames[0];
        Assert.Equal(4, f0.Width);
        Assert.Equal(3, f0.Height);
        Assert.Equal(11, f0.Unknown1);
        Assert.Equal(22, f0.Unknown2);
        // Row 0 fully transparent (empty_lines_above = 1).
        Assert.All(Enumerable.Range(0, 4), i => Assert.True(f0.Transparent[i]));
        // Row 1: [_,5,6,_]
        Assert.True(f0.Transparent[4]);
        Assert.Equal(5, f0.Indices[5]);
        Assert.Equal(6, f0.Indices[6]);
        Assert.True(f0.Transparent[7]);
        // Row 2: [7,(embedded 0 -> transparent),8,_]
        Assert.False(f0.Transparent[8]);
        Assert.Equal(7, f0.Indices[8]);
        Assert.True(f0.Transparent[9]); // index 0 embedded transparency
        Assert.Equal(8, f0.Indices[10]);
        Assert.True(f0.Transparent[11]);

        SprFrame f1 = frames[1];
        Assert.Equal(2, f1.Width);
        Assert.Equal(9, f1.Indices[0]);
        Assert.Equal(10, f1.Indices[1]);
        Assert.True(f1.Transparent[2]);
        Assert.Equal(11, f1.Indices[3]);
    }

    [Fact]
    public void ToRgbaSetsAlphaZeroForTransparentPixels()
    {
        byte[] file = SyntheticSpr.Build(
            new Frame(2, 1, 0, 0, 0, [new Row(1, [4])])); // [_,4]
        var palette = new Palette(BuildGray(), wasSixBit: false);

        byte[] rgba = Spr.Decode(file)[0].ToRgba(palette);

        Assert.Equal(0, rgba[3]);           // pixel 0 transparent -> alpha 0
        Assert.Equal([4, 4, 4, 255], rgba[4..8]); // pixel 1 -> gray(4)
    }

    [Fact]
    public void ControlByte0xFEReadsExplicitPixelCount()
    {
        // Hand-build a frame whose control byte is 0xFE, forcing an extra length byte.
        var file = new List<byte>();
        var body = new byte[] { 0x00, 0xFE, 0x02, 0x0A, 0x0B }; // lead 0, ctrl 0xFE, len 2, pixels
        AddU32(file, (uint)(16 + body.Length));
        AddU16(file, 3); AddU16(file, 1); AddU16(file, 0); AddU16(file, 0); AddU16(file, 0); AddU16(file, 0);
        file.AddRange(body);
        AddU32(file, 0xFFFFFFFF);

        var f = Spr.Decode(file.ToArray())[0];

        Assert.Equal(0x0A, f.Indices[0]);
        Assert.Equal(0x0B, f.Indices[1]);
        Assert.True(f.Transparent[2]);

        static void AddU16(List<byte> l, int v) { l.Add((byte)(v & 0xFF)); l.Add((byte)(v >> 8)); }
        static void AddU32(List<byte> l, uint v)
        {
            l.Add((byte)(v & 0xFF)); l.Add((byte)((v >> 8) & 0xFF));
            l.Add((byte)((v >> 16) & 0xFF)); l.Add((byte)((v >> 24) & 0xFF));
        }
    }

    private static Rgb[] BuildGray()
    {
        var p = new Rgb[256];
        for (int i = 0; i < 256; i++) p[i] = new Rgb((byte)i, (byte)i, (byte)i);
        return p;
    }
}

public class TrPaletteTests
{
    [Fact]
    public void ParsesIndexAndScalesSixBitRgb()
    {
        var pal = TrPalette.Load(new StringReader(
            "  1 -  63  0  0  - 0 0 0\n" +
            " 10 -  31 31 31 - 0 0 7 7\n" +
            "garbage line\n" +
            "255 - 0 0 63\n"));

        Assert.Equal(new Rgb(255, 0, 0), pal[1]);           // 63 -> 255
        Assert.Equal(new Rgb(125, 125, 125), pal[10]);      // 31 -> 125
        Assert.Equal(new Rgb(0, 0, 255), pal[255]);
        Assert.Equal(new Rgb(0, 0, 0), pal[2]);             // unspecified -> black
        Assert.True(pal.WasSixBit);
    }
}

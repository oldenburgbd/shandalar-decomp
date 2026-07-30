using Shandalar.Core.Formats;
using Xunit;

namespace Shandalar.Core.Tests;

public class PlogPaletteTests
{
    private static byte[] BuildPlog(params (byte r, byte g, byte b, byte flag)[] firstEntries)
    {
        var data = new byte[PlogPalette.HeaderSize + 256 * 4];
        data[1] = 3; data[2] = 0; data[3] = 1; // header: byte1=3, count=256 (0x0100 LE)
        for (int i = 0; i < firstEntries.Length; i++)
        {
            int o = PlogPalette.HeaderSize + i * 4;
            data[o] = firstEntries[i].r; data[o + 1] = firstEntries[i].g;
            data[o + 2] = firstEntries[i].b; data[o + 3] = firstEntries[i].flag;
        }
        return data;
    }

    [Fact]
    public void ReadsIndexOrderedEightBitEntries()
    {
        byte[] data = BuildPlog((0, 0, 0, 0), (28, 24, 26, 1), (78, 18, 14, 1));
        Palette pal = PlogPalette.Parse(data);

        Assert.False(pal.WasSixBit);            // 8-bit, no scaling
        Assert.Equal(new Rgb(0, 0, 0), pal[0]);
        Assert.Equal(new Rgb(28, 24, 26), pal[1]);
        Assert.Equal(new Rgb(78, 18, 14), pal[2]); // 78 > 63 confirms 8-bit
    }

    [Fact]
    public void RejectsTruncatedFile()
    {
        Assert.Throws<InvalidDataException>(() => PlogPalette.Parse(new byte[100]));
    }
}

public class PaletteOverlayTests
{
    private static Palette SolidGray()
    {
        var e = new Rgb[256];
        for (int i = 0; i < 256; i++) e[i] = new Rgb(50, 50, 50);
        return new Palette(e, wasSixBit: false);
    }

    [Fact]
    public void WithOverridesReplacesOnlyGivenEntries()
    {
        Palette result = SolidGray().WithOverrides([(5, new Rgb(255, 0, 0)), (200, new Rgb(0, 255, 0))]);

        Assert.Equal(new Rgb(255, 0, 0), result[5]);
        Assert.Equal(new Rgb(0, 255, 0), result[200]);
        Assert.Equal(new Rgb(50, 50, 50), result[6]); // untouched
    }

    [Fact]
    public void TrOverlayLayersPartialPaletteOnBase()
    {
        // A partial .tr defining just two entries (6-bit -> 8-bit).
        var tr = new StringReader("  5 - 63  0  0 - x\n200 - 0 63 0\n");
        var overrides = TrPalette.ReadEntries(tr);
        Palette result = SolidGray().WithOverrides(overrides);

        Assert.Equal(new Rgb(255, 0, 0), result[5]);   // 63 -> 255
        Assert.Equal(new Rgb(0, 255, 0), result[200]);
        Assert.Equal(new Rgb(50, 50, 50), result[7]);  // base preserved
    }
}

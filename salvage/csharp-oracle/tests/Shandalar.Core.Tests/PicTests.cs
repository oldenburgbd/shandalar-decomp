using Shandalar.Core.Formats;
using Xunit;

namespace Shandalar.Core.Tests;

public class PicRleTests
{
    [Fact]
    public void PassesThroughLiteralBytes()
    {
        Assert.Equal([1, 2, 3, 4], Pic.RleDecode([1, 2, 3, 4]));
    }

    [Fact]
    public void ExpandsRunToTotalCount()
    {
        // 0x41 literal, then 0x90 0x05 => 'A' repeated to a total run of 5.
        Assert.Equal([0x41, 0x41, 0x41, 0x41, 0x41], Pic.RleDecode([0x41, 0x90, 0x05]));
    }

    [Fact]
    public void EscapesLiteralControlByte()
    {
        // 0x90 0x00 is a literal 0x90.
        Assert.Equal([0x41, 0x90], Pic.RleDecode([0x41, 0x90, 0x00]));
    }

    [Fact]
    public void EmptyInputYieldsEmptyOutput()
    {
        Assert.Empty(Pic.RleDecode([]));
    }
}

public class PicDecodeTests
{
    private static byte[] SmallImagePixels(int count)
    {
        // Values 0..15 cycle — never 0x90, so the RLE stage is an identity pass.
        var px = new byte[count];
        for (int i = 0; i < count; i++)
            px[i] = (byte)(i % 16);
        return px;
    }

    [Fact]
    public void DecodesDimensionsAndIndicesFromSyntheticFile()
    {
        int w = 6, h = 4;
        byte[] px = SmallImagePixels(w * h);
        byte[] file = SyntheticPic.Build(w, h, px, SyntheticPic.GrayPalette(), sixBitPalette: false);

        PicImage img = Pic.Decode(file);

        Assert.Equal(w, img.Width);
        Assert.Equal(h, img.Height);
        Assert.Equal(px, img.Indices);
    }

    [Fact]
    public void ToRgbaUsesEmbeddedPalette()
    {
        byte[] px = [0, 1, 2, 3];
        byte[] file = SyntheticPic.Build(2, 2, px, SyntheticPic.GrayPalette(), sixBitPalette: false);

        byte[] rgba = Pic.Decode(file).ToRgba();

        // Gray palette: index i -> (i,i,i,255).
        Assert.Equal([0, 0, 0, 255], rgba[0..4]);
        Assert.Equal([1, 1, 1, 255], rgba[4..8]);
        Assert.Equal([3, 3, 3, 255], rgba[12..16]);
    }

    [Fact]
    public void M0PaletteIsScaledFromSixBitToEightBit()
    {
        // Gray palette capped at 63: entry 63 has value 63 (6-bit max) -> should scale to 255.
        byte[] px = [63, 63, 63, 63];
        byte[] file = SyntheticPic.Build(2, 2, px, SyntheticPic.GrayPalette(cap: 63), sixBitPalette: true);

        PicImage img = Pic.Decode(file);

        Assert.True(img.Palette!.WasSixBit);
        Assert.Equal(new Rgb(255, 255, 255), img.Palette[63]);
        Assert.Equal(new Rgb(0, 0, 0), img.Palette[0]);
    }

    [Fact]
    public void M1PaletteIsUsedWithoutScaling()
    {
        byte[] file = SyntheticPic.Build(2, 2, [1, 1, 1, 1], SyntheticPic.GrayPalette(), sixBitPalette: false);

        PicImage img = Pic.Decode(file);

        Assert.False(img.Palette!.WasSixBit);
        Assert.Equal(new Rgb(1, 1, 1), img.Palette[1]);
    }

    [Fact]
    public void UnderfilledStreamPadsRemainderWithIndexZero()
    {
        // Declare a 4x4 image but only supply 10 pixels; the rest must be index 0.
        int w = 4, h = 4;
        byte[] stored = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
        byte[] file = SyntheticPic.Build(w, h, stored, SyntheticPic.GrayPalette(), sixBitPalette: false);

        PicImage img = Pic.Decode(file);

        Assert.Equal(w * h, img.Indices.Length);
        Assert.Equal(stored, img.Indices[..10]);
        Assert.All(img.Indices[10..], b => Assert.Equal(0, b));
    }

    [Fact]
    public void RejectsFileWithNoImageBlock()
    {
        // Palette block only, then EOF.
        byte[] palOnly = SyntheticPic.Build(1, 1, [0], SyntheticPic.GrayPalette(), sixBitPalette: false);
        // Truncate to just the 774-byte palette block.
        byte[] palBlock = palOnly[..774];
        Assert.Throws<InvalidDataException>(() => Pic.Decode(palBlock));
    }
}

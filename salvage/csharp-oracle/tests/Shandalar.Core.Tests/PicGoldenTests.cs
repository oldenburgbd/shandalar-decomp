using System.Security.Cryptography;
using Shandalar.Core;
using Shandalar.Core.Formats;
using Xunit;

namespace Shandalar.Core.Tests;

/// <summary>
/// Golden tests decoding the real install's .pic files. Skipped automatically in CI
/// (SHANDALAR_GAME_DIR unset). See docs/formats/pic.md.
/// </summary>
public class PicGoldenTests
{
    private static GameInstall RequireInstall()
    {
        var install = GameInstall.FromEnvironment();
        Skip.If(install is null, $"{GameInstall.EnvVar} is not set");
        return install!;
    }

    [SkippableFact]
    public void EveryBaseDirPicDecodesToFullDimensions()
    {
        var install = RequireInstall();
        var files = Directory.GetFiles(install.Root, "*.pic");
        Assert.True(files.Length >= 90, $"expected the base dir's ~99 .pic files, found {files.Length}");

        foreach (var path in files)
        {
            PicImage img = Pic.Decode(File.ReadAllBytes(path));
            Assert.True(img.Width > 0 && img.Height > 0, $"{Path.GetFileName(path)} has no dimensions");
            Assert.Equal(img.Width * img.Height, img.Indices.Length);
        }
    }

    [SkippableFact]
    public void TitleDecodesTo640x480With8BitPalette()
    {
        var install = RequireInstall();
        PicImage img = Pic.Decode(File.ReadAllBytes(install.PathTo("Title.pic")));

        Assert.Equal(640, img.Width);
        Assert.Equal(480, img.Height);
        Assert.NotNull(img.Palette);
        Assert.False(img.Palette!.WasSixBit); // Title.pic is M1
        Assert.Equal(640 * 480 * 4, img.ToRgba().Length);
    }

    [SkippableFact]
    public void DungeonUsesSixBitM0Palette()
    {
        var install = RequireInstall();
        PicImage img = Pic.Decode(File.ReadAllBytes(install.PathTo("Dungeon.pic")));

        Assert.Equal(640, img.Width);
        Assert.Equal(480, img.Height);
        Assert.True(img.Palette!.WasSixBit); // Dungeon.pic is M0
    }

    [SkippableFact]
    public void ExternalPaletteFileHasNoEmbeddedPalette()
    {
        var install = RequireInstall();
        // Dun_Bar.pic is a lone X0 block (external palette).
        PicImage img = Pic.Decode(File.ReadAllBytes(install.PathTo("Dun_Bar.pic")));

        Assert.Null(img.Palette);
        Assert.Equal(640, img.Width);
        Assert.Equal(480, img.Height);
        Assert.Throws<InvalidOperationException>(() => img.ToRgba()); // needs an external palette
    }

    [SkippableFact]
    public void TitleRgbaOutputIsStable()
    {
        var install = RequireInstall();
        byte[] rgba = Pic.Decode(File.ReadAllBytes(install.PathTo("Title.pic"))).ToRgba();
        string hash = Convert.ToHexString(SHA256.HashData(rgba)).ToLowerInvariant();

        // Pins the exact decoded pixels of the title screen against regressions.
        Assert.Equal("837c16e07c21e8dae83fd41e22766f0b0e6a31fd39599d68bc8068c138fb5096", hash);
    }
}

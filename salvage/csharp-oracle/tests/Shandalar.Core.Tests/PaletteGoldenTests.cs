using System.Security.Cryptography;
using Shandalar.Core;
using Shandalar.Core.Formats;
using Xunit;

namespace Shandalar.Core.Tests;

/// <summary>
/// Golden tests for the real install's palette files. Skipped in CI (SHANDALAR_GAME_DIR unset).
/// See docs/formats/palettes.md.
/// </summary>
public class PaletteGoldenTests
{
    private static GameInstall RequireInstall()
    {
        var install = GameInstall.FromEnvironment();
        Skip.If(install is null, $"{GameInstall.EnvVar} is not set");
        return install!;
    }

    [SkippableFact]
    public void DuelPlogpalDecodesKnownEntries()
    {
        var install = RequireInstall();
        Palette pal = PlogPalette.Load(install.PathTo("Duel.plogpal"));

        Assert.False(pal.WasSixBit);
        Assert.Equal(new Rgb(0, 0, 0), pal[0]);
        Assert.Equal(new Rgb(28, 24, 26), pal[1]);
        Assert.Equal(new Rgb(78, 18, 14), pal[2]);
    }

    [SkippableTheory]
    [InlineData("Advpal.tr", 11)]
    [InlineData("Todpal.tr", 252)]
    public void TrFilesDefineExpectedEntryCounts(string name, int expected)
    {
        var install = RequireInstall();
        using var reader = new StreamReader(install.PathTo(name));
        var entries = TrPalette.ReadEntries(reader);
        Assert.Equal(expected, entries.Count);
    }

    [SkippableFact]
    public void FromFileDispatchesByExtension()
    {
        var install = RequireInstall();
        Assert.False(Palette.FromFile(install.PathTo("Duel.plogpal")).WasSixBit); // .plogpal -> 8-bit
        Assert.True(Palette.FromFile(install.PathTo("Todpal.tr")).WasSixBit);      // .tr -> 6-bit
        Assert.False(Palette.FromFile(install.PathTo("Winbak01.pic")).WasSixBit);  // .pic M1 -> 8-bit
    }

    [SkippableFact]
    public void CreatureArtRendersStablyWithDuelPalette()
    {
        var install = RequireInstall();
        Palette pal = Palette.FromFile(install.PathTo("Duel.plogpal"));
        byte[] rgba = Pic.Decode(File.ReadAllBytes(install.PathTo("Creatures640.pic"))).ToRgba(pal);
        string hash = Convert.ToHexString(SHA256.HashData(rgba)).ToLowerInvariant();

        Assert.Equal("fa001c406ad14776208d24480f6b9d70d5efe4119f57cf5cfa8949698fc4b730", hash);
    }
}

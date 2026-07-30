using System.Security.Cryptography;
using Shandalar.Core;
using Shandalar.Core.Formats;
using Xunit;

namespace Shandalar.Core.Tests;

/// <summary>
/// Golden tests decoding the real install's .spr files. Skipped in CI (SHANDALAR_GAME_DIR unset).
/// See docs/formats/spr.md.
/// </summary>
public class SprGoldenTests
{
    private static GameInstall RequireInstall()
    {
        var install = GameInstall.FromEnvironment();
        Skip.If(install is null, $"{GameInstall.EnvVar} is not set");
        return install!;
    }

    [SkippableTheory]
    [InlineData("Worlds.spr", 60)]
    [InlineData("Icons.spr", 24)]
    [InlineData("Compass.spr", 5)]
    [InlineData("S_Moon.spr", 16)]
    [InlineData("Days.spr", 7)]
    public void KnownFilesHaveExpectedFrameCounts(string name, int expected)
    {
        var install = RequireInstall();
        var frames = Spr.Decode(File.ReadAllBytes(install.PathTo(name)));
        Assert.Equal(expected, frames.Count);
        foreach (var f in frames)
        {
            Assert.True(f.Width > 0 && f.Height > 0);
            Assert.Equal(f.Width * f.Height, f.Indices.Length);
            Assert.Equal(f.Indices.Length, f.Transparent.Length);
        }
    }

    [SkippableFact]
    public void EveryBaseDirSprDecodes()
    {
        var install = RequireInstall();
        var files = Directory.GetFiles(install.Root, "*.spr");
        Assert.True(files.Length >= 30, $"expected ~35 base-dir .spr, found {files.Length}");

        foreach (var path in files)
        {
            var frames = Spr.Decode(File.ReadAllBytes(path));
            Assert.NotEmpty(frames);
        }
    }

    [SkippableFact]
    public void WorldsRendersStablyWithWinbak01Palette()
    {
        var install = RequireInstall();
        Palette palette = Pic.Decode(File.ReadAllBytes(install.PathTo("Winbak01.pic"))).Palette!;
        var frames = Spr.Decode(File.ReadAllBytes(install.PathTo("Worlds.spr")));

        using var sha = SHA256.Create();
        foreach (var f in frames)
        {
            byte[] rgba = f.ToRgba(palette);
            sha.TransformBlock(rgba, 0, rgba.Length, null, 0);
        }
        sha.TransformFinalBlock([], 0, 0);
        string hash = Convert.ToHexString(sha.Hash!).ToLowerInvariant();

        Assert.Equal("bcf1f6b370c39b5f6562376504c67dc6dfc451f7a0924e67bba0bb4914789d15", hash);
    }
}

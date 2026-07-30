using System.Security.Cryptography;
using Shandalar.Core;
using Shandalar.Core.Formats;
using Xunit;

namespace Shandalar.Core.Tests;

/// <summary>
/// Golden tests decoding the real card-art archives. Skipped in CI (SHANDALAR_GAME_DIR unset).
/// See docs/formats/cat.md.
/// </summary>
public class CatGoldenTests
{
    private static GameInstall RequireInstall()
    {
        var install = GameInstall.FromEnvironment();
        Skip.If(install is null, $"{GameInstall.EnvVar} is not set");
        return install!;
    }

    [SkippableFact]
    public void ArchivesHaveExpectedEntryCounts()
    {
        var install = RequireInstall();
        Assert.Equal(944, install.LoadSmallArt().Entries.Count);
        Assert.Equal(943, install.LoadMediumArt().Entries.Count);
    }

    [SkippableFact]
    public void AirElementalSmallArtDecodesStably()
    {
        var entry = RequireInstall().LoadSmallArt().FindByCardId(0)!;
        Assert.Equal("c0000.tif", entry.Name);

        WvlImage img = entry.Decode();
        Assert.Equal(144, img.Width);
        Assert.Equal(116, img.Height);
        Assert.Equal("5b19b72a59eab4aebc7c231af6c52b57873bbd01b69f4256ad5d07e52ecde10e",
            Convert.ToHexString(SHA256.HashData(img.Rgba)).ToLowerInvariant());
    }

    [SkippableFact]
    public void SerraAngelMediumArtDecodesStably()
    {
        WvlImage img = RequireInstall().LoadMediumArt().FindByCardId(221)!.Decode();
        Assert.Equal(288, img.Width);
        Assert.Equal(232, img.Height);
        Assert.Equal("f0898ff6cc9f67efe1f53be7599b89b86859237115752c766a9a2ff56152aba1",
            Convert.ToHexString(SHA256.HashData(img.Rgba)).ToLowerInvariant());
    }

    [SkippableFact]
    public void EveryEntryInBothArchivesDecodes()
    {
        var install = RequireInstall();
        foreach (var archive in new[] { install.LoadSmallArt(), install.LoadMediumArt() })
        {
            foreach (var entry in archive.Entries)
            {
                WvlImage img = entry.Decode();
                Assert.Equal(img.Width * img.Height * 4, img.Rgba.Length);
            }
        }
    }

    [SkippableFact]
    public void MediumArtIsTwiceSmallArtResolution()
    {
        var install = RequireInstall();
        WvlImage small = install.LoadSmallArt().FindByCardId(0)!.Decode();
        WvlImage med = install.LoadMediumArt().FindByCardId(0)!.Decode();
        Assert.Equal(small.Width * 2, med.Width);
        Assert.Equal(small.Height * 2, med.Height);
    }
}

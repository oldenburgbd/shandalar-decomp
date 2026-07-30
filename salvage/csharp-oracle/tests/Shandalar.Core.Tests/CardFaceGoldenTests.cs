using System.Security.Cryptography;
using Shandalar.Core;
using Shandalar.Core.Cards;
using Shandalar.Core.Rendering;
using Xunit;

namespace Shandalar.Core.Tests;

/// <summary>
/// Golden tests rendering card faces from the real install. Skipped in CI
/// (SHANDALAR_GAME_DIR unset). See docs/formats/card-face.md.
/// </summary>
public class CardFaceGoldenTests
{
    private static GameInstall RequireInstall()
    {
        var install = GameInstall.FromEnvironment();
        Skip.If(install is null, $"{GameInstall.EnvVar} is not set");
        return install!;
    }

    [SkippableFact]
    public void SerraAngelFaceRendersStably()
    {
        var install = RequireInstall();
        var renderer = CardFaceRenderer.Load(install);
        var serra = install.LoadCardStats().Single(c => c.Id == 221);

        RgbaImage face = renderer.Render(serra, "Magic");

        Assert.Equal(CardFace.Width, face.Width);
        Assert.Equal(CardFace.Height, face.Height);
        Assert.Equal("2480d99c45737955c5f1986a276cc0afeb3b21c915f80edc447cc79fc0c33b3b",
            Convert.ToHexString(SHA256.HashData(face.Pixels)).ToLowerInvariant());
    }

    [SkippableFact]
    public void CardsetsStripHasAllFiveExpansionSymbols()
    {
        var install = RequireInstall();
        var pal = Shandalar.Core.Formats.PlogPalette.Load(install.PathTo("Duel.plogpal"));
        var strip = Shandalar.Core.Formats.Pic.Decode(
            File.ReadAllBytes(install.PathTo(Path.Combine("Cardart", "Cardsets.pic"))));
        var symbols = new SetSymbols(strip, pal);

        Assert.Equal(5, symbols.SymbolCount);
        foreach (var set in new[] { "Dark", "Legends", "Arabian", "Antiquities", "Astral" })
            Assert.True(symbols.Knows(set), $"missing symbol for {set}");
        Assert.False(symbols.Knows("Magic")); // core set has no expansion symbol
    }

    [SkippableFact]
    public void OneFaceOfEveryFrameKindRenders()
    {
        var install = RequireInstall();
        var renderer = CardFaceRenderer.Load(install);
        var stats = install.LoadCardStats().ToDictionary(c => c.Id);

        foreach (var colour in new[]
                 {
                     CardColorCode.Black, CardColorCode.Blue, CardColorCode.Artifact,
                     CardColorCode.Gold, CardColorCode.Green, CardColorCode.Red,
                     CardColorCode.White, CardColorCode.Land,
                 })
        {
            var card = stats.Values.First(c => c.ColorCode == colour);
            RgbaImage face = renderer.Render(card);
            Assert.Equal(CardFace.Width * CardFace.Height * 4, face.Pixels.Length);
        }
    }
}

using Shandalar.Core;
using Shandalar.Core.Cards;
using Xunit;

namespace Shandalar.Core.Tests;

/// <summary>
/// Golden tests decoding the real Cards.dat. Skipped in CI (SHANDALAR_GAME_DIR unset).
/// See docs/formats/cards-dat.md.
/// </summary>
public class CardsDatGoldenTests
{
    private static GameInstall RequireInstall()
    {
        var install = GameInstall.FromEnvironment();
        Skip.If(install is null, $"{GameInstall.EnvVar} is not set");
        return install!;
    }

    private static IReadOnlyList<CardStats> Stats() => RequireInstall().LoadCardStats();

    [SkippableFact]
    public void HasExactly999RecordsWithDenseIds()
    {
        var stats = Stats();
        Assert.Equal(999, stats.Count);
        for (int i = 0; i < stats.Count; i++)
            Assert.Equal(i, stats[i].Id);
    }

    [SkippableFact]
    public void AirElementalIsBlue3UU4x4Flyer()
    {
        var c = Stats()[0];
        Assert.Equal("Air Elemental", c.Name);
        Assert.Equal("0000.WVL", c.ArtFile);
        Assert.Equal(CardColorCode.Blue, c.ColorCode);
        Assert.Equal(CardCategory.Creature, c.Category);
        Assert.Equal(3, c.Mana.Generic);
        Assert.Equal(2, c.Mana.Blue);
        Assert.Equal(5, c.Mana.ConvertedManaCost);
        Assert.Equal(4, c.Power);
        Assert.Equal(4, c.Toughness);
        Assert.True(c.HasFlying);
    }

    [SkippableTheory]
    // name, colour, category, generic, W, U, B, R, G, power, toughness, flying
    [InlineData("Serra Angel", CardColorCode.White, CardCategory.Creature, 3, 2, 0, 0, 0, 0, 4, 4, true)]
    [InlineData("Lightning Bolt", CardColorCode.Red, CardCategory.Instant, 0, 0, 0, 0, 1, 0, 0, 0, false)]
    [InlineData("Craw Wurm", CardColorCode.Green, CardCategory.Creature, 4, 0, 0, 0, 0, 2, 6, 4, false)]
    [InlineData("Black Knight", CardColorCode.Black, CardCategory.Creature, 0, 0, 0, 2, 0, 0, 2, 2, false)]
    [InlineData("Grizzly Bears", CardColorCode.Green, CardCategory.Creature, 1, 0, 0, 0, 0, 1, 2, 2, false)]
    public void KnownCardsDecodeCorrectly(string name, CardColorCode colour, CardCategory category,
        int generic, int w, int u, int b, int r, int g, int power, int toughness, bool flying)
    {
        var card = Stats().First(c => c.Name == name);
        Assert.Equal(colour, card.ColorCode);
        Assert.Equal(category, card.Category);
        Assert.Equal(new ManaCost(generic, w, u, b, r, g, IsVariable: false), card.Mana);
        Assert.Equal(power, card.Power);
        Assert.Equal(toughness, card.Toughness);
        Assert.Equal(flying, card.HasFlying);
    }

    [SkippableFact]
    public void XSpellsAreMarkedVariable()
    {
        var fireball = Stats().First(c => c.Name == "Fireball");
        Assert.True(fireball.Mana.IsVariable);
        Assert.Equal(CardColorCode.Red, fireball.ColorCode);
        Assert.Equal(1, fireball.Mana.Red);
    }

    [SkippableFact]
    public void StatsJoinToMasterCsvOnId()
    {
        var install = RequireInstall();
        var db = install.LoadCardDatabase();
        var stats = install.LoadCardStats();

        // The join key is the ID; every Cards.dat record resolves to a Master.csv record.
        // Names are NOT compared: Master.csv carries short display names (and some typos) while
        // Cards.dat has the canonical names — see docs/quirks.md Q-004.
        foreach (var s in stats)
            Assert.NotNull(db.Find(s.Id));
    }

    [SkippableFact]
    public void CardsDatCarriesCanonicalNamesWhereMasterAbbreviates()
    {
        var install = RequireInstall();
        var db = install.LoadCardDatabase();
        var stats = install.LoadCardStats();

        // Q-004 examples: canonical (Cards.dat) vs display (Master.csv).
        Assert.Equal("Circle of Protection: Blue", stats[34].Name);
        Assert.Equal("Blue Circle", db.Find(34)!.Name);
        Assert.Equal("Resurrection", stats.First(c => c.Id == 203).Name);
        Assert.Equal("Ressurection", db.Find(203)!.Name);
    }
}

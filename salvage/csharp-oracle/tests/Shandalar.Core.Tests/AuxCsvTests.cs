using Shandalar.Core.Cards;
using Xunit;

namespace Shandalar.Core.Tests;

public class InfoCsvTests
{
    [Theory]
    [InlineData("200003", 3, 2, 0, 0, 0, 0, false)] // 3WW (Serra Angel)
    [InlineData("000023", 3, 0, 2, 0, 0, 0, false)] // 3UU (Air Elemental)
    [InlineData("020004", 4, 0, 0, 0, 0, 2, false)] // 4GG (Craw Wurm)
    [InlineData("000200", 0, 0, 0, 2, 0, 0, false)] // BB (Black Knight)
    [InlineData("00100x", 0, 0, 0, 0, 1, 0, true)]  // XR (Fireball)
    [InlineData("000000", 0, 0, 0, 0, 0, 0, false)] // lands
    public void DecodesManaCode(string code, int generic, int w, int u, int b, int r, int g, bool variable)
    {
        Assert.Equal(new ManaCost(generic, w, u, b, r, g, variable), InfoCsv.ParseManaCode(code));
    }

    [Fact]
    public void LoadsRowsWithNullableStats()
    {
        var records = InfoCsv.Load(new StringReader(
            "ID,Card Name,Short Name,Card Set,Color,Type,Subtype,Group,Occuran.,Mana Costs,Pow,Tuff\n" +
            "0000,Air Elemental ,Air Elemental ,Magic,Blue,Summon,Elemental,None,Rare,000023,4,4\n" +
            "0001,Ancestral Recall,Ancestral Recall,Magic,Blue,Instant,na,None,Rare,000010,na,na\n"));

        Assert.Equal(2, records.Count);
        Assert.Equal("Air Elemental", records[0].Name); // trimmed
        Assert.Equal(4, records[0].Power);
        Assert.Null(records[1].Power);
        Assert.Equal("Rare", records[1].Rarity);
        Assert.Equal(1, records[1].Mana.Blue);
    }
}

public class AuxCsvGoldenTests
{
    private static Shandalar.Core.GameInstall RequireInstall()
    {
        var install = Shandalar.Core.GameInstall.FromEnvironment();
        Skip.If(install is null, $"{Shandalar.Core.GameInstall.EnvVar} is not set");
        return install!;
    }

    [SkippableFact]
    public void InfoCoversAll1250IdsAndManaLargelyMatchesCardsDat()
    {
        var install = RequireInstall();
        var info = install.LoadInfo();
        var stats = install.LoadCardStats();

        Assert.Equal(1250, info.Count);

        // Cross-validation: the two sources agree on 946/999. The 53 known disagreements are
        // Info.csv typos, Shandalar token creatures (no real cost), and internal "None" records
        // whose Cards.dat mana bytes hold non-mana data. Cards.dat is authoritative.
        var infoById = info.ToDictionary(i => i.Id);
        int mismatches = stats.Count(s => infoById[s.Id].Mana != s.Mana);
        Assert.Equal(53, mismatches);

        // And exactly on the anchor cards used across the suite:
        foreach (int id in new[] { 0, 84, 106, 145, 221, 224 })
            Assert.Equal(stats[id].Mana, infoById[id].Mana);
    }

    [SkippableFact]
    public void LegacyCoversIds0To999DespiteMalformedTail()
    {
        var legacy = RequireInstall().LoadLegacy();
        Assert.Equal(1000, legacy.Count);            // Q-005: garbage continuation dropped
        Assert.Equal(0, legacy[0].Id);
        Assert.Equal(999, legacy[^1].Id);
    }

    [SkippableFact]
    public void RoguesLoadTheOpponentRoster()
    {
        var rogues = RequireInstall().LoadRogues();
        Assert.Equal(66, rogues.Count);
        Assert.Equal("Isabella", rogues[0].Name);
        Assert.Equal(2, rogues[0].Difficulty);
        Assert.Equal("rogue01.pic", rogues[0].FaceArt);
        Assert.All(rogues, r => Assert.InRange(r.Difficulty, 1, 5));
    }

    [SkippableFact]
    public void RarityAndDeckbuilderTablesLoadWithCardRows()
    {
        var install = RequireInstall();
        var rarity = AuxCsvs.LoadRawTable(install.PathTo("Rarity.csv"), headerRows: 3);
        var builder = AuxCsvs.LoadRawTable(install.PathTo("Deckbuilder.csv"), headerRows: 2);

        Assert.Contains(rarity, r => r.Length > 2 && r[1] == "Air Elemental");
        Assert.Equal("Air Elemental", builder[0][0]);
        Assert.Equal("0000", builder[0][1]);
    }

    [SkippableFact]
    public void ConciseLoadsNumericTriples()
    {
        var install = RequireInstall();
        var triples = AuxCsvs.LoadConcise(install.PathTo("Concise.csv"));
        Assert.Equal(511, triples.Count);
        Assert.All(triples, t => Assert.InRange(t.Group, 1, 3));
        Assert.Equal(239, triples[0].CardId); // Swamp
    }
}

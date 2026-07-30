using Shandalar.Core.Cards;
using Xunit;

namespace Shandalar.Core.Tests;

public class CardDatabaseTests
{
    private static CardDatabase LoadFixture() =>
        CardDatabase.LoadMasterCsv(Path.Combine("fixtures", "master-sample.csv"));

    [Fact]
    public void LoadsAllRecordsSkippingHeader()
    {
        var db = LoadFixture();
        Assert.Equal(5, db.Cards.Count);
    }

    [Fact]
    public void TruncatedQuoteRowDoesNotSwallowFollowingRows()
    {
        // Quirk Q-001: the shipped Master.csv has rows cut off inside an open quote.
        var db = LoadFixture();
        Assert.Equal("This rule text is cut off mid-sentence with no closing quo",
            db.Find(3)!.RulesText);
        Assert.Equal("After Truncation", db.Find(4)!.Name);
    }

    [Fact]
    public void TrimsNameButPreservesRawName()
    {
        var card = LoadFixture().Find(0)!;
        Assert.Equal("Test Elemental", card.Name);
        Assert.Equal("Test Elemental ", card.RawName);
        Assert.Equal("Summon Elemental", card.TypeLine);
        Assert.Contains("\"quoted\" words", card.FlavorText);
    }

    [Fact]
    public void ShortRowsLoadWithEmptyTrailingFields()
    {
        var card = LoadFixture().Find(2)!;
        Assert.Equal("None", card.RulesText);
        Assert.Equal("", card.FlavorText);
    }

    [Fact]
    public void SearchIsCaseInsensitive()
    {
        Assert.Single(LoadFixture().Search("elemental"));
    }
}

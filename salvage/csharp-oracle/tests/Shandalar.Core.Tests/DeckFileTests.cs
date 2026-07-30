using Shandalar.Core.Cards;
using Xunit;

namespace Shandalar.Core.Tests;

public class DeckFileTests
{
    [Fact]
    public void ParsesDecksStyleWithTitleAndSideboards()
    {
        string text =
            "Lord of Fate (Bl/Wh)\n" +
            "\n" +
            ".188\t11\tPlains\n" +
            ".55\t2\tDark Ritual\n" +
            "\n" +
            ".vNone\n" +
            ".514\t3\tOrnithopter\n" +
            ".vBlack\n" +
            ".930\t3\tBrass Man\n";

        DeckList deck = DeckFile.Parse(new StringReader(text));

        Assert.Equal("Lord of Fate (Bl/Wh)", deck.Name);
        Assert.Equal(2, deck.Main.Count);
        Assert.Equal(new DeckEntry(188, 11, "Plains"), deck.Main[0]);
        Assert.Equal(new DeckEntry(55, 2, "Dark Ritual"), deck.Main[1]);
        Assert.Equal(13, deck.MainCardCount);

        Assert.Equal(new DeckEntry(514, 3, "Ornithopter"), Assert.Single(deck.Sideboards[SideboardColor.None]));
        Assert.Equal(new DeckEntry(930, 3, "Brass Man"), Assert.Single(deck.Sideboards[SideboardColor.Black]));
        Assert.Equal(4, deck.AllEntries.Count());
    }

    [Fact]
    public void ParsesPlaydeckStyleWithCommentHeader()
    {
        string text =
            ";Ape Lord\n" +
            ";Red/Green\n" +
            ";Coyote Tex\n" +
            ";comments\n" +
            "\n" +
            ".91\t12\tForest\n" +
            ".164\t10\tMountain\n";

        DeckList deck = DeckFile.Parse(new StringReader(text));

        Assert.Equal("Ape Lord", deck.Name);         // first comment is the name
        Assert.Equal(4, deck.Comments.Count);
        Assert.Equal("Red/Green", deck.Comments[1]);
        Assert.Equal(2, deck.Main.Count);
        Assert.Empty(deck.Sideboards);
        Assert.Equal(22, deck.MainCardCount);
    }

    [Fact]
    public void DistinguishesSideboardMarkerFromCardLine()
    {
        // ".5..." is a card (digit after dot); ".vRed" is a marker (letter after dot).
        DeckList deck = DeckFile.Parse(new StringReader(".5\t1\tCard Five\n.vRed\n.99\t2\tAnswer\n"));
        Assert.Single(deck.Main);
        Assert.Equal(5, deck.Main[0].CardId);
        Assert.Equal(new DeckEntry(99, 2, "Answer"), Assert.Single(deck.Sideboards[SideboardColor.Red]));
    }

    [Fact]
    public void FallsBackToProvidedNameWhenNoneInFile()
    {
        DeckList deck = DeckFile.Parse(new StringReader(".1\t1\tA Card\n"), nameFallback: "0069");
        Assert.Equal("0069", deck.Name);
    }
}

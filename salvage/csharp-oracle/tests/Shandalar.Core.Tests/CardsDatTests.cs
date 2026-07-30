using Shandalar.Core.Cards;
using Xunit;
using Card = Shandalar.Core.Tests.SyntheticCardsDat.Card;

namespace Shandalar.Core.Tests;

public class CardsDatTests
{
    private static IReadOnlyList<CardStats> BuildTwo() => CardsDat.Parse(SyntheticCardsDat.Build(
        new Card(0, "Test Flyer", "0000.WVL", ColorCode: 2, Category: 7,
            SyntheticCardsDat.Mana(generic: 3, blue: 2), Power: 4, Toughness: 4, AbilityFlags: 0x4),
        new Card(1, "Test Bear", "0001.WVL", ColorCode: 5, Category: 7,
            SyntheticCardsDat.Mana(generic: 1, green: 1), Power: 2, Toughness: 2, AbilityFlags: 0)));

    [Fact]
    public void DecodesCoreFields()
    {
        var c = BuildTwo()[0];
        Assert.Equal(0, c.Id);
        Assert.Equal("Test Flyer", c.Name);
        Assert.Equal("0000.WVL", c.ArtFile);
        Assert.Equal(CardColorCode.Blue, c.ColorCode);
        Assert.Equal(CardCategory.Creature, c.Category);
        Assert.Equal(4, c.Power);
        Assert.Equal(4, c.Toughness);
        Assert.True(c.HasFlying);
        Assert.True(c.IsCreature);
    }

    [Fact]
    public void DecodesManaCostFromPipArray()
    {
        var flyer = BuildTwo()[0];
        Assert.Equal(new ManaCost(Generic: 3, White: 0, Blue: 2, Black: 0, Red: 0, Green: 0, IsVariable: false), flyer.Mana);
        Assert.Equal(5, flyer.Mana.ConvertedManaCost); // 3UU
        Assert.False(flyer.Mana.IsColorless);

        var bear = BuildTwo()[1];
        Assert.Equal(1, bear.Mana.Generic);
        Assert.Equal(1, bear.Mana.Green);
        Assert.Equal(2, bear.Mana.ConvertedManaCost); // 1G
        Assert.False(bear.HasFlying);
    }

    [Fact]
    public void TreatsGeneric40AsVariableX()
    {
        var cards = CardsDat.Parse(SyntheticCardsDat.Build(
            new Card(0, "Test Fireball", "0000.WVL", ColorCode: 7, Category: 3,
                SyntheticCardsDat.Mana(generic: 40, red: 1), Power: 0, Toughness: 0, AbilityFlags: 0)));

        Assert.True(cards[0].Mana.IsVariable);
        Assert.Equal(0, cards[0].Mana.Generic);
        Assert.Equal(1, cards[0].Mana.Red);       // XR -> just the R counts
        Assert.Equal(1, cards[0].Mana.ConvertedManaCost);
    }

    [Fact]
    public void UnknownCategoryFallsBackToUnknown()
    {
        var cards = CardsDat.Parse(SyntheticCardsDat.Build(
            new Card(0, "Weird", "0000.WVL", ColorCode: 3, Category: 99,
                SyntheticCardsDat.Mana(generic: 1), Power: 0, Toughness: 0, AbilityFlags: 0)));
        Assert.Equal(CardCategory.Unknown, cards[0].Category);
    }

    [Fact]
    public void RejectsMisalignedSections()
    {
        byte[] good = SyntheticCardsDat.Build(
            new Card(0, "X", "0.WVL", 3, 1, SyntheticCardsDat.Mana(0), 0, 0, 0));
        good[4] += 200; // inflate the declared text size so sections overlap
        Assert.Throws<InvalidDataException>(() => CardsDat.Parse(good));
    }
}

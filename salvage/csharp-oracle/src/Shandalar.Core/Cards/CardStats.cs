namespace Shandalar.Core.Cards;

/// <summary>Colour / category code from Cards.dat (also the index into the mana pip array).</summary>
public enum CardColorCode
{
    /// <summary>Internal records: game effects, tokens, blank slots (71 cards).</summary>
    None = 0,
    Black = 1,
    Blue = 2,
    Artifact = 3,
    /// <summary>Multicolour ("gold") cards, mostly Legends (63 cards).</summary>
    Gold = 4,
    Green = 5,
    Land = 6,
    Red = 7,
    White = 8,
}

/// <summary>Card type category from Cards.dat.</summary>
public enum CardCategory
{
    Unknown = 0,
    Artifact = 1,
    Enchantment = 2,
    Instant = 3,
    Interrupt = 4,
    Land = 5,
    Sorcery = 6,
    Creature = 7,
}

/// <summary>
/// A mana cost: generic plus per-colour pips. <see cref="IsVariable"/> marks an X spell (the
/// generic slot in Cards.dat stores the sentinel 40 — Q-003); for those <see cref="Generic"/> is 0.
/// </summary>
public readonly record struct ManaCost(
    int Generic, int White, int Blue, int Black, int Red, int Green, bool IsVariable)
{
    public int Colored => White + Blue + Black + Red + Green;

    public int ConvertedManaCost => Generic + Colored;

    public bool IsColorless => Colored == 0;
}

/// <summary>
/// Gameplay data for one card, decoded from Cards.dat (see docs/formats/cards-dat.md). Keyed by
/// <see cref="Id"/>, which matches <see cref="CardRecord.Id"/> from Master.csv for IDs 0..998.
/// </summary>
public sealed record CardStats(
    int Id,
    string Name,
    string ArtFile,
    CardColorCode ColorCode,
    CardCategory Category,
    ManaCost Mana,
    int Power,
    int Toughness,
    uint AbilityFlags)
{
    /// <summary>Flying is ability-flags bit 0x4 (verified; other bits TBD, see issue #12).</summary>
    public bool HasFlying => (AbilityFlags & CardsDat.FlyingFlag) != 0;

    public bool IsCreature => Category == CardCategory.Creature;
}

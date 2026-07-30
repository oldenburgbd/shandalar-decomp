namespace Shandalar.Core.Cards;

/// <summary>
/// Reader for the binary card database Cards.dat: an 8-byte header, then fixed 152-byte gameplay
/// records for card IDs 0..N-1, then a text section of null-terminated strings the records point
/// into. See docs/formats/cards-dat.md for the full field layout.
/// </summary>
public static class CardsDat
{
    public const int HeaderSize = 8;
    public const int RecordSize = 152;
    public const uint FlyingFlag = 0x4;
    private const int XSpellSentinel = 40;

    // Record byte offsets (see docs/formats/cards-dat.md).
    private const int OffId = 0;
    private const int OffNamePtr = 4;
    private const int OffColour = 16;
    private const int OffCategory = 20;
    private const int OffMana = 40;      // 9-byte pip array indexed by colour code
    private const int OffArtPtr = 76;
    private const int OffPower = 124;
    private const int OffToughness = 128;
    private const int OffAbilityFlags = 136;

    // Mana pip array indices (== colour codes).
    private const int ManaGeneric = 0, ManaBlack = 1, ManaBlue = 2, ManaGreen = 5, ManaRed = 7, ManaWhite = 8;

    public static IReadOnlyList<CardStats> Load(string path) => Parse(File.ReadAllBytes(path));

    public static IReadOnlyList<CardStats> Parse(byte[] data)
    {
        if (data.Length < HeaderSize)
            throw new InvalidDataException("Cards.dat is too small for a header");

        int recordCount = (int)ReadU32(data, 0);
        int textSize = (int)ReadU32(data, 4);
        int textStart = data.Length - textSize;

        int expected = HeaderSize + recordCount * RecordSize;
        if (textStart < expected || textStart > data.Length)
            throw new InvalidDataException(
                $"Cards.dat sections don't line up: {recordCount} records need {expected} bytes " +
                $"but the text section starts at {textStart}");

        var cards = new List<CardStats>(recordCount);
        for (int i = 0; i < recordCount; i++)
        {
            int rec = HeaderSize + i * RecordSize;
            cards.Add(new CardStats(
                Id: (int)ReadU32(data, rec + OffId),
                Name: ReadString(data, textStart + (int)ReadU32(data, rec + OffNamePtr)),
                ArtFile: ReadString(data, textStart + (int)ReadU32(data, rec + OffArtPtr)),
                ColorCode: (CardColorCode)ReadU32(data, rec + OffColour),
                Category: ToCategory(ReadU32(data, rec + OffCategory)),
                Mana: ReadMana(data, rec + OffMana),
                Power: (int)ReadU32(data, rec + OffPower),
                Toughness: (int)ReadU32(data, rec + OffToughness),
                AbilityFlags: ReadU32(data, rec + OffAbilityFlags)));
        }
        return cards;
    }

    private static ManaCost ReadMana(byte[] data, int off)
    {
        int generic = data[off + ManaGeneric];
        bool variable = generic == XSpellSentinel;
        return new ManaCost(
            Generic: variable ? 0 : generic,
            White: data[off + ManaWhite],
            Blue: data[off + ManaBlue],
            Black: data[off + ManaBlack],
            Red: data[off + ManaRed],
            Green: data[off + ManaGreen],
            IsVariable: variable);
    }

    private static CardCategory ToCategory(uint v) =>
        Enum.IsDefined(typeof(CardCategory), (int)v) ? (CardCategory)v : CardCategory.Unknown;

    private static string ReadString(byte[] data, int offset)
    {
        if (offset < 0 || offset >= data.Length)
            return "";
        int end = offset;
        while (end < data.Length && data[end] != 0)
            end++;
        return System.Text.Encoding.ASCII.GetString(data, offset, end - offset);
    }

    private static uint ReadU32(byte[] d, int o) =>
        (uint)(d[o] | (d[o + 1] << 8) | (d[o + 2] << 16) | (d[o + 3] << 24));
}

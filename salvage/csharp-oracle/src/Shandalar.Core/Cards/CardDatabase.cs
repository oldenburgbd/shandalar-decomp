using Shandalar.Core.Text;

namespace Shandalar.Core.Cards;

/// <summary>
/// The card database as defined by Master.csv
/// (header: ID,Card Name,Type Description,Artist Name,Rule Text,Quote).
/// Some rows omit the Quote field entirely; missing trailing fields load as empty strings.
/// </summary>
public sealed class CardDatabase
{
    private readonly Dictionary<int, CardRecord> _byId;

    public IReadOnlyList<CardRecord> Cards { get; }

    private CardDatabase(List<CardRecord> cards)
    {
        Cards = cards;
        _byId = cards.ToDictionary(c => c.Id);
    }

    public CardRecord? Find(int id) => _byId.GetValueOrDefault(id);

    public IEnumerable<CardRecord> Search(string nameFragment) =>
        Cards.Where(c => c.Name.Contains(nameFragment, StringComparison.OrdinalIgnoreCase));

    public static CardDatabase LoadMasterCsv(string path)
    {
        using var reader = new StreamReader(path);
        return LoadMasterCsv(reader);
    }

    public static CardDatabase LoadMasterCsv(TextReader reader)
    {
        // One physical line = one record, matching the original engine. The shipped file
        // has rows truncated mid-quote (docs/quirks.md Q-001), so strict RFC-4180
        // multi-line parsing would merge runs of records into one broken field.
        var cards = new List<CardRecord>();
        bool isHeader = true;
        for (string? line = reader.ReadLine(); line is not null; line = reader.ReadLine())
        {
            if (isHeader)
            {
                isHeader = false;
                continue;
            }
            if (line.Length == 0)
                continue;
            var row = Csv.ParseLine(line);
            if (row[0].Length == 0)
                continue;

            cards.Add(new CardRecord(
                Id: int.Parse(row[0]),
                RawName: Get(row, 1),
                TypeLine: Get(row, 2),
                Artist: Get(row, 3),
                RulesText: Get(row, 4),
                FlavorText: Get(row, 5)));
        }
        return new CardDatabase(cards);

        static string Get(string[] row, int i) => i < row.Length ? row[i] : "";
    }
}

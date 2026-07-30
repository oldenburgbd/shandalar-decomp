using Shandalar.Core.Text;

namespace Shandalar.Core.Cards;

/// <summary>
/// One row of Info.csv: per-card display/deck-builder data for all 1,250 IDs (including the
/// effect pseudo-cards), with set, colour, type split, rarity, an encoded mana cost, and P/T.
/// </summary>
public sealed record InfoRecord(
    int Id,
    string Name,
    string ShortName,
    string Set,
    string Color,
    string Type,
    string Subtype,
    string Group,
    string Rarity,
    ManaCost Mana,
    int? Power,
    int? Toughness);

/// <summary>
/// Loader for Info.csv (header: ID,Card Name,Short Name,Card Set,Color,Type,Subtype,Group,
/// Occuran.,Mana Costs,Pow,Tuff). The mana code is six characters [W][G][R][B][U][generic];
/// a generic digit of 'x' marks an X spell. See docs/formats/csv-files.md.
/// </summary>
public static class InfoCsv
{
    public static IReadOnlyList<InfoRecord> Load(string path)
    {
        using var reader = new StreamReader(path, System.Text.Encoding.Latin1);
        return Load(reader);
    }

    public static IReadOnlyList<InfoRecord> Load(TextReader reader)
    {
        var records = new List<InfoRecord>();
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
            if (row.Length < 12 || !int.TryParse(row[0], out int id))
                continue;

            records.Add(new InfoRecord(
                Id: id,
                Name: row[1].Trim(),
                ShortName: row[2].Trim(),
                Set: row[3],
                Color: row[4],
                Type: row[5],
                Subtype: row[6],
                Group: row[7],
                Rarity: row[8],
                Mana: ParseManaCode(row[9]),
                Power: ParseStat(row[10]),
                Toughness: ParseStat(row[11])));
        }
        return records;
    }

    /// <summary>Decodes the six-character [W][G][R][B][U][generic] mana code.</summary>
    public static ManaCost ParseManaCode(string code)
    {
        if (code.Length < 6)
            return default;
        bool variable = code[5] is 'x' or 'X';
        return new ManaCost(
            Generic: variable ? 0 : Digit(code[5]),
            White: Digit(code[0]),
            Blue: Digit(code[4]),
            Black: Digit(code[3]),
            Red: Digit(code[2]),
            Green: Digit(code[1]),
            IsVariable: variable);

        static int Digit(char c) => char.IsAsciiDigit(c) ? c - '0' : 0;
    }

    private static int? ParseStat(string s) => int.TryParse(s, out int v) ? v : null;
}

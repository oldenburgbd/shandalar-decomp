using Shandalar.Core.Text;

namespace Shandalar.Core.Cards;

/// <summary>Extra per-card text from Legacy.csv (damage/effect/legacy sub-card wordings), IDs 0-999.</summary>
public sealed record LegacyRecord(
    int Id, string Name, string DamageText, string EffectTitle, string EffectText,
    string LegacyTitle, string LegacyText);

/// <summary>A duel opponent from Rogues.csv.</summary>
public sealed record RogueRecord(string Name, int Difficulty, string Quote, string FaceArt);

/// <summary>One numeric triple from the headerless Concise.csv (see docs/formats/csv-files.md).</summary>
public sealed record ConciseRecord(int CardId, int Group, int Value);

/// <summary>
/// Loaders for the remaining secondary CSVs. All use line-scoped parsing (quirk Q-001 family —
/// Legacy.csv's last record is malformed, Q-005) and Latin-1 (the files are 1990s Windows text).
/// Rarity.csv / Deckbuilder.csv are exposed as raw rows: their deep AI-weight semantics are
/// deferred to the Phase 2/3 AI work (issue #12). See docs/formats/csv-files.md.
/// </summary>
public static class AuxCsvs
{
    public static IReadOnlyList<LegacyRecord> LoadLegacy(string path)
    {
        using var reader = new StreamReader(path, System.Text.Encoding.Latin1);
        var records = new List<LegacyRecord>();
        bool isHeader = true;
        for (string? line = reader.ReadLine(); line is not null; line = reader.ReadLine())
        {
            if (isHeader)
            {
                isHeader = false;
                continue;
            }
            var row = Csv.ParseLine(line);
            if (row.Length < 7 || !int.TryParse(row[0], out int id))
                continue; // skips the binary-garbage continuation of the malformed last row (Q-005)
            records.Add(new LegacyRecord(id, row[1], row[2], row[3], row[4], row[5], row[6]));
        }
        return records;
    }

    public static IReadOnlyList<RogueRecord> LoadRogues(string path)
    {
        using var reader = new StreamReader(path, System.Text.Encoding.Latin1);
        var records = new List<RogueRecord>();
        bool isHeader = true;
        for (string? line = reader.ReadLine(); line is not null; line = reader.ReadLine())
        {
            if (isHeader)
            {
                isHeader = false;
                continue;
            }
            var row = Csv.ParseLine(line);
            if (row.Length < 4 || row[0].Length == 0)
                continue;
            records.Add(new RogueRecord(
                row[0], int.TryParse(row[1], out int d) ? d : 0, row[2], row[3]));
        }
        return records;
    }

    public static IReadOnlyList<ConciseRecord> LoadConcise(string path)
    {
        var records = new List<ConciseRecord>();
        foreach (var line in File.ReadLines(path))
        {
            var parts = line.Split(' ', StringSplitOptions.RemoveEmptyEntries);
            if (parts.Length >= 3 &&
                int.TryParse(parts[0], out int id) &&
                int.TryParse(parts[1], out int group) &&
                int.TryParse(parts[2], out int value))
                records.Add(new ConciseRecord(id, group, value));
        }
        return records;
    }

    /// <summary>
    /// Loads Rarity.csv or Deckbuilder.csv as raw rows (multi-row headers skipped). Each row is
    /// the split fields of one card line; row[0]/row[1] carry name/ID position per file's header.
    /// </summary>
    public static IReadOnlyList<string[]> LoadRawTable(string path, int headerRows)
    {
        var rows = new List<string[]>();
        int skipped = 0;
        using var reader = new StreamReader(path, System.Text.Encoding.Latin1);
        for (string? line = reader.ReadLine(); line is not null; line = reader.ReadLine())
        {
            if (skipped < headerRows)
            {
                skipped++;
                continue;
            }
            if (line.Length == 0)
                continue;
            rows.Add(Csv.ParseLine(line));
        }
        return rows;
    }
}

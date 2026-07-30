namespace Shandalar.Core.Text;

/// <summary>
/// Minimal RFC-4180 CSV reader for the game's data files (Master.csv, Rarity.csv, ...).
/// Handles quoted fields, doubled-quote escapes, and newlines inside quoted fields.
/// Rows in these files are not guaranteed to have a uniform field count.
/// </summary>
public static class Csv
{
    public static IEnumerable<string[]> ReadRecords(TextReader reader)
    {
        var fields = new List<string>();
        var field = new System.Text.StringBuilder();
        bool inQuotes = false;
        bool anyData = false;

        int c;
        while ((c = reader.Read()) >= 0)
        {
            char ch = (char)c;
            if (inQuotes)
            {
                if (ch == '"')
                {
                    if (reader.Peek() == '"')
                    {
                        reader.Read();
                        field.Append('"');
                    }
                    else
                    {
                        inQuotes = false;
                    }
                }
                else
                {
                    field.Append(ch);
                }
                anyData = true;
                continue;
            }

            switch (ch)
            {
                case '"':
                    inQuotes = true;
                    anyData = true;
                    break;
                case ',':
                    fields.Add(field.ToString());
                    field.Clear();
                    anyData = true;
                    break;
                case '\r':
                    break;
                case '\n':
                    if (anyData)
                    {
                        fields.Add(field.ToString());
                        yield return fields.ToArray();
                    }
                    fields.Clear();
                    field.Clear();
                    anyData = false;
                    break;
                default:
                    field.Append(ch);
                    anyData = true;
                    break;
            }
        }

        if (anyData)
        {
            fields.Add(field.ToString());
            yield return fields.ToArray();
        }
    }

    public static IEnumerable<string[]> ReadRecords(string path)
    {
        using var reader = new StreamReader(path);
        foreach (var record in ReadRecords(reader))
            yield return record;
    }

    /// <summary>
    /// Parses a single physical line as one record, the way the original game reads
    /// Master.csv. Unlike RFC-4180, a quote left open at end-of-line ends its field there —
    /// the shipped file contains rows truncated mid-quote (see docs/quirks.md, Q-001).
    /// </summary>
    public static string[] ParseLine(string line)
    {
        var fields = new List<string>();
        var field = new System.Text.StringBuilder();
        bool inQuotes = false;

        for (int i = 0; i < line.Length; i++)
        {
            char ch = line[i];
            if (inQuotes)
            {
                if (ch == '"')
                {
                    if (i + 1 < line.Length && line[i + 1] == '"')
                    {
                        field.Append('"');
                        i++;
                    }
                    else
                    {
                        inQuotes = false;
                    }
                }
                else
                {
                    field.Append(ch);
                }
            }
            else if (ch == '"')
            {
                inQuotes = true;
            }
            else if (ch == ',')
            {
                fields.Add(field.ToString());
                field.Clear();
            }
            else
            {
                field.Append(ch);
            }
        }

        fields.Add(field.ToString());
        return fields.ToArray();
    }
}

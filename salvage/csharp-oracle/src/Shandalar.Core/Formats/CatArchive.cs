namespace Shandalar.Core.Formats;

/// <summary>One entry of a .cat archive: a named WVL-compressed card-art image.</summary>
public sealed class CatEntry
{
    /// <summary>Entry name as stored (e.g. "c0000.tif"); numeric part = card ID.</summary>
    public string Name { get; }

    /// <summary>Card ID parsed from the digits in <see cref="Name"/>, or -1.</summary>
    public int CardId { get; }

    /// <summary>The raw WVL blob (156-byte header + VLC-compressed wavelet planes).</summary>
    public byte[] Data { get; }

    public CatEntry(string name, int cardId, byte[] data)
    {
        Name = name;
        CardId = cardId;
        Data = data;
    }

    /// <summary>Decodes this entry's card art to RGB pixels.</summary>
    public WvlImage Decode() => Wvl.Decode(Data);
}

/// <summary>
/// Reader for the Cardart .cat archives (Smallart.cat / Medart.cat). Layout: a uint32 entry
/// count, then 12-byte index entries {u32 runtimePtr (garbage on disk), u32 offset, u32 size};
/// each entry's name lives inside its own data at offset 8. See docs/formats/cat.md.
/// </summary>
public sealed class CatArchive
{
    private readonly Dictionary<int, CatEntry> _byCardId;

    public IReadOnlyList<CatEntry> Entries { get; }

    private CatArchive(List<CatEntry> entries)
    {
        Entries = entries;
        _byCardId = new Dictionary<int, CatEntry>();
        foreach (var e in entries)
            if (e.CardId >= 0)
                _byCardId.TryAdd(e.CardId, e);
    }

    public CatEntry? FindByCardId(int cardId) => _byCardId.GetValueOrDefault(cardId);

    public static CatArchive Load(string path) => Parse(File.ReadAllBytes(path));

    public static CatArchive Parse(byte[] data)
    {
        if (data.Length < 4)
            throw new InvalidDataException(".cat too small");
        int count = BitConverter.ToInt32(data, 0);
        int indexEnd = 4 + count * 12;
        if (count <= 0 || indexEnd > data.Length)
            throw new InvalidDataException($".cat entry count {count} doesn't fit the file");

        var entries = new List<CatEntry>(count);
        for (int i = 0; i < count; i++)
        {
            int e = 4 + i * 12;
            // First u32 is a dumped runtime pointer — ignored.
            uint offset = BitConverter.ToUInt32(data, e + 4);
            uint size = BitConverter.ToUInt32(data, e + 8);
            if (offset + size > (uint)data.Length)
                throw new InvalidDataException($".cat entry {i} exceeds the file");

            byte[] blob = new byte[size];
            Array.Copy(data, offset, blob, 0, size);
            string name = ReadEntryName(blob);
            entries.Add(new CatEntry(name, ParseCardId(name), blob));
        }
        return new CatArchive(entries);
    }

    private static string ReadEntryName(byte[] blob)
    {
        // The WVL header stores the original filename at offset 8, terminated by 0 (or a 2 byte).
        var sb = new System.Text.StringBuilder();
        for (int i = 8; i < blob.Length && blob[i] != 0 && blob[i] != 2; i++)
            sb.Append((char)blob[i]);
        return sb.ToString();
    }

    private static int ParseCardId(string name)
    {
        int start = 0;
        while (start < name.Length && !char.IsAsciiDigit(name[start]))
            start++;
        int end = start;
        while (end < name.Length && char.IsAsciiDigit(name[end]))
            end++;
        return end > start && int.TryParse(name[start..end], out int id) ? id : -1;
    }
}

using Shandalar.Core.Formats;
using Xunit;

namespace Shandalar.Core.Tests;

public class CatArchiveTests
{
    /// <summary>Builds a minimal synthetic .cat: index + name-bearing blobs (no image data).</summary>
    private static byte[] BuildCat(params (string name, byte[] payload)[] entries)
    {
        var blobs = new List<byte[]>();
        foreach (var (name, payload) in entries)
        {
            // Blob layout mirrors the WVL header: 8 bytes, then NUL-terminated name, then payload.
            var blob = new List<byte>(new byte[8]);
            blob.AddRange(System.Text.Encoding.ASCII.GetBytes(name));
            blob.Add(0);
            blob.AddRange(payload);
            blobs.Add(blob.ToArray());
        }

        var file = new List<byte>();
        file.AddRange(BitConverter.GetBytes(entries.Length));
        int dataStart = 4 + entries.Length * 12;
        int off = dataStart;
        foreach (var blob in blobs)
        {
            file.AddRange(BitConverter.GetBytes(0xDEADBEEF)); // runtime ptr: garbage
            file.AddRange(BitConverter.GetBytes(off));
            file.AddRange(BitConverter.GetBytes(blob.Length));
            off += blob.Length;
        }
        foreach (var blob in blobs)
            file.AddRange(blob);
        return file.ToArray();
    }

    [Fact]
    public void ParsesEntriesNamesAndCardIds()
    {
        var cat = CatArchive.Parse(BuildCat(
            ("c0000.tif", [1, 2, 3]),
            ("0001.tif", [4]),          // some entries lack the 'c' prefix
            ("c0221.tif", [5, 6])));

        Assert.Equal(3, cat.Entries.Count);
        Assert.Equal("c0000.tif", cat.Entries[0].Name);
        Assert.Equal(0, cat.Entries[0].CardId);
        Assert.Equal(1, cat.Entries[1].CardId);
        Assert.Equal(221, cat.Entries[2].CardId);
        Assert.Same(cat.Entries[2], cat.FindByCardId(221));
        Assert.Null(cat.FindByCardId(999));
    }

    [Fact]
    public void EntryDataIsSlicedByIndexOffsetAndSize()
    {
        var cat = CatArchive.Parse(BuildCat(("c0007.tif", [9, 8, 7])));
        byte[] blob = cat.Entries[0].Data;
        Assert.Equal([9, 8, 7], blob[^3..]);
    }

    [Fact]
    public void RejectsIndexBeyondFile()
    {
        byte[] file = BuildCat(("c0000.tif", [1]));
        file[0] = 200; // inflate entry count past the file
        Assert.Throws<InvalidDataException>(() => CatArchive.Parse(file));
    }
}

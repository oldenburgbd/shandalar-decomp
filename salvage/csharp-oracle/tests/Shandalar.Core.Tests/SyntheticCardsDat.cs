using System.Text;

namespace Shandalar.Core.Tests;

/// <summary>Builds minimal, valid Cards.dat byte streams for CI tests (no real game files).</summary>
internal static class SyntheticCardsDat
{
    internal sealed record Card(
        int Id, string Name, string Art, int ColorCode, int Category,
        byte[] Mana9, int Power, int Toughness, uint AbilityFlags);

    private const int RecordSize = 152;

    public static byte[] Build(params Card[] cards)
    {
        // Text section: name\0 art\0 per card; remember offsets (relative to text start).
        var text = new List<byte>();
        var nameOff = new int[cards.Length];
        var artOff = new int[cards.Length];
        for (int i = 0; i < cards.Length; i++)
        {
            nameOff[i] = text.Count;
            text.AddRange(Encoding.ASCII.GetBytes(cards[i].Name));
            text.Add(0);
            artOff[i] = text.Count;
            text.AddRange(Encoding.ASCII.GetBytes(cards[i].Art));
            text.Add(0);
        }

        var file = new byte[8 + cards.Length * RecordSize + text.Count];
        WriteU32(file, 0, (uint)cards.Length);
        WriteU32(file, 4, (uint)text.Count);

        for (int i = 0; i < cards.Length; i++)
        {
            int rec = 8 + i * RecordSize;
            Card c = cards[i];
            WriteU32(file, rec + 0, (uint)c.Id);
            WriteU32(file, rec + 4, (uint)nameOff[i]);
            WriteU32(file, rec + 16, (uint)c.ColorCode);
            WriteU32(file, rec + 20, (uint)c.Category);
            Array.Copy(c.Mana9, 0, file, rec + 40, 9);
            WriteU32(file, rec + 76, (uint)artOff[i]);
            WriteU32(file, rec + 124, (uint)c.Power);
            WriteU32(file, rec + 128, (uint)c.Toughness);
            WriteU32(file, rec + 136, c.AbilityFlags);
        }

        Array.Copy(text.ToArray(), 0, file, 8 + cards.Length * RecordSize, text.Count);
        return file;
    }

    /// <summary>Builds the 9-byte mana pip array from generic + per-colour pips (index by colour code).</summary>
    public static byte[] Mana(int generic, int white = 0, int blue = 0, int black = 0, int red = 0, int green = 0)
    {
        var m = new byte[9];
        m[0] = (byte)generic;
        m[1] = (byte)black;
        m[2] = (byte)blue;
        m[5] = (byte)green;
        m[7] = (byte)red;
        m[8] = (byte)white;
        return m;
    }

    private static void WriteU32(byte[] b, int o, uint v)
    {
        b[o] = (byte)(v & 0xFF); b[o + 1] = (byte)((v >> 8) & 0xFF);
        b[o + 2] = (byte)((v >> 16) & 0xFF); b[o + 3] = (byte)((v >> 24) & 0xFF);
    }
}

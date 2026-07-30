namespace Shandalar.Core.Cards;

/// <summary>
/// One row of Master.csv. IDs 0000-1249 cover both real cards and internal
/// effect pseudo-cards (e.g. "Bandit Effect") used by the Shandalar layer.
/// Names in the file may carry trailing whitespace; <see cref="Name"/> is trimmed
/// and <see cref="RawName"/> preserves the original bytes.
/// </summary>
public sealed record CardRecord(
    int Id,
    string RawName,
    string TypeLine,
    string Artist,
    string RulesText,
    string FlavorText)
{
    public string Name => RawName.Trim();
}

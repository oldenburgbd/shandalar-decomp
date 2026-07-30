using Shandalar.Core;
using Shandalar.Core.Cards;
using Xunit;

namespace Shandalar.Core.Tests;

/// <summary>
/// Golden tests over the real install's .dck files. Skipped in CI (SHANDALAR_GAME_DIR unset).
/// See docs/formats/dck.md.
/// </summary>
public class DeckGoldenTests
{
    private static GameInstall RequireInstall()
    {
        var install = GameInstall.FromEnvironment();
        Skip.If(install is null, $"{GameInstall.EnvVar} is not set");
        return install!;
    }

    [SkippableFact]
    public void AllShippedDecksParseAndEveryCardIdResolves()
    {
        var install = RequireInstall();
        var db = install.LoadCardDatabase();
        var paths = install.DeckPaths().ToList();

        Assert.Equal(132, paths.Count);

        foreach (var path in paths)
        {
            DeckList deck = install.LoadDeck(path);
            Assert.NotEmpty(deck.Main);
            foreach (DeckEntry entry in deck.AllEntries)
            {
                Assert.True(entry.Count > 0, $"{Path.GetFileName(path)}: non-positive count");
                Assert.NotNull(db.Find(entry.CardId));
            }
        }
    }

    [SkippableFact]
    public void ThemeDeckDecodesWithSideboards()
    {
        var install = RequireInstall();
        DeckList deck = install.LoadDeck(Path.Combine(install.Root, "Decks", "0010.dck"));

        Assert.Equal("Lord of Fate (Bl/Wh, 4th Edition)", deck.Name);
        Assert.Equal(new DeckEntry(188, 11, "Plains"), deck.Main[0]);
        // 45 of the Decks files carry all six sideboard colours.
        Assert.Equal(6, deck.Sideboards.Count);
        Assert.Contains(SideboardColor.White, deck.Sideboards.Keys);
    }

    [SkippableFact]
    public void PlaydeckDecodesAsMainOnly()
    {
        var install = RequireInstall();
        DeckList deck = install.LoadDeck(Path.Combine(install.Root, "Playdeck", "Ape Lord.dck"));

        Assert.Equal("Ape Lord", deck.Name);
        Assert.Empty(deck.Sideboards);
        Assert.True(deck.MainCardCount >= 40);
    }
}

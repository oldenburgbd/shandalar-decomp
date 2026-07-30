using Shandalar.Core;
using Xunit;

namespace Shandalar.Core.Tests;

/// <summary>
/// Golden tests against a real game install. Skipped automatically (including in CI)
/// unless SHANDALAR_GAME_DIR points at an original v3.0 install.
/// </summary>
public class RealInstallTests
{
    private static GameInstall RequireInstall()
    {
        var install = GameInstall.FromEnvironment();
        Skip.If(install is null, $"{GameInstall.EnvVar} is not set");
        return install!;
    }

    [SkippableFact]
    public void MasterCsvHasExactly1250Records()
    {
        var db = RequireInstall().LoadCardDatabase();
        Assert.Equal(1250, db.Cards.Count);
    }

    [SkippableFact]
    public void CardIdsAreDenseFromZero()
    {
        var db = RequireInstall().LoadCardDatabase();
        for (int i = 0; i < db.Cards.Count; i++)
            Assert.NotNull(db.Find(i));
    }

    [SkippableFact]
    public void KnownCardsResolveById()
    {
        var db = RequireInstall().LoadCardDatabase();
        Assert.Equal("Air Elemental", db.Find(0)!.Name);
        Assert.Equal("Ancestral Recall", db.Find(1)!.Name);
    }
}

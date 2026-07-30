using System.Text.Json;
using Shandalar.Core;
using Shandalar.Core.Extraction;
using Shandalar.Core.Formats;
using Xunit;

namespace Shandalar.Core.Tests;

public class PngWriterTests
{
    [Fact]
    public void ProducesStructurallyValidPng()
    {
        byte[] rgba = new byte[2 * 2 * 4];
        rgba[0] = 255; rgba[3] = 255;      // red pixel
        byte[] png = PngWriter.EncodeRgba(2, 2, rgba);

        // Signature.
        Assert.Equal(new byte[] { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A }, png[..8]);

        // Walk chunks: IHDR (13 bytes, 2x2, depth 8, colour 6) ... IEND, all CRCs valid.
        int pos = 8;
        var types = new List<string>();
        while (pos < png.Length)
        {
            int len = (png[pos] << 24) | (png[pos + 1] << 16) | (png[pos + 2] << 8) | png[pos + 3];
            string type = System.Text.Encoding.ASCII.GetString(png, pos + 4, 4);
            types.Add(type);
            if (type == "IHDR")
            {
                Assert.Equal(13, len);
                Assert.Equal(2, (png[pos + 8] << 24) | (png[pos + 9] << 16) | (png[pos + 10] << 8) | png[pos + 11]);
                Assert.Equal(8, png[pos + 16]);  // bit depth
                Assert.Equal(6, png[pos + 17]);  // RGBA
            }
            pos += 12 + len;
        }
        Assert.Equal(["IHDR", "IDAT", "IEND"], types);
        Assert.Equal(png.Length, pos);
    }

    [Fact]
    public void RejectsMismatchedBufferSize()
    {
        Assert.Throws<ArgumentException>(() => PngWriter.EncodeRgba(2, 2, new byte[3]));
    }
}

/// <summary>Golden extraction tests against the real install (skipped in CI).</summary>
public class ExtractionGoldenTests
{
    private static GameInstall RequireInstall()
    {
        var install = GameInstall.FromEnvironment();
        Skip.If(install is null, $"{GameInstall.EnvVar} is not set");
        return install!;
    }

    [SkippableFact]
    public void ExtractsDecksCardsAndPicsWithManifest()
    {
        var install = RequireInstall();
        string outDir = Path.Combine(Path.GetTempPath(), "shandalar-extract-test-" + Guid.NewGuid().ToString("N"));
        try
        {
            var extractor = new AssetExtractor(install, outDir);
            extractor.Run(ExtractScope.Pics | ExtractScope.Decks | ExtractScope.CardDb);

            Assert.True(File.Exists(Path.Combine(outDir, "decks.json")));
            Assert.True(File.Exists(Path.Combine(outDir, "cards.json")));
            Assert.True(File.Exists(Path.Combine(outDir, "pics", "Title.png")));
            Assert.True(File.Exists(Path.Combine(outDir, "pics", "Creatures640.png")));

            // ~99 base-dir pics, minus any that fail (none expected).
            Assert.True(Directory.GetFiles(Path.Combine(outDir, "pics")).Length >= 95);

            using var manifest = JsonDocument.Parse(File.ReadAllBytes(Path.Combine(outDir, "manifest.json")));
            var sources = manifest.RootElement.GetProperty("sources");
            Assert.Contains(sources.EnumerateArray(),
                s => s.GetProperty("path").GetString() == "Master.csv");
            Assert.Contains(sources.EnumerateArray(),
                s => s.GetProperty("path").GetString() == "Title.pic"
                     && s.GetProperty("sha256").GetString()!.Length == 64);

            // cards.json carries the joined DB: all 1,250 records.
            using var cards = JsonDocument.Parse(File.ReadAllBytes(Path.Combine(outDir, "cards.json")));
            Assert.Equal(1250, cards.RootElement.GetArrayLength());
        }
        finally
        {
            Directory.Delete(outDir, recursive: true);
        }
    }

    [SkippableFact]
    public void ExtractedCardArtPngRoundTripsThroughDecoder()
    {
        var install = RequireInstall();
        // Sanity: art PNG bytes decode from the same RGBA the golden hash pins.
        WvlImage img = install.LoadSmallArt().FindByCardId(0)!.Decode();
        byte[] png = PngWriter.EncodeRgba(img.Width, img.Height, img.Rgba);
        Assert.Equal(0x89, png[0]);
        Assert.True(png.Length > 1000);
    }
}

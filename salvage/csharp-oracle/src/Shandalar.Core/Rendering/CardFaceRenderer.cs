using Shandalar.Core.Cards;
using Shandalar.Core.Formats;

namespace Shandalar.Core.Rendering;

/// <summary>
/// Composes card faces from original assets: colour frame (Cardart/Cardbk_*.pic) + medium
/// card art (Cardart/Medart.cat) scaled into the art window + mana cost pips. Text (name,
/// type, rules) is not rasterized here — the original drew it with Windows fonts, so the
/// viewer overlays it; the layout rects in <see cref="CardFace"/> say where.
/// </summary>
public sealed class CardFaceRenderer
{
    /// <summary>Cost pip size in the name bar (the bar is 14px tall).</summary>
    public const int CostPipSize = 14;

    private readonly GameInstall _install;
    private readonly Palette _duelPalette;
    private readonly ManaStrip _manaStrip;
    private readonly SetSymbols _setSymbols;
    private readonly CatArchive _mediumArt;
    private readonly Dictionary<string, RgbaImage> _frames = new(StringComparer.OrdinalIgnoreCase);

    private CardFaceRenderer(
        GameInstall install, Palette duelPalette, ManaStrip manaStrip, SetSymbols setSymbols,
        CatArchive mediumArt)
    {
        _install = install;
        _duelPalette = duelPalette;
        _manaStrip = manaStrip;
        _setSymbols = setSymbols;
        _mediumArt = mediumArt;
    }

    public static CardFaceRenderer Load(GameInstall install)
    {
        var pal = PlogPalette.Load(install.PathTo("Duel.plogpal"));
        var strip = Pic.Decode(
            File.ReadAllBytes(install.PathTo(Path.Combine("Cardart", "Manasymbols.pic"))));
        var sets = Pic.Decode(
            File.ReadAllBytes(install.PathTo(Path.Combine("Cardart", "Cardsets.pic"))));
        return new CardFaceRenderer(
            install, pal, new ManaStrip(strip, pal), new SetSymbols(sets, pal),
            install.LoadMediumArt());
    }

    /// <summary>
    /// Renders the face bitmap for a card: frame + art + cost pips (no text). Lands and
    /// internal records draw no cost. <paramref name="set"/> (from Master.csv/Info.csv)
    /// selects set-specific land frames.
    /// </summary>
    public RgbaImage Render(CardStats stats, string? set = null)
    {
        var face = GetFrame(CardFace.FrameFileFor(stats, set)).Copy();

        var art = _mediumArt.FindByCardId(stats.Id);
        if (art != null)
            Blit.Scaled(face, RgbaImage.FromWvl(art.Decode()), CardFace.ArtWindow);

        if (stats.ColorCode is not (CardColorCode.Land or CardColorCode.None))
        {
            int[] cells = ManaStrip.CostCells(stats.Mana);
            int x = CardFace.NameBar.X + CardFace.NameBar.Width - cells.Length * CostPipSize;
            foreach (int cell in cells)
            {
                _manaStrip.DrawCell(face, cell, x, CardFace.NameBar.Y, CostPipSize);
                x += CostPipSize;
            }
        }

        // Expansion symbol right-aligned on the type line; core set has none (like real cards).
        if (set is not null)
            _setSymbols.TryDraw(face, set,
                CardFace.TypeLine.X + CardFace.TypeLine.Width, CardFace.TypeLine.Y - 1);

        return face;
    }

    private RgbaImage GetFrame(string file)
    {
        if (_frames.TryGetValue(file, out var cached))
            return cached;
        var pic = Pic.Decode(
            File.ReadAllBytes(_install.PathTo(Path.Combine(CardFace.FrameDir, file))));
        var frame = RgbaImage.FromPic(pic, pic.Palette is null ? _duelPalette : null);
        _frames[file] = frame;
        return frame;
    }
}

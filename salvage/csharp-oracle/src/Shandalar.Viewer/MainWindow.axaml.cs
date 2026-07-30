using System.Globalization;
using Avalonia.Controls;
using Avalonia.Interactivity;
using Avalonia.Media;
using Avalonia.Media.Imaging;
using Shandalar.Core;
using Shandalar.Core.Cards;
using Shandalar.Core.Formats;
using Shandalar.Core.Rendering;

namespace Shandalar.Viewer;

public partial class MainWindow : Window
{
    private GameInstall? _install;
    private CardFaceRenderer? _faceRenderer;
    private CardDatabase? _cardDb;
    private Dictionary<int, CardStats> _stats = new();
    private Dictionary<int, InfoRecord> _info = new();
    private List<CardItem> _cards = new();
    private List<AssetItem> _pics = new();
    private List<AssetItem> _sprs = new();
    private List<PaletteChoice> _palettes = new();

    private sealed record CardItem(int Id, string Name)
    {
        public override string ToString() => $"{Id:D4}  {Name}";
    }

    private sealed record AssetItem(string RelPath)
    {
        public override string ToString() => RelPath;
    }

    private sealed record PaletteChoice(string Label, Func<Palette?> Resolve)
    {
        public override string ToString() => Label;
    }

    public MainWindow()
    {
        InitializeComponent();
        var install = TryOpen(Environment.GetEnvironmentVariable(GameInstall.EnvVar));
        if (install is null)
            InstallBanner.IsVisible = true;
        else
            LoadInstall(install);
    }

    private static GameInstall? TryOpen(string? dir)
    {
        if (string.IsNullOrWhiteSpace(dir))
            return null;
        try
        {
            return GameInstall.Open(dir);
        }
        catch
        {
            return null;
        }
    }

    private void OnOpenInstall(object? sender, RoutedEventArgs e)
    {
        var install = TryOpen(InstallPathBox.Text);
        if (install is null)
        {
            InstallError.Text = "Not a game install (Magic.exe / Master.csv not found there).";
            return;
        }
        InstallBanner.IsVisible = false;
        LoadInstall(install);
    }

    private void LoadInstall(GameInstall install)
    {
        _install = install;
        _cardDb = install.LoadCardDatabase();
        _stats = install.LoadCardStats().ToDictionary(s => s.Id);
        _info = install.LoadInfo().ToDictionary(i => i.Id);
        _faceRenderer = CardFaceRenderer.Load(install);

        _cards = _cardDb.Cards.OrderBy(c => c.Id).Select(c => new CardItem(c.Id, c.Name)).ToList();
        _pics = EnumerateAssets("*.pic");
        _sprs = EnumerateAssets("*.spr");
        _palettes = BuildPaletteChoices();

        CardList.ItemsSource = _cards;
        PicList.ItemsSource = _pics;
        SprList.ItemsSource = _sprs;
        PicPalette.ItemsSource = _palettes;
        PicPalette.SelectedIndex = 0;
        SprPalette.ItemsSource = _palettes.Skip(1).ToList(); // sprites have no embedded palette
        SprPalette.SelectedIndex = 0; // Winbak01 — the sprite default (docs/formats/palettes.md)
        CardList.SelectedIndex = 0;
    }

    private List<AssetItem> EnumerateAssets(string pattern) =>
        Directory.EnumerateFiles(_install!.Root, pattern, SearchOption.AllDirectories)
            .Select(p => new AssetItem(Path.GetRelativePath(_install.Root, p)))
            .OrderBy(a => a.RelPath, StringComparer.OrdinalIgnoreCase)
            .ToList();

    /// <summary>"Auto" first, then Winbak01's embedded palette, then every palette file found.</summary>
    private List<PaletteChoice> BuildPaletteChoices()
    {
        var choices = new List<PaletteChoice> { new("Auto", () => null) };

        string winbak = _install!.PathTo("Winbak01.pic");
        if (File.Exists(winbak))
            choices.Add(new PaletteChoice("Winbak01.pic (embedded)",
                () => Pic.Decode(File.ReadAllBytes(winbak)).Palette));

        var files = Directory.EnumerateFiles(_install.Root, "*.plogpal")
            .Concat(Directory.EnumerateFiles(_install.Root, "*.tr"))
            .OrderBy(p => p, StringComparer.OrdinalIgnoreCase);
        foreach (var path in files)
        {
            string name = Path.GetFileName(path);
            choices.Add(new PaletteChoice(name, () => Palette.FromFile(path)));
        }
        return choices;
    }

    // ------------------------------------------------------------------ cards

    private void OnCardSearchChanged(object? sender, TextChangedEventArgs e)
    {
        string q = CardSearch.Text ?? "";
        CardList.ItemsSource = q.Length == 0
            ? _cards
            : _cards.Where(c => c.Name.Contains(q, StringComparison.OrdinalIgnoreCase)
                || c.Id.ToString("D4").Contains(q)).ToList();
        CardList.SelectedIndex = 0;
    }

    private void OnCardSelected(object? sender, SelectionChangedEventArgs e)
    {
        if (CardList.SelectedItem is not CardItem item || _faceRenderer is null)
            return;

        var record = _cardDb!.Find(item.Id);
        var info = _info.GetValueOrDefault(item.Id);
        var stats = _stats.GetValueOrDefault(item.Id)
            ?? new CardStats(item.Id, item.Name, "", CardColorCode.None,
                CardCategory.Unknown, default, 0, 0, 0);

        RgbaImage face = _faceRenderer.Render(stats, info?.Set);
        FaceImage.Source = ToBitmap(face);

        FaceName.Text = item.Name;
        FaceType.Text = record?.TypeLine ?? "";
        FaceRules.Text = record?.RulesText ?? "";
        FaceFlavor.Text = record?.FlavorText ?? "";
        FaceArtist.Text = record?.Artist is { Length: > 0 } a ? $"Illus. {a}" : "";
        FacePT.Text = stats.IsCreature ? $"{stats.Power}/{stats.Toughness}" : "";

        // The original picks white or black text per frame; match it by sampling each
        // region's frame luminance (left part of the bars, clear of pips and the P/T corner).
        FaceName.Foreground = ContrastBrush(face, CardFace.NameBar, 0.5);
        FaceType.Foreground = ContrastBrush(face, CardFace.TypeLine, 0.5);
        var bottom = ContrastBrush(face, CardFace.BottomBar, 0.5);
        FaceArtist.Foreground = bottom;
        FacePT.Foreground = bottom;

        // Rules text fills the box like the original: use the largest size that still fits.
        double size = FitRulesSize(FaceRules.Text, FaceFlavor.Text);
        FaceRules.FontSize = size;
        FaceFlavor.FontSize = Math.Max(6, size - 1);

        DetailTitle.Text = $"{item.Id:D4} — {item.Name}";
        DetailFacts.Text = string.Join('\n', new[]
        {
            record?.TypeLine,
            info is null ? null : $"{info.Set} · {info.Rarity}",
            stats.ColorCode == CardColorCode.None ? null : $"Colour: {stats.ColorCode}",
            stats.IsCreature ? $"Power/Toughness: {stats.Power}/{stats.Toughness}" : null,
            stats.Mana.ConvertedManaCost > 0 || stats.Mana.IsVariable
                ? $"Mana: {FormatMana(stats.Mana)}" : null,
        }.Where(s => !string.IsNullOrEmpty(s)));
        DetailRules.Text = record?.RulesText ?? "";
        DetailFlavor.Text = record?.FlavorText ?? "";
    }

    /// <summary>White text on dark frame regions, black on light ones.</summary>
    private static IBrush ContrastBrush(RgbaImage face, FaceRect rect, double widthFraction)
    {
        long sum = 0;
        int count = 0;
        int w = (int)(rect.Width * widthFraction);
        for (int y = rect.Y; y < rect.Y + rect.Height && y < face.Height; y += 2)
            for (int x = rect.X; x < rect.X + w && x < face.Width; x += 2)
            {
                int o = (y * face.Width + x) * 4;
                sum += (face.Pixels[o] * 299 + face.Pixels[o + 1] * 587 + face.Pixels[o + 2] * 114) / 1000;
                count++;
            }
        return count > 0 && sum / count < 110 ? Brushes.White : Brushes.Black;
    }

    /// <summary>Largest rules-text size (13 down to 6) whose rules + flavor fit the text box.</summary>
    private static double FitRulesSize(string rules, string flavor)
    {
        const double boxHeight = 92, boxWidth = 174;
        var sans = new Typeface(new FontFamily("Arial,Helvetica,sans-serif"));
        var italic = new Typeface(new FontFamily("Arial,Helvetica,sans-serif"), FontStyle.Italic);
        for (double size = 13; size > 6; size -= 0.5)
        {
            double h = 0;
            if (rules.Length > 0)
                h += MeasureHeight(rules, sans, size, boxWidth);
            if (flavor.Length > 0)
                h += 3 + MeasureHeight(flavor, italic, Math.Max(6, size - 1), boxWidth);
            if (h <= boxHeight)
                return size;
        }
        return 6;
    }

    private static double MeasureHeight(string text, Typeface typeface, double size, double maxWidth)
    {
        var ft = new FormattedText(text, CultureInfo.InvariantCulture, FlowDirection.LeftToRight,
            typeface, size, Brushes.Black)
        { MaxTextWidth = maxWidth };
        return ft.Height;
    }

    private static string FormatMana(ManaCost m)
    {
        var s = "";
        if (m.IsVariable)
            s += "X";
        else if (m.Generic > 0)
            s += m.Generic;
        s += new string('W', m.White) + new string('U', m.Blue) + new string('B', m.Black)
            + new string('R', m.Red) + new string('G', m.Green);
        return s.Length == 0 ? "0" : s;
    }

    // ------------------------------------------------------------------- pics

    private void OnPicSearchChanged(object? sender, TextChangedEventArgs e) =>
        FilterList(PicList, _pics, PicSearch.Text);

    private void OnSprSearchChanged(object? sender, TextChangedEventArgs e) =>
        FilterList(SprList, _sprs, SprSearch.Text);

    private static void FilterList(ListBox list, List<AssetItem> all, string? query)
    {
        string q = query ?? "";
        list.ItemsSource = q.Length == 0
            ? all
            : all.Where(a => a.RelPath.Contains(q, StringComparison.OrdinalIgnoreCase)).ToList();
    }

    private void OnPicSelected(object? sender, SelectionChangedEventArgs e)
    {
        if (PicList?.SelectedItem is not AssetItem item || _install is null)
            return;
        try
        {
            PicImage img = Pic.Decode(File.ReadAllBytes(_install.PathTo(item.RelPath)));
            Palette? chosen = (PicPalette.SelectedItem as PaletteChoice)?.Resolve();
            Palette pal = chosen ?? img.Palette ?? AutoPaletteFor(item.RelPath);
            PicView.Source = ToBitmap(new RgbaImage(img.Width, img.Height, img.ToRgba(pal)));
            string src = chosen is not null ? "selected"
                : img.Palette is not null ? "embedded" : "auto";
            PicInfo.Text = $"{img.Width}×{img.Height} · palette: {src}";
        }
        catch (Exception ex)
        {
            PicView.Source = null;
            PicInfo.Text = $"decode failed: {ex.Message}";
        }
    }

    /// <summary>Same defaults as extraction: Creatures* uses Duel.plogpal, else Winbak01.</summary>
    private Palette AutoPaletteFor(string relPath)
    {
        string name = Path.GetFileNameWithoutExtension(relPath);
        bool duelArt = name.StartsWith("Creatures", StringComparison.OrdinalIgnoreCase)
            || relPath.Contains(Path.DirectorySeparatorChar);
        return duelArt
            ? Palette.FromFile(_install!.PathTo("Duel.plogpal"))
            : Pic.Decode(File.ReadAllBytes(_install!.PathTo("Winbak01.pic"))).Palette!;
    }

    // ---------------------------------------------------------------- sprites

    private void OnSprSelected(object? sender, SelectionChangedEventArgs e)
    {
        if (SprList?.SelectedItem is not AssetItem item || _install is null)
            return;
        try
        {
            var frames = Spr.Decode(File.ReadAllBytes(_install.PathTo(item.RelPath)));
            Palette pal = (SprPalette.SelectedItem as PaletteChoice)?.Resolve()
                ?? Pic.Decode(File.ReadAllBytes(_install.PathTo("Winbak01.pic"))).Palette!;
            RgbaImage sheet = SpriteSheet.Build(frames, pal);
            SprView.Source = ToBitmap(sheet);
            SprInfo.Text = $"{frames.Count} frames · sheet {sheet.Width}×{sheet.Height}";
        }
        catch (Exception ex)
        {
            SprView.Source = null;
            SprInfo.Text = $"decode failed: {ex.Message}";
        }
    }

    private static Bitmap ToBitmap(RgbaImage image)
    {
        using var ms = new MemoryStream(image.ToPng());
        return new Bitmap(ms);
    }
}

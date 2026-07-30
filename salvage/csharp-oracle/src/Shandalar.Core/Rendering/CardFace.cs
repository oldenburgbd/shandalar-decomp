using Shandalar.Core.Cards;

namespace Shandalar.Core.Rendering;

/// <summary>
/// Card face geometry and frame selection (see docs/formats/card-face.md). The frame templates
/// are Cardart/Cardbk_*.pic, all 228x325; rects were measured from Cardbk_Red.pic. The
/// original's exact layout constants and land-frame dispatch live in Magic.exe and will be
/// confirmed when the card-draw routine is decompiled (#11); until then this is a documented
/// best-effort reconstruction.
/// </summary>
public static class CardFace
{
    public const int Width = 228;
    public const int Height = 325;

    /// <summary>Directory (relative to the install root) holding the frame templates.</summary>
    public const string FrameDir = "Cardart";

    /// <summary>Card name, left-aligned; mana cost pips are right-aligned in the same bar.</summary>
    public static readonly FaceRect NameBar = new(12, 3, 204, 14);

    public static readonly FaceRect ArtWindow = new(20, 22, 188, 158);

    /// <summary>Type line ("Summon Angel") between the art window and the text box.</summary>
    public static readonly FaceRect TypeLine = new(20, 182, 188, 13);

    /// <summary>Rules + flavor text parchment area.</summary>
    public static readonly FaceRect TextBox = new(24, 200, 180, 96);

    /// <summary>Artist credit on the left; power/toughness on the right.</summary>
    public static readonly FaceRect BottomBar = new(12, 300, 204, 20);

    /// <summary>
    /// Picks the frame template (relative to Cardart/) for a card. Lands get set-specific
    /// frames; core-set basics get their colour's land frame; anything unresolved falls back
    /// to Cardbk_Special (the Astral marble frame).
    /// </summary>
    public static string FrameFileFor(CardStats stats, string? set = null)
    {
        if (stats.ColorCode == CardColorCode.Land)
        {
            var bySet = set switch
            {
                "Antiquities" => "Cardbk_Antiquitiesland.pic",
                "Arabian" => "Cardbk_Arabiannightsland.pic",
                "Dark" => "Cardbk_Darklandsland.pic",
                "Legends" => "Cardbk_Legendsland.pic",
                _ => null,
            };
            if (bySet != null)
                return bySet;
            return stats.Name switch
            {
                "Plains" => "Cardbk_Whiteland.pic",
                "Island" => "Cardbk_Blueland.pic",
                "Swamp" => "Cardbk_Blackland.pic",
                "Mountain" => "Cardbk_Redland.pic",
                "Forest" => "Cardbk_Greenland.pic",
                _ => "Cardbk_Special.pic",
            };
        }

        return stats.ColorCode switch
        {
            CardColorCode.Black => "Cardbk_Black.pic",
            CardColorCode.Blue => "Cardbk_Blue.pic",
            CardColorCode.Artifact => "Cardbk_Artifact.pic",
            CardColorCode.Gold => "Cardbk_Gold.pic",
            CardColorCode.Green => "Cardbk_Green.pic",
            CardColorCode.Red => "Cardbk_Red.pic",
            CardColorCode.White => "Cardbk_White.pic",
            _ => "Cardbk_Special.pic",
        };
    }
}

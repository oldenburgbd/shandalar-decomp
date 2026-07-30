using Shandalar.Core.Cards;
using Shandalar.Core.Rendering;
using Xunit;

namespace Shandalar.Core.Tests;

public class CardFaceTests
{
    private static CardStats Stats(
        CardColorCode color, string name = "Test", CardCategory category = CardCategory.Creature,
        ManaCost mana = default) =>
        new(0, name, "", color, category, mana, 1, 1, 0);

    [Theory]
    [InlineData(CardColorCode.Black, "Cardbk_Black.pic")]
    [InlineData(CardColorCode.Blue, "Cardbk_Blue.pic")]
    [InlineData(CardColorCode.Artifact, "Cardbk_Artifact.pic")]
    [InlineData(CardColorCode.Gold, "Cardbk_Gold.pic")]
    [InlineData(CardColorCode.Green, "Cardbk_Green.pic")]
    [InlineData(CardColorCode.Red, "Cardbk_Red.pic")]
    [InlineData(CardColorCode.White, "Cardbk_White.pic")]
    [InlineData(CardColorCode.None, "Cardbk_Special.pic")]
    public void Frame_follows_colour_code(CardColorCode color, string expected) =>
        Assert.Equal(expected, CardFace.FrameFileFor(Stats(color)));

    [Theory]
    [InlineData("Plains", null, "Cardbk_Whiteland.pic")]
    [InlineData("Island", null, "Cardbk_Blueland.pic")]
    [InlineData("Swamp", null, "Cardbk_Blackland.pic")]
    [InlineData("Mountain", null, "Cardbk_Redland.pic")]
    [InlineData("Forest", null, "Cardbk_Greenland.pic")]
    [InlineData("City of Brass", "Arabian", "Cardbk_Arabiannightsland.pic")]
    [InlineData("Maze of Ith", "Dark", "Cardbk_Darklandsland.pic")]
    [InlineData("Mishra's Factory", "Antiquities", "Cardbk_Antiquitiesland.pic")]
    [InlineData("Karakas", "Legends", "Cardbk_Legendsland.pic")]
    [InlineData("Badlands", "Magic", "Cardbk_Special.pic")]
    public void Land_frames_follow_set_then_basic_name(string name, string? set, string expected) =>
        Assert.Equal(
            expected,
            CardFace.FrameFileFor(Stats(CardColorCode.Land, name, CardCategory.Land), set));

    [Fact]
    public void Cost_cells_serra_angel()
    {
        // 3WW -> numeral 3, then two white pips.
        var cells = ManaStrip.CostCells(new ManaCost(3, 2, 0, 0, 0, 0, false));
        Assert.Equal([ManaStrip.CellForNumeral(3), ManaStrip.CellWhite, ManaStrip.CellWhite], cells);
    }

    [Fact]
    public void Cost_cells_x_spell()
    {
        // XR (Disintegrate) -> X cell, then a red pip; no numeral.
        var cells = ManaStrip.CostCells(new ManaCost(0, 0, 0, 0, 1, 0, true));
        Assert.Equal([ManaStrip.CellX, ManaStrip.CellRed], cells);
    }

    [Fact]
    public void Cost_cells_zero_cost_shows_numeral_zero()
    {
        var cells = ManaStrip.CostCells(new ManaCost(0, 0, 0, 0, 0, 0, false));
        Assert.Equal([ManaStrip.CellForNumeral(0)], cells);
    }

    [Fact]
    public void Cost_cells_coloured_only_has_no_numeral()
    {
        // BB (Dark Ritual is B; use BB) -> two black pips only.
        var cells = ManaStrip.CostCells(new ManaCost(0, 0, 0, 2, 0, 0, false));
        Assert.Equal([ManaStrip.CellBlack, ManaStrip.CellBlack], cells);
    }

    [Fact]
    public void Blit_scales_nearest_neighbour()
    {
        // 2x2 source (R G / B W) doubled into a 4x4 dest: each source pixel becomes a 2x2 block.
        byte[] src = new byte[16];
        void Px(int i, byte r, byte g, byte b)
        {
            src[i * 4] = r;
            src[i * 4 + 1] = g;
            src[i * 4 + 2] = b;
            src[i * 4 + 3] = 255;
        }
        Px(0, 255, 0, 0);
        Px(1, 0, 255, 0);
        Px(2, 0, 0, 255);
        Px(3, 255, 255, 255);

        var dest = new RgbaImage(4, 4, new byte[64]);
        Blit.Scaled(dest, new RgbaImage(2, 2, src), new FaceRect(0, 0, 4, 4));

        // Top-left block red, top-right block green.
        Assert.Equal(255, dest.Pixels[(0 * 4 + 1) * 4]);      // (1,0) R
        Assert.Equal(255, dest.Pixels[(1 * 4 + 0) * 4]);      // (0,1) R
        Assert.Equal(255, dest.Pixels[(0 * 4 + 2) * 4 + 1]);  // (2,0) G
        // Bottom-right block white.
        Assert.Equal(255, dest.Pixels[(3 * 4 + 3) * 4 + 2]);  // (3,3) B channel
    }

    [Fact]
    public void Blit_clips_outside_dest()
    {
        var dest = new RgbaImage(2, 2, new byte[16]);
        var src = new RgbaImage(2, 2, new byte[16]);
        // Rect partly outside: must not throw.
        Blit.Scaled(dest, src, new FaceRect(1, 1, 4, 4));
        Blit.Scaled(dest, src, new FaceRect(-2, -2, 4, 4));
    }
}

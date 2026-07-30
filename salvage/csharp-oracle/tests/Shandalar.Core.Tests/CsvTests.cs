using Shandalar.Core.Text;
using Xunit;

namespace Shandalar.Core.Tests;

public class CsvTests
{
    private static List<string[]> Parse(string text) =>
        Csv.ReadRecords(new StringReader(text)).ToList();

    [Fact]
    public void ParsesSimpleRows()
    {
        var rows = Parse("a,b,c\nd,e,f\n");
        Assert.Equal(2, rows.Count);
        Assert.Equal(["a", "b", "c"], rows[0]);
        Assert.Equal(["d", "e", "f"], rows[1]);
    }

    [Fact]
    public void HandlesQuotedFieldWithCommaAndEscapedQuotes()
    {
        var rows = Parse("x,\"hello, \"\"world\"\"\",y\n");
        Assert.Equal(["x", "hello, \"world\"", "y"], Assert.Single(rows));
    }

    [Fact]
    public void HandlesNewlineInsideQuotedField()
    {
        var rows = Parse("a,\"line1\nline2\",b\n");
        Assert.Equal(["a", "line1\nline2", "b"], Assert.Single(rows));
    }

    [Fact]
    public void HandlesRaggedRowsAndMissingFinalNewline()
    {
        var rows = Parse("a,b\nc,d,e,f\ng");
        Assert.Equal(3, rows.Count);
        Assert.Equal(2, rows[0].Length);
        Assert.Equal(4, rows[1].Length);
        Assert.Equal(["g"], rows[2]);
    }

    [Fact]
    public void HandlesCrLfLineEndings()
    {
        var rows = Parse("a,b\r\nc,d\r\n");
        Assert.Equal(2, rows.Count);
        Assert.Equal(["c", "d"], rows[1]);
    }
}

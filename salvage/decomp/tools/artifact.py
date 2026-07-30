"""Turn progress/index.html into a body fragment publishable as an Artifact.

progress.ps1 rewrites index.html on every measured build, so this must not be a
hand-edited copy -- it would drift the moment anyone runs a build. It APPENDS
rather than rewrites: the palette, the treemap and the tables come through
untouched, and everything added here is either required by the Artifact host or
is a gap that only shows on a phone.

  1. The host wraps the file in its own <!doctype>/<head>/<body>, so those are
     stripped and the content emitted directly.
  2. The host's theme toggle stamps data-theme on the root element, which has to
     beat the prefers-color-scheme query in BOTH directions. The two token
     blocks are parsed out of the page's own CSS and re-emitted as data-theme
     rules, so a palette change in progress.ps1 carries over by itself.
  3. The treemap showed its tooltip on mousemove only. There is no hover on a
     phone, so a tap on a tile did nothing legible. A separate listener adds
     tap-to-inspect; it reads the same __k the page already hangs on each rect,
     so the generated script is not touched.

    python tools/artifact.py [-o out.html]
"""
import io, re, sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
SRC = HERE.parent / "progress" / "index.html"
OUT = HERE.parent / "progress" / "artifact.html"


def grab(pattern, text, what):
    m = re.search(pattern, text, re.S)
    if not m:
        sys.exit("could not find %s in %s" % (what, SRC))
    return m.group(1)


def main():
    if "-o" in sys.argv:
        out = Path(sys.argv[sys.argv.index("-o") + 1])
    else:
        out = OUT
    s = io.open(SRC, encoding="utf-8").read()

    # progress.ps1 writes an em dash through a code path that double-encodes it
    s = s.replace("â€”", "—")

    title = grab(r"<title>(.*?)</title>", s, "the title")
    style = grab(r"<style>(.*?)</style>", s, "the stylesheet")
    wrap = grab(r'(<div class="wrap">.*?</div>\s*)\n<script id="invdata"', s,
                "the page body")
    invdata = grab(r'(<script id="invdata".*?</script>)', s, "the data island")
    main_js = grab(r'</script>\s*(<script>\n\(function.*?</script>)', s,
                   "the page script")

    # The page defines its palette twice: once bare for dark, once inside the
    # light media query. Lift both so the host's toggle can select either.
    dark = grab(r":root \{(.*?)\}", style, "the dark tokens")
    light = grab(r"@media \(prefers-color-scheme: light\) \{\s*:root \{(.*?)\}",
                 style, "the light tokens")

    extra = """
/* --- added for the Artifact host ------------------------------------------
   The viewer's theme toggle stamps data-theme on the root element and has to
   win over the OS preference in both directions. These are the page's own two
   token blocks, lifted verbatim. */
:root[data-theme="dark"] {%s}
:root[data-theme="light"] {%s}

/* --- added for phones ----------------------------------------------------- */
html { -webkit-text-size-adjust: 100%%; }
/* Digits that sit in a column should line up. */
.num, .pct, .mono { font-variant-numeric: tabular-nums; }
/* The table scrolls inside its own box; keep that from arming the browser's
   back-swipe, and never let the page itself scroll sideways. */
.tableWrap { overscroll-behavior-x: contain; -webkit-overflow-scrolling: touch; }
body { overflow-x: hidden; }
/* A tapped tooltip is placed, not hovered, so it needs to stay on screen. */
#tmTip { max-width: min(74vw, 320px); white-space: normal; text-align: center; }
#tm rect.f { touch-action: manipulation; }
.crumbs button { min-height: 32px; }
@media (max-width: 560px) {
  .wrap { padding: 18px 14px 48px; }
  .big { font-size: 32px; }
  .hero { padding: 16px; }
  h1 { font-size: 19px; }
  /* Tiles are small on a narrow screen; say so rather than leaving a dead map. */
  #tmWrap::after {
    content: "Tap a tile for its name, size and match.";
    display: block; margin-top: 9px; color: var(--dim); font-size: 12px;
  }
}
@media (prefers-reduced-motion: reduce) {
  * { animation-duration: .01ms !important; transition-duration: .01ms !important; }
}
""" % (dark, light)

    touch = """<script>
/* Tap-to-inspect for the coverage map.
   The generated script wires the tooltip to mousemove, which a phone never
   fires. Each tile already carries its record on the rect as __k, and the
   tooltip is addressable by id, so this reads both without reaching inside
   that closure -- it keeps working when progress.ps1 regenerates the page.
   Mouse pointers are left alone; they already have hover. */
(function () {
  var svg = document.getElementById("tm"), tip = document.getElementById("tmTip");
  if (!svg || !tip) return;

  function bytes(n) {
    if (n >= 1048576) return (n / 1048576).toFixed(2) + " MB";
    if (n >= 1024) return (n / 1024).toFixed(1) + " kB";
    return Math.round(n) + " B";
  }
  function state(k) {
    if (k.score === null || k.score === undefined) {
      return (k.value > 0 ? (100 * k.matched / k.value) : 0).toFixed(2) + "% matching";
    }
    if (k.score === -2) return "stubbed";
    if (k.score < 0) return "not started";
    return k.score.toFixed(2) + "% matching";
  }

  var mark = null;
  function clear() {
    if (mark) { mark.remove(); mark = null; }
    tip.hidden = true;
  }

  svg.addEventListener("pointerdown", function (ev) {
    if (ev.pointerType === "mouse") return;
    var t = ev.target, k = t.__k;
    if (!k) { clear(); return; }
    if (mark) mark.remove();
    mark = document.createElementNS("http://www.w3.org/2000/svg", "rect");
    mark.setAttribute("class", "hl");
    ["x", "y", "width", "height"].forEach(function (a) {
      mark.setAttribute(a, t.getAttribute(a));
    });
    svg.appendChild(mark);

    tip.textContent = k.label + " \\u2022 " + bytes(k.value) + " \\u2022 " + state(k)
                      + (k.drill ? " \\u2022 tap again to open" : "");
    tip.hidden = false;

    // Place it over the tile, then pull it back inside the map's edges. The
    // stylesheet lifts it by 135% of its own height, so the anchor has to clear
    // that much or the tooltip rides above the map.
    var box = svg.getBoundingClientRect(), tb = t.getBoundingClientRect();
    var x = tb.left + tb.width / 2 - box.left;
    var half = tip.offsetWidth / 2 + 6;
    var lift = tip.offsetHeight * 1.35 + 4;
    tip.style.left = Math.max(half, Math.min(box.width - half, x)) + "px";
    tip.style.top = Math.max(lift, tb.top + tb.height / 2 - box.top) + "px";
  }, { passive: true });

  document.addEventListener("pointerdown", function (ev) {
    if (ev.pointerType !== "mouse" && !svg.contains(ev.target)) clear();
  }, { passive: true });
  addEventListener("resize", clear);
})();
</script>"""

    body = "\n".join(["<title>%s</title>" % title,
                      "<style>%s%s</style>" % (style, extra),
                      wrap.rstrip(), invdata, main_js, touch, ""])
    io.open(out, "w", encoding="utf-8", newline="").write(body)
    print("wrote %s (%d bytes)" % (out, len(body.encode("utf-8"))))


if __name__ == "__main__":
    main()

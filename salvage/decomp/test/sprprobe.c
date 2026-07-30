/* Differential test driver: runs the RECONSTRUCTED C over real game data and
 * emits the same columns as `shandalar sprprobe`, so the two can be diffed.
 *
 * This exists because reccmp only proves our machine code resembles the
 * original's. It says nothing about behaviour, and for every function below
 * 100% we had no evidence of correctness at all beyond reading a diff by eye.
 * This is the second axis: run the reconstruction on real files and compare
 * against the independently-verified C# reader.
 *
 * It deliberately exercises the reconstruction rather than reimplementing
 * anything — the frame walk is Spr_LoadFile (0x0057b710), the measurement is
 * Spr_GetFrameExtent (0x0057c450). A bug in either shows up as a diff.
 *
 * Safety: only functions that touch no dangerous globals are called here.
 * Sid_CreateScreenPage must never be reached (quirks Q-009), and nothing in
 * this driver can reach it. The game directory is opened read-only.
 *
 * Columns, one line per frame:
 *   <file> <frame> <width> <height> <u1> <u2> <emptyLines> <field14> <minX> <maxX>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sidlib/sidsprite.h"

/* The original performs no bounds checking on the caller's frame array, so the
 * caller is responsible for making it big enough. The largest sheet in the
 * shipped install is well under this. */
#define MAX_FRAMES 8192

static void *g_frames[MAX_FRAMES];

static const char *baseName(const char *path)
{
    const char *slash = strrchr(path, '\\');
    const char *fwd = strrchr(path, '/');

    if (fwd != NULL && (slash == NULL || fwd > slash)) {
        slash = fwd;
    }
    return (slash != NULL) ? slash + 1 : path;
}

static void probeFile(const char *path)
{
    int count;
    int i;

    count = Spr_LoadFile(g_frames, path);

    for (i = 0; i < count; i++) {
        const SprFrame *f = (const SprFrame *)g_frames[i];
        int minX;
        int maxX;

        Spr_GetFrameExtent(f, &minX, &maxX);

        /* The header fields are 16-bit; print them unsigned so the columns line
         * up with the C# side, which reads them as ushort. The struct keeps
         * them signed because that is how the original reads encodedRows
         * (movsx), and changing the type would change generated code. */
        printf("%s %d %d %d %d %d %d %d %d %d\n",
               baseName(path), i,
               f->width & 0xffff, f->height & 0xffff,
               f->unknown8 & 0xffff, f->unknown10 & 0xffff,
               f->emptyLines & 0xffff, f->encodedRows & 0xffff,
               minX, maxX);
    }
}

int main(int argc, char **argv)
{
    int i;

    if (argc < 2) {
        fprintf(stderr, "usage: sprprobe <file.spr> [file.spr ...]\n");
        return 1;
    }

    for (i = 1; i < argc; i++) {
        probeFile(argv[i]);
    }

    return 0;
}

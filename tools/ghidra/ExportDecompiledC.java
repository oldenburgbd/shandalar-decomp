/* Export decompiled C for a list of function addresses.
 * args: <addr-list-file> <output-dir>
 * addr-list-file: one hex VA per line (0x-prefixed or bare).
 * Writes <outdir>/<va-hex>.c per function that decompiles.
 */
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionManager;

import java.io.File;
import java.io.FileWriter;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.List;

public class ExportDecompiledC extends GhidraScript {
    @Override
    protected void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 2) {
            printerr("usage: ExportDecompiledC <addr-list-file> <outdir>");
            return;
        }
        List<String> lines = Files.readAllLines(Paths.get(args[0]));
        File outdir = new File(args[1]);
        outdir.mkdirs();

        DecompInterface ifc = new DecompInterface();
        ifc.openProgram(currentProgram);
        FunctionManager fm = currentProgram.getFunctionManager();

        int done = 0, failed = 0;
        for (String line : lines) {
            line = line.trim();
            if (line.isEmpty()) continue;
            long va = Long.parseLong(line.replace("0x", ""), 16);
            Address addr = currentProgram.getAddressFactory()
                    .getDefaultAddressSpace().getAddress(va);
            Function fn = fm.getFunctionAt(addr);
            if (fn == null) fn = fm.getFunctionContaining(addr);
            if (fn == null) { println(String.format("NOFUNC %x", va)); failed++; continue; }
            DecompileResults res = ifc.decompileFunction(fn, 120, monitor);
            if (res == null || !res.decompileCompleted()) {
                println(String.format("FAIL %x", va)); failed++; continue;
            }
            File out = new File(outdir, String.format("%08x.c", va));
            FileWriter w = new FileWriter(out);
            w.write(String.format("/* %s @ 0x%08x size=%d */%n",
                    fn.getName(), va, fn.getBody().getNumAddresses()));
            w.write(res.getDecompiledFunction().getC());
            w.close();
            done++;
        }
        ifc.dispose();
        println(String.format("ExportDecompiledC: %d written, %d failed", done, failed));
    }
}

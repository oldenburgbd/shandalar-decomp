//Exports a per-function inventory (address, name, size, complexity, callers/callees)
//plus a program summary, as CSV/JSON for scope estimation and progress tracking.
//@category MTG-RE
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.block.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.*;
import java.io.*;
import java.util.*;

public class ExportFunctionInventory extends GhidraScript {

    @Override
    public void run() throws Exception {
        String outDir = System.getProperty("mtgre.outdir");
        if (outDir == null) {
            String[] a = getScriptArgs();
            outDir = (a.length > 0) ? a[0] : ".";
        }
        new File(outDir).mkdirs();
        String base = currentProgram.getName().replaceAll("[^A-Za-z0-9_.-]", "_");

        Listing listing = currentProgram.getListing();
        FunctionManager fm = currentProgram.getFunctionManager();
        BasicBlockModel bbm = new BasicBlockModel(currentProgram);

        // ---- function inventory ----
        File csv = new File(outDir, base + ".functions.csv");
        PrintWriter w = new PrintWriter(new BufferedWriter(new FileWriter(csv)));
        w.println("address,name,size,is_thunk,is_external,namespace,num_blocks,num_callers,num_callees,"
                + "num_params,has_varargs,calling_convention,stack_frame_size,section");

        int total = 0, thunks = 0;
        long totalBytes = 0;
        Map<String, Integer> perSection = new TreeMap<String, Integer>();
        Map<String, Long> perSectionBytes = new TreeMap<String, Long>();
        List<long[]> sizes = new ArrayList<long[]>();

        FunctionIterator it = fm.getFunctions(true);
        while (it.hasNext()) {
            if (monitor.isCancelled()) break;
            Function f = it.next();
            Address ep = f.getEntryPoint();
            long size = f.getBody().getNumAddresses();
            MemoryBlock mb = currentProgram.getMemory().getBlock(ep);
            String sect = (mb == null) ? "?" : mb.getName();

            int nblocks = 0;
            try {
                CodeBlockIterator cbi = bbm.getCodeBlocksContaining(f.getBody(), monitor);
                while (cbi.hasNext()) { cbi.next(); nblocks++; }
            } catch (Exception e) { nblocks = -1; }

            int ncallers = f.getCallingFunctions(monitor).size();
            int ncallees = f.getCalledFunctions(monitor).size();

            w.printf("%s,%s,%d,%b,%b,%s,%d,%d,%d,%d,%b,%s,%d,%s%n",
                    ep.toString(),
                    csvEsc(f.getName()),
                    size,
                    f.isThunk(),
                    f.isExternal(),
                    csvEsc(f.getParentNamespace().getName()),
                    nblocks, ncallers, ncallees,
                    f.getParameterCount(),
                    f.hasVarArgs(),
                    csvEsc(String.valueOf(f.getCallingConventionName())),
                    f.getStackFrame().getFrameSize(),
                    csvEsc(sect));

            total++;
            if (f.isThunk()) thunks++;
            totalBytes += size;
            sizes.add(new long[]{size});
            perSection.put(sect, perSection.containsKey(sect) ? perSection.get(sect) + 1 : 1);
            perSectionBytes.put(sect,
                (perSectionBytes.containsKey(sect) ? perSectionBytes.get(sect) : 0L) + size);
        }
        w.close();

        // ---- coverage: how much of each executable block is inside a function ----
        StringBuilder sections = new StringBuilder();
        long execTotal = 0, execCovered = 0;
        for (MemoryBlock mb : currentProgram.getMemory().getBlocks()) {
            long len = mb.getSize();
            long covered = 0;
            if (mb.isExecute()) {
                execTotal += len;
                Address a = mb.getStart();
                while (a != null && a.compareTo(mb.getEnd()) <= 0) {
                    Function f = fm.getFunctionContaining(a);
                    if (f != null) {
                        long fsz = f.getBody().getNumAddresses();
                        covered += fsz;
                        Address next = f.getBody().getMaxAddress();
                        a = (next == null) ? null : next.next();
                    } else {
                        a = a.next();
                    }
                    if (monitor.isCancelled()) break;
                }
                execCovered += covered;
            }
            if (sections.length() > 0) sections.append(",");
            sections.append(String.format(
                "{\"name\":\"%s\",\"start\":\"%s\",\"size\":%d,\"read\":%b,\"write\":%b,\"exec\":%b,\"covered\":%d}",
                mb.getName(), mb.getStart().toString(), len, mb.isRead(), mb.isWrite(), mb.isExecute(), covered));
        }

        // ---- undefined/uncategorised bytes in exec space ----
        long instrCount = 0;
        InstructionIterator ii = listing.getInstructions(true);
        while (ii.hasNext()) { ii.next(); instrCount++; if (monitor.isCancelled()) break; }

        long dataCount = 0, definedData = 0;
        DataIterator di = listing.getDefinedData(true);
        while (di.hasNext()) { Data d = di.next(); definedData += d.getLength(); dataCount++;
            if (monitor.isCancelled()) break; }

        // ---- size histogram ----
        Collections.sort(sizes, new Comparator<long[]>() {
            public int compare(long[] a, long[] b) { return Long.compare(a[0], b[0]); }
        });
        long median = sizes.isEmpty() ? 0 : sizes.get(sizes.size() / 2)[0];
        long p90 = sizes.isEmpty() ? 0 : sizes.get((int) (sizes.size() * 0.90))[0];
        long p99 = sizes.isEmpty() ? 0 : sizes.get((int) (sizes.size() * 0.99))[0];
        long max = sizes.isEmpty() ? 0 : sizes.get(sizes.size() - 1)[0];

        StringBuilder ps = new StringBuilder();
        for (Map.Entry<String, Integer> e : perSection.entrySet()) {
            if (ps.length() > 0) ps.append(",");
            ps.append(String.format("\"%s\":{\"functions\":%d,\"bytes\":%d}",
                e.getKey(), e.getValue(), perSectionBytes.get(e.getKey())));
        }

        File js = new File(outDir, base + ".summary.json");
        PrintWriter jw = new PrintWriter(new BufferedWriter(new FileWriter(js)));
        jw.printf("{%n");
        jw.printf("  \"program\": \"%s\",%n", currentProgram.getName());
        jw.printf("  \"executable_md5\": \"%s\",%n", currentProgram.getExecutableMD5());
        jw.printf("  \"executable_sha256\": \"%s\",%n", currentProgram.getExecutableSHA256());
        jw.printf("  \"language\": \"%s\",%n", currentProgram.getLanguageID().getIdAsString());
        jw.printf("  \"compiler_spec\": \"%s\",%n", currentProgram.getCompilerSpec().getCompilerSpecID().getIdAsString());
        jw.printf("  \"image_base\": \"%s\",%n", currentProgram.getImageBase().toString());
        jw.printf("  \"total_functions\": %d,%n", total);
        jw.printf("  \"thunk_functions\": %d,%n", thunks);
        jw.printf("  \"real_functions\": %d,%n", total - thunks);
        jw.printf("  \"total_function_bytes\": %d,%n", totalBytes);
        jw.printf("  \"instruction_count\": %d,%n", instrCount);
        jw.printf("  \"defined_data_items\": %d,%n", dataCount);
        jw.printf("  \"defined_data_bytes\": %d,%n", definedData);
        jw.printf("  \"exec_bytes_total\": %d,%n", execTotal);
        jw.printf("  \"exec_bytes_in_functions\": %d,%n", execCovered);
        jw.printf("  \"function_size\": {\"median\":%d,\"p90\":%d,\"p99\":%d,\"max\":%d},%n",
                  median, p90, p99, max);
        jw.printf("  \"per_section\": {%s},%n", ps.toString());
        jw.printf("  \"sections\": [%s]%n", sections.toString());
        jw.printf("}%n");
        jw.close();

        println("MTGRE: wrote " + csv.getAbsolutePath());
        println("MTGRE: wrote " + js.getAbsolutePath());
        println("MTGRE: functions=" + total + " (thunks=" + thunks + ") bytes=" + totalBytes
                + " execTotal=" + execTotal + " execCovered=" + execCovered);
    }

    private static String csvEsc(String s) {
        if (s == null) return "";
        if (s.indexOf(',') >= 0 || s.indexOf('"') >= 0)
            return "\"" + s.replace("\"", "\"\"") + "\"";
        return s;
    }
}

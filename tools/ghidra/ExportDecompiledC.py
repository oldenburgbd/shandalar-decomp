# Export decompiled C for a list of function addresses.
# Ghidra Jython post-script.
#   args: <addr-list-file> <output-dir>
# addr-list-file: one hex VA per line (0x-prefixed or bare).
# Writes <outdir>/<addr>.c, one file per function that decompiles.

import os
from ghidra.app.decompiler import DecompInterface
from ghidra.util.task import ConsoleTaskMonitor

args = getScriptArgs()
addr_file, outdir = args[0], args[1]
if not os.path.isdir(outdir):
    os.makedirs(outdir)

ifc = DecompInterface()
ifc.openProgram(currentProgram)
monitor = ConsoleTaskMonitor()
fm = currentProgram.getFunctionManager()
space = currentProgram.getAddressFactory().getDefaultAddressSpace()

done = failed = 0
for line in open(addr_file):
    line = line.strip()
    if not line:
        continue
    va = int(line, 16)
    addr = space.getAddress(va)
    fn = fm.getFunctionAt(addr)
    if fn is None:
        fn = fm.getFunctionContaining(addr)
    if fn is None:
        print("NOFUNC %x" % va)
        failed += 1
        continue
    res = ifc.decompileFunction(fn, 120, monitor)
    if not res.decompileCompleted():
        print("FAIL %x %s" % (va, res.getErrorMessage()))
        failed += 1
        continue
    out = os.path.join(outdir, "%08x.c" % va)
    f = open(out, "w")
    f.write("/* %s @ 0x%08x size=%d */\n" % (fn.getName(), va, fn.getBody().getNumAddresses()))
    f.write(res.getDecompiledFunction().getC())
    f.close()
    done += 1
print("ExportDecompiledC: %d written, %d failed -> %s" % (done, failed, outdir))

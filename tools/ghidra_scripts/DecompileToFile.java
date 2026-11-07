// Headless helper: writes Ghidra's pseudo-C for the given functions to a file.
// Args: <output file> <function name or hex address>...
// Used by scripts/ghidra_decomp.py (see docs/GHIDRA.md).
//@category dsd

import java.io.PrintWriter;

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.Symbol;

public class DecompileToFile extends GhidraScript {
    @Override
    protected void run() throws Exception {
        String[] args = getScriptArgs();
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);
        try (PrintWriter out = new PrintWriter(args[0], "UTF-8")) {
            for (int i = 1; i < args.length; i++) {
                Function f = lookup(args[i]);
                if (f == null) {
                    out.println("// " + args[i] + ": not found\n");
                    continue;
                }
                DecompileResults res = decomp.decompileFunction(f, 60, monitor);
                out.println("// " + f.getName() + " @ " + f.getEntryPoint());
                if (res.decompileCompleted()) {
                    out.println(res.getDecompiledFunction().getC());
                } else {
                    out.println("// decompile failed: " + res.getErrorMessage() + "\n");
                }
            }
        }
        decomp.dispose();
    }

    private Function lookup(String key) {
        for (Symbol s : currentProgram.getSymbolTable().getSymbols(key)) {
            Function f = getFunctionAt(s.getAddress());
            if (f != null) {
                return f;
            }
        }
        try {
            Address a = toAddr(Long.parseLong(key.replaceFirst("^0x", ""), 16));
            Function f = getFunctionAt(a);
            return f != null ? f : getFunctionContaining(a);
        } catch (NumberFormatException e) {
            return null;
        }
    }
}

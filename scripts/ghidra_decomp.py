"""Print Ghidra's pseudo-C for functions, using a headless copy of the Ghidra project.

Usage:  python scripts/ghidra_decomp.py <function name or hex address> [...]

Needs Ghidra 11.2.1 + dsd-ghidra (docs/GHIDRA.md) and a project copy at GHIDRA_PROJECT
(default C:/Tools/ghidra_projects/claude/si) that already contains the synced ROM.
The pseudo-C is only a starting point: match.py decides whether the C is right.
"""
import os
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
GHIDRA = Path(os.environ.get("GHIDRA_HOME", "C:/Tools/ghidra_11.2.1_PUBLIC"))
PROJECT = Path(os.environ.get("GHIDRA_PROJECT", "C:/Tools/ghidra_projects/claude/si"))


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    with tempfile.TemporaryDirectory() as tmp:
        out = Path(tmp) / "out.c"
        cmd = [str(GHIDRA / "support" / "analyzeHeadless.bat"), str(PROJECT.parent), PROJECT.name,
               "-process", "sunshine_islands_usa.nds", "-readOnly", "-noanalysis",
               "-scriptPath", str(ROOT / "tools" / "ghidra_scripts"),
               "-postScript", "DecompileToFile.java", str(out), *sys.argv[1:]]
        env = dict(os.environ)
        if "JAVA_HOME" not in env:
            jdks = sorted(Path("C:/Program Files/Eclipse Adoptium").glob("jdk-21*"))
            if jdks:
                env["JAVA_HOME"] = str(jdks[-1])
                env["PATH"] = str(jdks[-1] / "bin") + os.pathsep + env["PATH"]
        r = subprocess.run(cmd, capture_output=True, text=True, stdin=subprocess.DEVNULL, env=env)
        if not out.exists():
            print(r.stdout[-3000:], r.stderr[-2000:])
            sys.exit("ghidra failed")
        print(out.read_text(encoding="utf-8"))


if __name__ == "__main__":
    main()

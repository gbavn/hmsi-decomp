"""Generate the objdiff progress report (build/usa/report.json) after `python scripts/build.py`.

Writes objdiff.json with dsd, points alias units (<file>_itcm/_dtcm/_rodata/_version, which build.py
links from sections of <file>.o) at the real object, then runs objdiff-cli.

Usage:  python scripts/report.py [--objdiff PATH_TO_OBJDIFF_CLI]
"""
import argparse
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
DSD = ROOT / "tools" / "dsd.exe"
CONFIG = ROOT / "config" / "usa" / "arm9" / "config.yaml"
OUT = ROOT / "build" / "usa" / "report.json"
ALIAS = re.compile(r"_(itcm|dtcm|rodata|version)\.o$")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--objdiff", default=str(ROOT / "tools" / "objdiff" / "objdiff-cli.exe"))
    a = ap.parse_args()

    subprocess.run([str(DSD), "objdiff", "-c", str(CONFIG), "-o", str(ROOT)], cwd=ROOT, check=True)
    path = ROOT / "objdiff.json"
    project = json.loads(path.read_text())
    for unit in project["units"]:
        base = unit.get("base_path")
        if base and not (ROOT / base).exists():
            real = ALIAS.sub(".o", base)
            if real != base and (ROOT / real).exists():
                unit["base_path"] = real
            else:
                sys.exit(f"missing object {base} (run scripts/build.py first)")
    path.write_text(json.dumps(project, indent=2))

    OUT.parent.mkdir(parents=True, exist_ok=True)
    subprocess.run([a.objdiff, "report", "generate", "-p", str(ROOT), "-o", str(OUT)], cwd=ROOT, check=True)
    measures = json.loads(OUT.read_text())["measures"]
    print(f"code {measures.get('matched_code_percent', 0):.2f}%  "
          f"functions {measures.get('matched_functions', 0)}/{measures.get('total_functions', 0)}  -> {OUT}")


if __name__ == "__main__":
    main()

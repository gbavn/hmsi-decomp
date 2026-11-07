"""Generate nitro/fx/fx_const.h from the NitroSDK fx_const.csv table.

Usage:  python scripts/gen_fx_const.py <fx_const.csv> <out fx_const.h>
"""
import csv
import math
import sys

FRAC = {"fx32": 12, "fx64": 12, "fx64c": 32, "fx16": 12}
SUFFIX = {"fx32": "L", "fx64": "LL", "fx64c": "LL", "fx16": ""}


def encode(value, frac):
    if value == 0:
        return 0
    a = abs(value)
    whole = math.floor(a)
    int_bits = int(a * (1 << frac)) & ~((1 << frac) - 1)
    frac_bits = int((a - whole) * (1 << frac) + 0.5) & ((1 << frac) - 1)
    v = int_bits + frac_bits
    return -v if value < 0 else v


def main(src, dst):
    out = ["#ifndef FX_CONST_H", "#define FX_CONST_H", ""]
    with open(src, newline="") as f:
        for row in csv.reader(f):
            if not row or row[0].startswith("#"):
                continue
            name, typ, value = row[0], row[1], float(row[2])
            enc = encode(value, FRAC[typ])
            out.append(f"#define {typ.upper()}_{name} (({typ}) 0x{enc & 0xFFFFFFFFFFFFFFFF:016x}{SUFFIX[typ]})")
    out += ["", "#endif // FX_CONST_H", ""]
    with open(dst, "w") as f:
        f.write("\n".join(out))


if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2])

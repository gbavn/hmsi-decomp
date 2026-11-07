"""Add a library source file (e.g. from libs/) as a complete unit, automatically.

  1. compile the file and find every compiled function in the module by bytes
  2. derive the unit's .text range and the addresses of its .rodata/.data/.bss
     (from where the code's relocations point in the original)
  3. rename the symbols in symbols.txt and append the unit to delinks.txt

Usage:  python scripts/addlib.py <source> [--module main] [--dry-run]
Afterwards run scripts/commit.sh (it builds and verifies the ROM before committing).
"""
import argparse
import re
import struct
import subprocess
import sys
import tempfile
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import match  # noqa: E402

DATA_SECTIONS = (".rodata", ".data", ".bss", ".sbss", ".sdata", ".init", ".ctor")


def read_obj(path):
    b = path.read_bytes()
    shoff, = struct.unpack_from("<I", b, 0x20)
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", b, 0x2E)
    secs = []
    for i in range(shnum):
        name, typ, flags, addr, off, size, link, info, align, entsize = struct.unpack_from("<10I", b, shoff + i * shentsize)
        secs.append(dict(name=name, type=typ, off=off, size=size, link=link, info=info, align=align))

    def cstr(off):
        return b[off:b.index(b"\0", off)].decode()

    shstr = secs[shstrndx]["off"]
    for s in secs:
        s["name"] = cstr(shstr + s["name"])
    symtab = next(s for s in secs if s["type"] == 2)
    strtab = secs[symtab["link"]]["off"]
    syms = []
    for i in range(symtab["size"] // 16):
        name, value, size, info, other, shndx = struct.unpack_from("<IIIBBH", b, symtab["off"] + i * 16)
        syms.append(dict(name=cstr(strtab + name), value=value, size=size, bind=info >> 4, type=info & 0xF,
                         shndx=shndx))
    relocs = defaultdict(list)  # target section -> [(offset, sym index, type, addend)]
    for s in secs:
        if s["type"] == 4:
            for i in range(s["size"] // 12):
                off, info, addend = struct.unpack_from("<IIi", b, s["off"] + i * 12)
                relocs[s["info"]].append((off, info >> 8, info & 0xFF, addend))
        elif s["type"] == 9:
            tsec = secs[s["info"]]
            for i in range(s["size"] // 8):
                off, info = struct.unpack_from("<II", b, s["off"] + i * 8)
                addend, = struct.unpack_from("<i", b, tsec["off"] + off) if (info & 0xFF) == 2 else (0,)
                relocs[s["info"]].append((off, info >> 8, info & 0xFF, addend))
    return b, secs, syms, relocs


def load_module(module):
    syms_file, relocs_file, binary, base = match.module_info(module)
    funcs = []
    for line in syms_file.read_text().splitlines():
        m = match.SYM_RE.match(line)
        if m:
            funcs.append((int(m.group(4), 16), int(m.group(3), 16), m.group(1)))
    funcs.sort()
    orig_relocs = {}
    for line in relocs_file.read_text().splitlines():
        m = re.match(r"from:0x([0-9a-f]+) kind:(\S+) to:0x([0-9a-f]+)(?: add:(-?0x[0-9a-f]+))?", line)
        if m:
            add = int(m.group(4), 16) if m.group(4) else 0
            orig_relocs[int(m.group(1), 16)] = (m.group(2), int(m.group(3), 16) + add)
    return syms_file, funcs, orig_relocs, binary.read_bytes(), base


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("source")
    ap.add_argument("--module", default="main")
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--force", action="store_true", help="accept a lone tiny function match")
    a = ap.parse_args()

    syms_file, ofuncs, orelocs, obin, obase = load_module(a.module)
    # code already covered by complete units is off limits
    covered = []
    _, _, files = syms_file.with_name("delinks.txt").read_text().partition("\n\n")
    for m in re.finditer(r"\.text\s+start:0x([0-9a-f]+) end:0x([0-9a-f]+)", files):
        covered.append((int(m.group(1), 16), int(m.group(2), 16)))
    by_size = defaultdict(list)
    for addr, size, name in ofuncs:
        if not any(s <= addr < e for s, e in covered):
            by_size[size].append((addr, name))

    with tempfile.TemporaryDirectory() as tmp:
        obj = Path(tmp) / "out.o"
        match.compile_src(match.ROOT / a.source, match.DEFAULT_COMPILER, None, obj)
        b, secs, syms, relocs = read_obj(obj)

    # ---- 1. match functions ------------------------------------------------------------------------
    cands = {}
    for idx, s in enumerate(syms):
        if s["type"] != 2 or not s["size"] or not (0 < s["shndx"] < len(secs)):
            continue
        sec = secs[s["shndx"]]
        if not sec["name"].startswith(".text"):
            continue
        start = s["value"] & ~1
        # a function alone in its section owns the trailing literal pool too
        nxt = min([o["value"] & ~1 for o in syms if o["shndx"] == s["shndx"] and o["type"] == 2
                   and (o["value"] & ~1) > start] + [sec["size"]])
        size = nxt - start if nxt - start > s["size"] else s["size"]
        data = bytearray(b[sec["off"] + start: sec["off"] + start + size])
        cmask = set()
        for off, *_ in relocs.get(s["shndx"], ()):
            if start <= off < start + size:
                cmask.update(range(off - start, off - start + 4))
        hits = []
        sym_size = s["size"]
        cands_sz = list(by_size.get(len(data), ()))
        if len(data) != sym_size:
            cands_sz += list(by_size.get(sym_size, ()))  # dsd may exclude the literal pool
        for addr, name in cands_sz:
            orig = obin[addr - obase: addr - obase + len(data)]
            omask = set()
            for i in range(0, len(data), 4):
                if addr + i in orelocs:
                    omask.update(range(i, i + 4))
            if not match.compare(orig, omask, data, cmask):
                hits.append((addr, name))
        if hits:
            cands[idx] = hits
    if not cands:
        sys.exit("no functions of this file were found in the module")

    # Resolve ambiguous candidates (byte-identical functions that only differ in relocations).
    # The linker keeps the source order, so compiled functions (in section order) must map to
    # increasing addresses. Anchor on the unambiguous matches near the median.
    names_at = {}
    for line in syms_file.read_text().splitlines():
        m = re.match(r"^(\S+) kind:\S+ addr:0x([0-9a-f]+)", line)
        if m:
            names_at.setdefault(int(m.group(2), 16), m.group(1))

    def consistency(idx, addr):
        """+1 per relocation whose original target already carries the expected name, -1 per conflict."""
        s = syms[idx]
        start = s["value"] & ~1
        score = 0
        here = names_at.get(addr)
        if here == re.sub(r"\$.*", "", s["name"]):
            score += 3  # an earlier unit already named this address the same way
        elif here and not re.match(r"(func|data)_", here):
            score -= 3
        for off, symi, rtype, addend in relocs.get(s["shndx"], ()):
            if not (start <= off < start + s["size"]):
                continue
            want = syms[symi]["name"]
            o = orelocs.get(addr + off - start)
            if o is None:
                continue
            # calls into this file's own functions: check against their unique match, if any
            if symi in cands and len(cands[symi]) == 1:
                target = ((o[1] - addend) if rtype == 2 else o[1]) & ~1
                score += 2 if cands[symi][0][0] == target else -2
                continue
            if not want or syms[symi]["shndx"] != 0:
                continue  # only external symbols say anything about the pairing
            have = names_at.get(((o[1] - addend) if rtype == 2 else o[1]) & ~1)
            if have == want:
                score += 1
            elif have and not re.match(r"(func|data)_", have):
                score -= 1
        return score

    unique = [h[0][0] for h in cands.values() if len(h) == 1]
    center = sorted(unique)[len(unique) // 2] if unique else min(h[0][0] for h in cands.values())
    order = sorted(cands, key=lambda i: syms[i]["shndx"])
    chosen = {}
    used = set()
    prev = 0
    for idx in order:
        hits = [h for h in cands[idx] if h[0] not in used]
        if hits:
            best_score = max(consistency(idx, h[0]) for h in hits)
            hits = [h for h in hits if consistency(idx, h[0]) == best_score]
            if best_score < 0:
                continue
        if not hits:
            continue
        near = [h for h in hits if abs(h[0] - center) < 0x10000] or hits
        after = [h for h in near if h[0] > prev]
        addr, name = min(after, key=lambda h: h[0]) if after else min(near, key=lambda h: abs(h[0] - center))
        chosen[idx] = (addr, name)
        used.add(addr)
        prev = addr

    lo = min(addr for addr, _ in chosen.values())
    hi = max(addr + syms[idx]["size"] for idx, (addr, _) in chosen.items())
    # drop outliers: keep the largest contiguous run of matched functions
    in_range = [(addr, size, name) for addr, size, name in ofuncs if lo <= addr < hi]
    matched_addrs = {addr for addr, _ in chosen.values()}
    runs, cur = [], []
    for f in in_range:
        if f[0] in matched_addrs:
            cur.append(f)
        else:
            if cur:
                runs.append(cur)
            cur = []
    if cur:
        runs.append(cur)
    best = max(runs, key=len)
    if len(runs) > 1:
        print(f"warning: matched functions are not contiguous; using the run {best[0][0]:#x}..{best[-1][0]:#x}")
        for r in runs:
            if r is not best:
                print("  skipped run:", ", ".join(f"{x[2]}@{x[0]:#x}" for x in r))
    text_start, text_end = best[0][0], best[-1][0] + best[-1][1]
    if text_end - text_start <= 0x20 and len(best) == 1 and not a.force:
        sys.exit(f"only one tiny function matched ({best[0][2]}@{best[0][0]:#x}); likely a coincidence (use --force)")
    keep = {idx: v for idx, v in chosen.items() if text_start <= v[0] < text_end}

    # ---- 2. data sections (per section index; same-named sections are merged if contiguous) ---------
    bases = defaultdict(set)
    for idx, (addr, _) in keep.items():
        s = syms[idx]
        start = s["value"] & ~1
        for off, symi, rtype, addend in relocs.get(s["shndx"], ()):
            if not (start <= off < start + s["size"]):
                continue
            tgt = syms[symi]
            if not (0 < tgt["shndx"] < len(secs)):
                continue
            if secs[tgt["shndx"]]["name"] not in DATA_SECTIONS:
                continue
            o = orelocs.get(addr + off - start)
            if o is None:
                continue
            bases[tgt["shndx"]].add(o[1] - (tgt["value"] + addend))
    # fallback: global data symbols whose names are already known in symbols.txt
    name_addr = {}
    for line in syms_file.read_text().splitlines():
        m = re.match(r"^(\S+) kind:(?:data|bss)\S* addr:0x([0-9a-f]+)", line)
        if m:
            name_addr[m.group(1)] = int(m.group(2), 16)
    for s in syms:
        if s["bind"] == 1 and 0 < s["shndx"] < len(secs) and s["name"] in name_addr:
            if secs[s["shndx"]]["name"] in DATA_SECTIONS and not bases[s["shndx"]]:
                bases[s["shndx"]].add(name_addr[s["name"]] - s["value"])
    sections = [(".text", text_start, text_end)]
    sec_addr = {}
    for name in DATA_SECTIONS:
        parts = [(i, sec) for i, sec in enumerate(secs) if sec["name"] == name and sec["type"] in (1, 8) and sec["size"]]
        if not parts:
            continue
        placed = []
        unplaced = []
        for i, sec in parts:
            if len(bases[i]) == 1:
                b0 = next(iter(bases[i]))
                placed.append((b0, b0 + sec["size"]))
                sec_addr[i] = b0
            else:
                unplaced.append((i, sec))
        if not placed:
            print(f"warning: could not place {name} (size {sum(p['size'] for _, p in parts):#x})")
            continue
        placed.sort()
        # sections nobody references are assumed to directly follow the placed ones (aligned)
        for i, sec in unplaced:
            align = max(sec["align"], 1)
            b0 = (placed[-1][1] + align - 1) // align * align
            print(f"warning: {name} section {i} (size {sec['size']:#x}) unreferenced; assuming {b0:#x}")
            placed.append((b0, b0 + sec["size"]))
            sec_addr[i] = b0
        lo, hi = placed[0][0], placed[-1][1]
        gaps = [(a[1], b[0]) for a, b in zip(placed, placed[1:]) if b[0] - a[1] >= 0x20 or b[0] < a[1]]
        if gaps:
            print(f"warning: {name} pieces are not contiguous: {[(hex(a), hex(b)) for a, b in placed]}")
            continue
        if hi % 4:
            # swallow alignment padding after the data, otherwise dsd makes it a tiny gap that
            # the linker re-aligns (shifting everything behind it)
            pad = -hi % 4
            if name in (".bss", ".sbss") or not any(obin[hi - obase: hi - obase + pad]):
                hi += pad
        sections.append((name, lo, hi))

    # ---- 3. report / apply ---------------------------------------------------------------------------
    renames, local = [], []
    for idx, (addr, name) in sorted(keep.items(), key=lambda kv: kv[1][0]):
        new = re.sub(r"\$.*", "", syms[idx]["name"])
        print(f"  {addr:#010x} {name:28} -> {new}{' (static)' if syms[idx]['bind'] == 0 else ''}")
        if new != name:
            renames += [name, new]
        if syms[idx]["bind"] == 0:
            local.append(new)
    # external symbols this unit references: give the original targets those names
    known_all = {}
    for line in syms_file.read_text().splitlines():
        m = re.match(r"^(\S+) kind:(\S+) addr:0x([0-9a-f]+)", line)
        if m:
            known_all.setdefault(int(m.group(3), 16), m.group(1))
    ext = {}
    for idx, (addr, _) in keep.items():
        s = syms[idx]
        start = s["value"] & ~1
        for off, symi, rtype, addend in relocs.get(s["shndx"], ()):
            if not (start <= off < start + s["size"]):
                continue
            tgt = syms[symi]
            if tgt["shndx"] != 0 or not tgt["name"]:
                continue
            o = orelocs.get(addr + off - start)
            if o is None:
                continue
            taddr = ((o[1] - addend) if rtype == 2 else o[1]) & ~1
            old = known_all.get(taddr)
            if old is None:
                print(f"warning: no symbol at {taddr:#x} for external {tgt['name']}")
            elif old != tgt["name"]:
                if not re.match(r"(func|data)_", old):
                    print(f"warning: {taddr:#x} is named {old}, unit expects {tgt['name']}")
                else:
                    ext[old] = tgt["name"]
    # relocations inside the unit's own data (e.g. tables of function pointers)
    placed = {n: s for n, s, e in sections if n != ".text"}
    for si, sec in enumerate(secs):
        if sec["name"] not in placed or sec["type"] != 1:
            continue
        for off, symi, rtype, addend in relocs.get(si, ()):
            tgt = syms[symi]
            if tgt["shndx"] != 0 or not tgt["name"]:
                continue
            o = orelocs.get(placed[sec["name"]] + off)
            if o is None:
                continue
            taddr = ((o[1] - addend) if rtype == 2 else o[1]) & ~1
            old = known_all.get(taddr)
            if old and old != tgt["name"] and re.match(r"(func|data)_", old):
                ext[old] = tgt["name"]
    for old, new in ext.items():
        renames += [old, new]

    # Original symbols inside our data that don't start a compiled object (dsd named every address
    # something referenced): references to them become "containing object + offset".
    interior = []  # (old name, old addr, new target name, new target addr)
    fixed = []  # symbols inside static objects: keep them as absolute linker symbols
    starts = {}
    for s in syms:
        if s["type"] == 1 and s["size"] and s["shndx"] in sec_addr:
            starts[sec_addr[s["shndx"]] + s["value"]] = (re.sub(r"\$.*", "", s["name"]), s["size"], s["bind"])
    ranges = [(s0, e0) for n, s0, e0 in sections if n != ".text"]
    for line in syms_file.read_text().splitlines():
        m = re.match(r"^(\S+) kind:(?:data|bss)\S* addr:0x([0-9a-f]+)", line)
        if not m:
            continue
        addr = int(m.group(2), 16)
        if addr in starts or not any(s0 <= addr < e0 for s0, e0 in ranges):
            continue
        cont = [(a0, v) for a0, v in starts.items() if a0 < addr < a0 + v[1] and v[2] == 1]
        if cont:
            a0, v = cont[0]
            interior.append((m.group(1), addr, v[0], a0))
        else:
            fixed.append((m.group(1), addr))

    # global data symbols
    sec_base = {n: s for n, s, e in sections}
    known = {}
    for line in syms_file.read_text().splitlines():
        m = re.match(r"^(\S+) kind:(\S+) addr:0x([0-9a-f]+)", line)
        if m:
            known.setdefault(int(m.group(3), 16), m.group(1))
    for s in syms:
        if s["type"] != 1 or not (0 < s["shndx"] < len(secs)) or s["bind"] != 1:
            continue
        if s["shndx"] in sec_addr:
            addr = sec_addr[s["shndx"]] + s["value"]
            if addr in known and known[addr] != s["name"]:
                renames += [known[addr], s["name"]]
    print(f"unit: {a.source}")
    for n, s, e in sections:
        print(f"    {n:11} start:{s:#010x} end:{e:#010x}")
    print(f"functions: {len(keep)}")
    if a.dry_run:
        return
    if renames:
        cmd = [sys.executable, str(Path(__file__).parent / "rename.py"), *renames]
        if local:
            cmd += ["--local", *local]
        subprocess.run(cmd, check=True)
    if interior:
        drop = {n for n, *_ in interior}
        lines = [l for l in syms_file.read_text().splitlines() if l.split(" ", 1)[0] not in drop]
        syms_file.write_text("\n".join(lines) + "\n")
        for relocs_file in match.CONFIG.rglob("relocs.txt"):  # overlays may point into main's data too
            text = old = relocs_file.read_text()
            for n, addr, tn, a0 in interior:
                text = text.replace(f"to:{addr:#010x} module:{a.module}",
                                    f"to:{a0:#010x} add:{addr - a0:#x} module:{a.module}")
            if text != old:
                relocs_file.write_text(text)
        print(f"rewrote references to {len(interior)} interior symbols")
    if fixed:
        referenced = set()
        for relocs_file in match.CONFIG.rglob("relocs.txt"):
            for line in relocs_file.read_text().splitlines():
                mm = re.search(r"to:0x([0-9a-f]+) module:" + a.module + r"$", line)
                if mm:
                    referenced.add(int(mm.group(1), 16))
        consts = syms_file.with_name("sdk_symbols.txt")
        have = consts.read_text() if consts.exists() else ""
        new = [f"{n} = {addr:#010x}\n" for n, addr in fixed if addr in referenced and f"{n} =" not in have]
        if new:
            with consts.open("a") as f:
                f.writelines(new)
            print(f"defined {len(new)} referenced symbols inside static data as absolute linker symbols")
        unused = {n for n, addr in fixed if addr not in referenced}
        if unused:
            kept = [l for l in syms_file.read_text().splitlines() if l.split(" ", 1)[0] not in unused]
            syms_file.write_text("\n".join(kept) + "\n")
    delinks = syms_file.with_name("delinks.txt")
    entry = f"\n{a.source}:\n    complete\n" + "".join(
        f"    {n:11} start:{s:#010x} end:{e:#010x}\n" for n, s, e in sections)
    with delinks.open("a") as f:
        f.write(entry)
    print("added to", delinks.relative_to(match.ROOT))
    add_itcm(a, b, secs, syms, relocs)


def add_itcm(a, b, secs, syms, relocs):
    """Functions the file puts in the .itcm section live in the ITCM module: add a unit there too."""
    funcs = [s for s in syms if s["type"] == 2 and s["size"] and 0 < s["shndx"] < len(secs)
             and secs[s["shndx"]]["name"] == ".itcm"]
    if not funcs:
        return
    syms_file, ofuncs, orelocs, obin, obase = load_module("itcm")
    found = {}
    for s in funcs:
        sec = secs[s["shndx"]]
        start = s["value"] & ~1
        data = bytearray(b[sec["off"] + start: sec["off"] + start + s["size"]])
        cmask = set()
        for off, *_ in relocs.get(s["shndx"], ()):
            if start <= off < start + s["size"]:
                cmask.update(range(off - start, off - start + 4))
        for addr, size, name in ofuncs:
            if size != len(data):
                continue
            orig = obin[addr - obase: addr - obase + size]
            omask = {i for i in range(size) if (addr + i - i % 4) in orelocs}
            if not match.compare(orig, omask, data, cmask) and addr not in found.values():
                found[s["name"]] = addr
                break
    if len(found) != len(funcs):
        print(f"itcm: only {len(found)}/{len(funcs)} functions found; not adding an ITCM unit")
        return
    lo = min(found.values())
    hi = max(addr + next(f["size"] for f in funcs if f["name"] == n) for n, addr in found.items())
    inside = [f for f in ofuncs if lo <= f[0] < hi]
    if len(inside) != len(found):
        print("itcm: matched functions are not contiguous; not adding an ITCM unit")
        return
    renames = []
    for n, addr in found.items():
        old = next(f[2] for f in ofuncs if f[0] == addr)
        if old != n:
            renames += [old, n]
    if renames:
        subprocess.run([sys.executable, str(Path(__file__).parent / "rename.py"), *renames], check=True)
    delinks = syms_file.with_name("delinks.txt")
    with delinks.open("a") as f:
        alias = a.source[:-2] + "_itcm.c"  # dsd needs a distinct name; build.py maps it to <file>.o(.itcm)
        f.write(f"\n{alias}:\n    complete\n    .text       start:{lo:#010x} end:{hi:#010x}\n")
    print(f"itcm unit: .text {lo:#x}..{hi:#x} ({len(found)} functions)")


if __name__ == "__main__":
    main()

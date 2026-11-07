"""Compiler flags per source area, shared by build.py and match.py."""
from pathlib import PurePath

# NitroSystem / NitroSDK: same flags as the ntrtwl reference builds.
LIB_FLAGS = [
    "-O4,p", "-proc", "arm946e", "-enum", "int", "-lang", "c99", "-Cpp_exceptions", "off",
    "-gccext,on", "-ipa", "file", "-interworking", "-inline", "on,noauto", "-char", "signed",
    "-gccinc", "-nolink", "-msgstyle", "gcc", "-i", "include",
]

# Game code (guess based on other NDS decomps; refine once game functions are matched).
GAME_FLAGS = [
    "-O4,p", "-enum", "int", "-char", "signed", "-str", "noreuse", "-proc", "arm946e",
    "-gccext,on", "-fp", "soft", "-inline", "noauto", "-Cpp_exceptions", "off", "-RTTI", "off",
    "-interworking", "-w", "off", "-sym", "on", "-gccinc", "-nolink", "-msgstyle", "gcc",
    "-i", "include",
]


def flags_for(src):
    parts = PurePath(str(src).replace("\\", "/")).parts
    if "nnsys" in parts or "nitro" in parts:
        return list(LIB_FLAGS)
    lang = "-lang=c++" if PurePath(str(src)).suffix in (".cpp", ".cc") else "-lang=c"
    return GAME_FLAGS + [lang]

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
    "-interworking", "-w", "off", "-sym", "on", "-nosyspath", "-gccinc", "-nolink", "-msgstyle", "gcc",
    "-DSDK_TS", "-DSDK_4M", "-DSDK_ARM9", "-DSDK_CW", "-DSDK_FINALROM", "-DSDK_CODE_ARM", "-DNNS_FINALROM",
    "-i", "include/game", "-i", "libs/stub", "-i", "libs/NitroSDK/include", "-i", "libs/NitroSystem/include",
]


# Vendored reference library sources (libs/NitroSDK, libs/NitroSystem).
VENDOR_FLAGS = [
    "-O4,p", "-proc", "arm946e", "-enum", "int", "-lang", "c99", "-Cpp_exceptions", "off",
    "-gccext,on", "-ipa", "file", "-interworking", "-inline", "on,noauto", "-char", "signed",
    "-nosyspath", "-gccinc", "-nolink", "-msgstyle", "gcc",
    "-DSDK_CW_FORCE_EXPORT_SUPPORT", "-DSDK_TS", "-DSDK_4M", "-DSDK_ARM9", "-DSDK_CW", "-DSDK_FINALROM",
    "-DSDK_CODE_ARM", "-DNNS_FINALROM", "-DSP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS",
    "-i", "libs/stub", "-i", "libs/NitroSDK/include", "-i", "libs/NitroSystem/include",
]


# Compiler version per source area (tools/mwccarm/<version>).
DEFAULT_COMPILER = "2.0/sp2p2"
SDK_COMPILER = "2.0/sp1p5"  # NitroSDK/NitroSystem were built with an older compiler (<= sp1p5)


def compiler_for(src):
    parts = PurePath(str(src).replace("\\", "/")).parts
    if len(parts) > 1 and parts[0] == "libs":
        return SDK_COMPILER
    return DEFAULT_COMPILER


def flags_for(src):
    parts = PurePath(str(src).replace("\\", "/")).parts
    if parts and parts[0] == "libs":
        extra = []
        if len(parts) > 3 and parts[2] == "libraries":
            lib = PurePath(*parts[:4])
            extra = ["-i", str(lib / "include"), "-i", str(lib / "src")]
        if parts[-1] == "crt0.c":
            # dsd finds the module params and autoload callback by these names.
            extra += ["-D_start_ModuleParams=BuildInfo", "-D_start_AutoloadDoneCallback=AutoloadCallback"]
        return list(VENDOR_FLAGS) + extra
    if "nnsys" in parts or "nitro" in parts:
        return list(LIB_FLAGS)
    lang = "-lang=c++" if PurePath(str(src)).suffix in (".cpp", ".cc") else "-lang=c"
    return GAME_FLAGS + [lang]

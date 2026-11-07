// Data that the original IslandView.cpp has at the end of its sections, kept in a unit linked right
// after IslandView.cpp (and IslandPaths.cpp). Layout workarounds:
// - the file-name suffixes and "camera" are string literals in the original, the last of
//   IslandView.cpp's .data; a struct here because the path strings before them had to move out of
//   IslandView.cpp.
// - a 4-byte constant 64 after IslandView.cpp's other constants, apparently unused (maybe the model
//   array size); mwcc sorts .rodata by size, which would put it first inside IslandView.cpp.
#include "ov003_strings.h"

IslandViewStrings gIslandViewStrings = {
    ".NSBMD", ".NSBTP", ".NSBTA", ".NSBCA", ".NSBMA", ".NSBVA", "camera",
};

extern const int gIslandUnusedMaxModels[1];
const int gIslandUnusedMaxModels[1] = {64};

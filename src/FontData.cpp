// Globals of Font.cpp (FontState gFont). In the original they are statics placed before the vblank_count
// of <nitro/gx/gx.h>; our mwcc puts vblank_count first, so they live here (and in FontData2.cpp, split
// so that our size-sorted .bss keeps the original order), with plain types. Layout workaround.
typedef unsigned long u32;

u32 gFont[0x14 / 4];

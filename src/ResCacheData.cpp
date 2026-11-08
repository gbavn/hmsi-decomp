// The globals of ResCache.cpp (see ResCache.h). In the original they are statics placed before the
// vblank_count of <nitro/gx/gx.h>; our mwcc puts vblank_count first and sorts same-size objects its own
// way, so they live in this unit and ResCacheData2.cpp, with plain types so that they don't include
// the g2d headers. Layout workaround.
typedef unsigned long u32;

u32 gResCacheState;
u32 gResCacheDtorChain[4][3];
u32 gResCrcTable[0x400 / 4];
u32 gAnimCache[0x1804 / 4];

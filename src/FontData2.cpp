// More globals of Font.cpp (see FontData.cpp): the destructor chain link of gFontFile (filled by the
// still-asm static initializer 0x020afd58), the 13 loaded palettes and the glyph file (a GFileObject).
typedef unsigned long u32;

u32 gFontDtorChain[3];
u32 gFontPalettes[13];
u32 gFontFile[0x4c / 4];

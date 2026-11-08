// The message font (ARM9 0x0202f140..0x0202fa88). Names are ours.
//
// Text is a byte string: '@' starts a 2-byte code (written high byte first), 0x81-0x9f / 0xe0-0xff start a
// 2-byte Shift-JIS-like code (low byte first), anything else is a 1-byte code. Codes are looked up in
// the code tables font2x2.bin (16-bit codes, wide glyphs) and font1x2.bin (8-bit codes, narrow glyphs);
// the index found selects the glyph in FontIndex.bin (wide, 0x80 bytes each, read from the file or
// memory) or FontIndex2.bin (narrow, 0x40 bytes each, in memory). The 13 palettes are rom/mes/font_0xx.NCLR.
#include <nitro.h>
#include <nnsys/g2d.h>
#include <nnsys/gfd.h>
#include "game_file.h"
#include "ResCache.h"

extern "C" {
void func_0202af40(void *obj, int value);
void func_02029df4(void *quad, s16 x, s16 y, s16 w, s16 h, int a, int b, int c);
void func_02059690(const void *src, void *dst, u32 w, u32 h);
void func_02036dd4(const void *src, u16 offset, u32 size);
extern u16 data_02151b20[];
}

// A table read from a file (a view class: see PackFile.cpp).
class FontTable {
public:
    virtual FontTable *Dtor();
    virtual FontTable *DtorDelete();

    u8 *data;
    u32 size;
};

typedef struct U32x3 {
    u32 v[3];
} U32x3;

// One loaded palette (0x1c bytes).
typedef struct FontPalette {
    NNSG2dImagePaletteProxy proxy;
    void *data;
    u32 vram;
} FontPalette;

// A glyph drawn as a 3D quad.
typedef struct FontGlyph {
    fx32 z;
    s16 x;
    s16 y;
    u8 *chr;
    void *plttData;
    u8 unk10[4];
    u32 unk14;
    u8 unk18[4];
    u32 unk1C;
    u8 unk20[0x44 - 0x20];
    u32 vram;
    u32 is2x2;
    u32 unk4C;
    u32 unk50;
    u32 unk54;
    u8 unk58[4];
    u32 unk5C;
    NNSG2dImagePaletteProxy pltt;
    u32 plttVram;
    fx32 posX;
    fx32 posY;
    fx32 posZ;
    struct {
        u8 unk0[4];
        s16 x;
        s16 y;
    } quad;
} FontGlyph;

typedef struct FontState {
    u8 wideInMemory;
    u8 *wideGlyphs;
    FontTable *narrowCodes;
    u8 *narrowGlyphs;
    FontTable *wideCodes;
} FontState;

// .data of the file, in one struct to keep the original order.
typedef struct FontData {
    const char *indexFiles[2];
    const char *palettes[13];
    void *tableVtable[4];
    void *baseVtable[4];
    void *fileVtable[4];
    char font013[24];
    char font014[24];
    char font017[24];
    char font018[24];
    char font002[24];
    char font005[24];
    char font012[24];
    char font003[24];
    char font004[24];
    char font010[24];
    char font011[24];
    char font001[24];
    char font000[24];
    char fontIndex[24];
    char fontIndex2[24];
    char font2x2[24];
    char font1x2[24];
} FontData;

extern "C" {
extern FontData gFontData;
extern FontState gFont;
extern u32 gFontDtorChain[3];
extern FontPalette *gFontPalettes[13];
}
extern GFileObject gFontFile;

extern "C" {

const u32 *func_0202f140(const u32 *a, const u32 *b)
{
    return *a < *b ? a : b;
}

void *func_0202f154(void *self)
{
    *(void ***)self = &gFontData.baseVtable[2];
    return self;
}

void *func_0202f164(void *self)
{
    return self;
}

void *func_0202f168(void *self)
{
    operator delete(self);
    return self;
}

// Length of the character code at `s`: 0 for '@', 2 for a 2-byte code, 1 otherwise.
int func_0202f17c(void *self, const u8 *s)
{
    u8 c = *s;

    if (c == '@') {
        return 0;
    }
    if (c >= 0xe0) {
        return 2;
    }
    if (c > 0x80 && c < 0xa0) {
        return 2;
    }
    return 1;
}

u32 func_0202f1b4(void *self, const u16 *code)
{
    FontTable *table = gFont.wideCodes;
    u32 i;
    u32 size = table->size;
    const u16 *codes = (const u16 *)table->data;

    for (i = 0; i < size; i++) {
        if (*code == *codes++) {
            return i;
        }
    }
    return 0;
}

u32 func_0202f1f8(void *self, const u8 *code)
{
    FontTable *table = gFont.narrowCodes;
    u32 i;
    u32 size = table->size;
    const u8 *codes = table->data;

    for (i = 0; i < size; i++) {
        if (*code == *codes++) {
            return i;
        }
    }
    return 0;
}

FontTable *func_0202f23c(FontTable *self, const char *path)
{
    *(void ***)self = &gFontData.tableVtable[2];
    {
        GFileObject file;

        GFile_Open(file.Get(), path);
        self->data = new u8[GFile_GetLength(file.Get())];
        GFile_Read(file.Get(), GFile_GetLength(file.Get()), self->data);
        self->size = GFile_GetLength(file.Get());
    }
    return self;
}

FontTable *func_0202f2cc(FontTable *self)
{
    *(void ***)self = &gFontData.tableVtable[2];
    delete[] self->data;
    return self;
}

FontTable *func_0202f2f0(FontTable *self)
{
    *(void ***)self = &gFontData.tableVtable[2];
    delete[] self->data;
    operator delete(self);
    return self;
}

// Decodes the character at `s` into a glyph index; returns the number of bytes used (-1 at the end).
int func_0202f31c(void *self, const u8 *s, u16 *index, u8 *width)
{
    u32 code;

    if (*s == 0) {
        return -1;
    }
    *width = func_0202f17c(self, s);
    switch (*width) {
    case 0:
        code = 0;
        code = s[0] << 8;
        code |= s[1];
        *index = code;
        return 2;
    case 2:
        code = 0;
        code = s[1] << 8;
        code |= s[0];
        *index = func_0202f1b4(self, (u16 *)&code);
        return 2;
    case 1:
        *index = func_0202f1f8(self, s);
        return 1;
    }
    return 0;
}

// Loads glyph `index` (`width` 1 or 2) into VRAM and sets up `glyph` at (x, y) with palette `pal`.
void func_0202f3e8(void *self, FontGlyph *glyph, u32 index, u32 width, u8 x, u8 y, u8 pal, u8 z)
{
    u32 size;
    s32 posX;
    s32 posY;

    if (width == 2) {
        if (gFont.wideInMemory == 1) {
            glyph->chr = new u8[0x80];
            size = 0x80;
            glyph->is2x2 = TRUE;
            MI_CpuCopy8(gFont.wideGlyphs + index * 0x80, glyph->chr, size);
        } else {
            glyph->chr = new u8[0x80];
            size = 0x80;
            glyph->is2x2 = TRUE;
            GFile_Seek(gFontFile.Get(), index << 7, FS_SEEK_SET);
            GFile_Read(gFontFile.Get(), size, glyph->chr);
        }
    } else if (width == 1) {
        glyph->chr = new u8[0x40];
        size = 0x40;
        glyph->is2x2 = FALSE;
        MI_CpuCopy8(gFont.narrowGlyphs + index * 0x40, glyph->chr, size);
    }
    DC_FlushRange(glyph->chr, size);
    glyph->plttData = gFontPalettes[pal]->data;
    glyph->plttVram = gFontPalettes[pal]->vram;
    posY = y + 8;
    posX = x + width * 4;

    glyph->pltt = gFontPalettes[pal]->proxy;
    glyph->unk1C = 0;
    func_02029df4(&glyph->quad, posX, posY, width * 8, 0x10, 0, 0, 0x1f);
    glyph->posX = posX << 12;
    glyph->posY = posY << 12;
    glyph->posZ = 0;
    glyph->x = glyph->quad.x;
    glyph->y = glyph->quad.y;
    func_0202af40(&glyph->is2x2, 1);
    glyph->unk4C = 1;
    glyph->unk50 = 3;
    glyph->unk54 = 0;
    glyph->unk5C = 0;
    glyph->z = z << 12;
    glyph->unk14 = 0;
    glyph->vram = NNS_GfdAllocTexVram(size, FALSE, 0);
    NNS_GfdRegisterNewVramTransferTask(NNS_GFD_DST_3D_TEX_VRAM, NNS_GfdGetTexKeyAddr(glyph->vram), glyph->chr, size);
    if (width == 2 && gFont.wideInMemory == 0) {
        GFile_Rewind(gFontFile.Get());
    }
}

// Draws glyph `index` on the sub screen's BG (characters at `tile`, map entries at `pos` + `row` * 32).
void func_0202f61c(void *self, u32 index, u32 width, u32 pos, u8 row, u8 pal, u32 tile, u8 transfer)
{
    u32 base = tile * 4;
    u32 size;
    u8 *glyphs;
    u32 count;
    u16 *map;
    u8 *chr;
    u8 *out;
    u32 i;

    if (width == 2) {
        size = 0x80;
        glyphs = gFont.wideGlyphs;
        count = 4;
    } else if (width == 1) {
        size = 0x40;
        glyphs = gFont.narrowGlyphs;
        count = 2;
    }
    map = new u16[count];
    for (i = 0; i < count; i++) {
        map[i] = (base + 1 + i) | (pal << 12);
    }
    DC_FlushRange(map, count * 2);
    chr = new u8[size];
    out = new u8[size];
    if (width == 2 && gFont.wideInMemory == 0) {
        GFile_Seek(gFontFile.Get(), size * index, FS_SEEK_SET);
        GFile_Read(gFontFile.Get(), size, chr);
        DC_FlushRange(chr, size);
    } else {
        MI_CpuCopy8(glyphs + size * index, chr, size);
    }
    if (width == 2) {
        func_02059690(chr, out, 0x10, 0x10);
    } else {
        MI_CpuCopy8(chr, out, size);
    }
    if (transfer) {
        DC_FlushRange(out, size);
        func_02036dd4(out, (tile << 7) + 0x20, size);
        MI_CpuCopy16(map, &data_02151b20[pos + row * 32], width * 2);
        MI_CpuCopy16(&map[(width >> 1) + 1], &data_02151b20[pos + (row + 1) * 32], width * 2);
        delete[] chr;
        delete[] map;
        return;
    }
    DC_FlushRange(out, size);
    GXS_LoadBG1Char(out, tile << 7, size);
    delete[] out;
    delete[] chr;
    delete[] map;
}

void func_0202f81c(void *self, FontGlyph *glyph)
{
    NNS_GfdFreeTexVram(glyph->vram);
    delete[] glyph->chr;
}

void func_0202f844(void)
{
    u8 *buf = new u8[0x600];

    MI_CpuFill8(buf, 0, 0x600);
    DC_FlushRange(buf, 0x600);
    GXS_LoadBG0Scr(buf, 0, 0x600);
    delete[] buf;
}

void func_0202f888(void)
{
    const char *path = gFontData.indexFiles[0];
    int i;

    {
        GFileObject file;

        GFile_Open(file.Get(), path);
        gFont.narrowGlyphs = new u8[GFile_GetLength(file.Get())];
        OS_GetTick();
        GFile_Read(file.Get(), GFile_GetLength(file.Get()), gFont.narrowGlyphs);
        DC_FlushRange(gFont.narrowGlyphs, GFile_GetLength(file.Get()));
    }
    for (i = 0; i < 13; i++) {
        const char *const *paths = gFontData.palettes;

        gFontPalettes[i] = new FontPalette;
        gFontPalettes[i]->data = func_0202c454(&gFontPalettes[i]->proxy, paths[i], &gFontPalettes[i]->vram, FALSE);
    }
    {
        FontTable *table = (FontTable *)operator new(sizeof(FontTable));

        if (table != NULL) {
            table = func_0202f23c(table, gFontData.font2x2);
        }
        gFont.wideCodes = table;
    }
    {
        FontTable *table = (FontTable *)operator new(sizeof(FontTable));

        if (table != NULL) {
            table = func_0202f23c(table, gFontData.font1x2);
        }
        gFont.narrowCodes = table;
    }
}

void func_0202f9bc(void)
{
    GFile_Open(gFontFile.Get(), gFontData.indexFiles[1]);
    func_0202f888();
    gFont.wideInMemory = 0;
}

void func_0202f9f0(void)
{
    int i;

    for (i = 0; i < 13; i++) {
        func_0202d370(gFontPalettes[i]->vram);
        operator delete(gFontPalettes[i]);
    }
    if (gFont.wideCodes != NULL) {
        gFont.wideCodes->DtorDelete();
    }
    if (gFont.narrowCodes != NULL) {
        gFont.narrowCodes->DtorDelete();
    }
}

void func_0202fa60(void)
{
    GFile_Close(gFontFile.Get());
    delete[] gFont.narrowGlyphs;
    func_0202f9f0();
}

void func_0202aec0(void);
void func_0202afe0(void);

FontData gFontData = {
    {gFontData.fontIndex2, gFontData.fontIndex},
    {gFontData.font000, gFontData.font001, gFontData.font002, gFontData.font003, gFontData.font004,
     gFontData.font010, gFontData.font011, gFontData.font005, gFontData.font013, gFontData.font014,
     gFontData.font017, gFontData.font018, gFontData.font012},
    {NULL, NULL, (void *)func_0202f2cc, (void *)func_0202f2f0},
    {NULL, NULL, (void *)func_0202f164, (void *)func_0202f168},
    {NULL, NULL, (void *)func_0202aec0, (void *)func_0202afe0},
    "rom/mes/font_013.NCLR",
    "rom/mes/font_014.NCLR",
    "rom/mes/font_017.NCLR",
    "rom/mes/font_018.NCLR",
    "rom/mes/font_002.NCLR",
    "rom/mes/font_005.NCLR",
    "rom/mes/font_012.NCLR",
    "rom/mes/font_003.NCLR",
    "rom/mes/font_004.NCLR",
    "rom/mes/font_010.NCLR",
    "rom/mes/font_011.NCLR",
    "rom/mes/font_001.NCLR",
    "rom/mes/font_000.NCLR",
    "rom/font/FontIndex.bin",
    "rom/font/FontIndex2.bin",
    "rom/font/font2x2.bin",
    "rom/font/font1x2.bin",
};
}

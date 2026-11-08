#ifndef RESCACHE_H
#define RESCACHE_H

// Resource caches (ARM9 0x0202bf9c..): the 2D graphics files loaded for sprites (characters, palettes,
// cell banks, animation banks) are kept by name and by a CRC32 of the name, with a reference count,
// so several sprites using the same file share one copy in memory/VRAM. Names are ours.
#include <nnsys/g2d.h>
#include <nitro/math/crc.h>
#include <nitro/mi/memory.h>

// Common header of every cache entry. In the original the entries are classes with a virtual
// destructor (vtables 0x020bc3c4..); here the vtable pointer is a plain field.
struct ResEntry {
    void *vtable;
    u32 crc;        // CRC32 of the file name
    char *name;     // file name (from the last '/'), at most 32 bytes
    u16 refCount;   // 0: free slot
    void *data;     // the file
    u8 keepData;    // don't delete `data` on release
    u8 index;       // tells apart different names with the same CRC
};

struct CharEntry : ResEntry {
    u32 vram;                       // NNSGfdTexKey
    NNSG2dImageAttr attr;
    NNSG2dCharacterData charData;
};

struct PlttEntry : ResEntry {
    u32 vram;                       // NNSGfdPlttKey
    NNSG2dPaletteData plttData;
};

struct CellEntry : ResEntry {
    NNSG2dCellDataBank bank;
};

struct AnimEntry : ResEntry {
    NNSG2dAnimBankData bank;
};

struct CharCache {
    void Clear() {
        last = NULL;
        MI_CpuClearFast(entries, sizeof(entries));
    }
    void Add(const char *name, u32 vram, void *data, const NNSG2dImageAttr *attr, const NNSG2dCharacterData *charData,
             u32 crc, u8 index, u8 keepData);
    BOOL FindByName(const char *name);
    BOOL Use(NNSG2dCharacterData **chr, NNSG2dImageProxy *proxy);
    BOOL FindByCrc(u32 crc, u8 index);
    BOOL Release(void);

    CharEntry entries[0xc0];
    CharEntry *last;    // set by the find functions
};

struct PlttCache {
    void Clear() {
        last = NULL;
        MI_CpuClearFast(entries, sizeof(entries));
    }
    void Add(const char *name, u32 vram, void *data, const NNSG2dPaletteData *plttData, u32 crc, u8 index,
             u8 keepData);
    BOOL FindByName(const char *name);
    BOOL Use(NNSG2dImagePaletteProxy *proxy);
    BOOL FindByVram(u32 vram);
    BOOL FindByCrc(u32 crc, u8 index);
    BOOL Release(void);

    PlttEntry entries[0xc0];
    PlttEntry *last;
};

struct CellCache {
    void Clear() {
        last = NULL;
        MI_CpuClearFast(entries, sizeof(entries));
    }
    void Add(const char *name, void *data, NNSG2dCellDataBank bank, u32 crc, u8 index);
    BOOL FindByName(const char *name);
    BOOL FindByCrc(u32 crc, u8 index);
    BOOL Release(void);

    CellEntry entries[0x80];
    CellEntry *last;
};

struct AnimCache {
    void Clear() {
        last = NULL;
        MI_CpuClearFast(entries, sizeof(entries));
    }
    void Add(const char *name, void *data, NNSG2dAnimBankData bank, u32 crc, u8 index);
    BOOL FindByName(const char *name);
    BOOL FindByCrc(u32 crc, u8 index);
    BOOL Release(void);

    AnimEntry entries[0x80];
    AnimEntry *last;
};

extern u32 gResCacheState;
extern u32 gResCacheDtorChain[4][3];   // destructor chain links of the caches (static initializer, still asm)
extern MATHCRC32Table gResCrcTable;
extern AnimCache gAnimCache;
extern CellCache gCellCache;
extern PlttCache gPlttCache;
extern CharCache gCharCache;

// In the original all of the above are statics of ResCache.cpp, which mwcc addresses relative to the
// start of the file's .bss (0x020ec890); some accesses (`gCharCache.last` -> 0x020f4890 + 0xe40) only
// come out the same when written through this view of the whole block.
struct ResCacheGlobals {
    u32 state;
    u32 dtorChain[4][3];
    MATHCRC32Table crcTable;
    AnimCache anim;
    CellCache cell;
    PlttCache pltt;
    CharCache chr;
};
#define gResCache (*(ResCacheGlobals *)&gResCacheState)

extern "C" {
void func_0202bf9c(void);   // clear all caches
void func_0202c024(void);   // init the CRC table
void *func_0202c050(NNSG2dImageProxy *proxy, NNSG2dCharacterData **chr, const char *path, u32 *vram, u32 *crc,
                    u8 *index, u32 transferSize, u8 compressed);
void *func_0202c454(NNSG2dImagePaletteProxy *proxy, const char *path, u32 *vram, u8 compressed);
void *func_0202c72c(NNSG2dCellDataBank **bank, const char *path, u32 *transferSize, u32 *crc, u8 *index,
                    u8 compressed);
void *func_0202c898(NNSG2dAnimBankData **bank, const char *path, u32 *crc, u8 *index, u8 compressed);
void func_0202c9e8(NNSG2dImageProxy *proxy, NNSG2dCharacterData **chr, void *ncgr, u32 *vram, const char *key,
                   u32 *crc, u8 *index, u32 transferSize);
void func_0202cca8(NNSG2dImageProxy *proxy, NNSG2dCharacterData **chr, void *ncgr, u32 *transferKey, u32 *vram,
                   const char *key, u32 *crc, u8 *index);
void func_0202ce30(NNSG2dImagePaletteProxy *proxy, void *nclr, u32 *vram, const char *key);
void func_0202cff4(NNSG2dCellDataBank **bank, void *ncer, u32 *transferSize, const char *key, u32 *crc, u8 *index);
void func_0202d150(NNSG2dAnimBankData **bank, void *nanr, const char *key, u32 *crc, u8 *index);
BOOL func_0202d284(u32 crc, u8 index, u32 vram);    // release a character file
BOOL func_0202d33c(u32 crc, u8 index);
BOOL func_0202d370(u32 vram);                       // release a palette
BOOL func_0202d3d8(u32 crc, u8 index);              // release a cell bank
BOOL func_0202d40c(u32 crc, u8 index);              // release an animation bank
void func_0202e00c(NNSG2dImageProxy *proxy, NNSG2dCharacterData **chr, void *ncgr, u32 *vram, const char *key,
                   u32 *crc, u8 *index);
BOOL func_0202e22c(NNSG2dImagePaletteProxy *proxy, void *nclr, u32 *vram, const char *key);
}

#endif

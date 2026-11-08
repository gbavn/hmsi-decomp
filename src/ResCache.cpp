// The resource caches of the 2D sprites (ARM9 0x0202bf9c..0x0202e810), see ResCache.h.
#include <nnsys/g2d.h>
#include <nnsys/g2d/load/g2d_NAN_load.h>
#include <nnsys/g2d/load/g2d_NCE_load.h>
#include <nnsys/g2d/load/g2d_NCG_load.h>
#include <nnsys/g2d/load/g2d_NCL_load.h>
#include <nnsys/gfd.h>
#include <string.h>
#include "ResCache.h"
#include "game_file.h"

extern "C" {
u32 func_02021d48(const char *s);                             // strlen
const char *func_0202204c(const char *s, int c);              // strrchr
void func_02074cd8(char *dst, u32 size, const char *src);     // bounded strcpy
int func_0202af48(u32 size);
int func_0202af94(u32 size);
}

extern "C" {
static void func_0202e3fc(const NNSG2dPaletteData *pltt, const NNSG2dPaletteCompressInfo *cmpInfo, u32 addr,
                          NNS_G2D_VRAM_TYPE type, NNSG2dImagePaletteProxy *proxy);
static void func_0202e438(const NNSG2dPaletteData *pltt, u32 addr, NNS_G2D_VRAM_TYPE type,
                          NNSG2dImagePaletteProxy *proxy);
static void func_0202e484(const NNSG2dPaletteData *pltt, const NNSG2dPaletteCompressInfo *cmpInfo, u32 addr,
                          NNS_G2D_VRAM_TYPE type);
static void func_0202e500(const NNSG2dPaletteData *pltt, u32 offset, u32 addr, u32 size, NNS_G2D_VRAM_TYPE type);
}

static inline void SetImageAttr(NNSG2dImageProxy *proxy, NNSG2dImageAttr attr)
{
    proxy->attr = attr;
}

static inline u32 CellTransferSize(const NNSG2dCellDataBank *bank)
{
    return bank->pVramTransferData != NULL ? bank->pVramTransferData->szByteMax : 0;
}

extern "C" void func_0202bf9c(void)
{
    gResCache.chr.last = NULL;
    MI_CpuClearFast(gCharCache.entries, sizeof(gCharCache.entries));
    gResCache.pltt.last = NULL;
    MI_CpuClearFast(gPlttCache.entries, sizeof(gPlttCache.entries));
    gResCache.cell.last = NULL;
    MI_CpuClearFast(gCellCache.entries, sizeof(gCellCache.entries));
    gResCache.anim.last = NULL;
    MI_CpuClearFast(gAnimCache.entries, sizeof(gAnimCache.entries));
}

extern "C" void func_0202c024(void)
{
    gResCacheState = 0;
    MATH_CRC32POSIXInitTable(&gResCrcTable);
}

// Loads a character file (NCGR) by path, or reuses the cached copy, and sets up `proxy`.
extern "C" void *func_0202c050(NNSG2dImageProxy *proxy, NNSG2dCharacterData **chr, const char *path, u32 *vram,
                               u32 *crcOut, u8 *indexOut, u32 transferSize, u8 compressed)
{
    u32 crc = MATH_CalcCRC32POSIX(&gResCrcTable, path, func_02021d48(path));
    u8 index = 0;
    BOOL found = gCharCache.FindByName(path);
    BOOL used = gCharCache.FindByCrc(crc, index);
    void *data;
    u32 key;

    if (!found && used) {
        for (u8 i = 1; i < 16; i++) {
            if (!gCharCache.FindByCrc(crc, i)) {
                index = i;
                break;
            }
        }
    } else if (found && !used) {
        gCharCache.FindByName(path);
        index = gResCache.chr.last->index;
        crc = gResCache.chr.last->crc;
    }
    *crcOut = crc;
    *indexOut = index;
    NNS_G2dInitImageProxy(proxy);
    if (transferSize != 0 || !found) {
        GFileObject file;

        GFile_Open(file.Get(), path);
        if (GFile_IsClosed(file.Get())) {
            return NULL;
        }
        if (!found) {
            if (compressed) {
                data = new u8[GFile_GetLength(file.Get())];
                GFile_Read(file.Get(), GFile_GetLength(file.Get()), data);
                data = GFile_UncompressLZ(data);
            } else {
                data = new u8[GFile_GetLength(file.Get())];
                GFile_Read(file.Get(), GFile_GetLength(file.Get()), data);
            }
            if (!NNS_G2dGetUnpackedCharacterData(data, chr)) {
                delete (u8 *)data;
                return NULL;
            }
        } else {
            data = gResCache.chr.last->data;
        }
        if (transferSize != 0) {
            NNSG2dCharacterData *charData;

            key = NNS_GfdAllocTexVram(transferSize, FALSE, 0);
            if (found) {
                gCharCache.Use(chr, proxy);
            }
            charData = *chr;
            DC_FlushRange(charData->pRawData, charData->szByte);
            NNS_G2dLoadImageVramTransfer(*chr, NNS_GfdGetTexKeyAddr(key), NNS_G2D_VRAM_TYPE_3DMAIN, proxy);
            *vram = key;
            if (!found) {
                gCharCache.Add(path, 0xffff, data, &proxy->attr, charData, crc, index, FALSE);
            }
        } else {
            NNSG2dCharacterData *charData = *chr;
            u32 addr;

            DC_FlushRange(charData->pRawData, charData->szByte);
            key = NNS_GfdAllocTexVram(charData->szByte, FALSE, 0);
            addr = NNS_GfdGetTexKeyAddr(key);
            NNS_GfdRegisterNewVramTransferTask(NNS_GFD_DST_3D_TEX_VRAM, addr, charData->pRawData, charData->szByte);
            NNSi_G2dSetupImageProxyPrams(*chr, addr, NNS_G2D_VRAM_TYPE_3DMAIN, proxy);
            gCharCache.Add(path, key, data, &proxy->attr, charData, crc, index, FALSE);
            *vram = key;
        }
        return data;
    }
    if (!gCharCache.Use(chr, proxy)) {
        return NULL;
    }
    *vram = gCharCache.last->vram;
    SetImageAttr(proxy, gCharCache.last->attr);
    return NULL;
}

// Loads a palette file (NCLR) by path, or reuses the cached copy.
extern "C" void *func_0202c454(NNSG2dImagePaletteProxy *proxy, const char *path, u32 *vram, u8 compressed)
{
    u32 crc = MATH_CalcCRC32POSIX(&gResCrcTable, path, func_02021d48(path));
    u8 index = 0;
    BOOL found = gPlttCache.FindByName(path);
    BOOL used = gPlttCache.FindByCrc(crc, index);
    NNSG2dPaletteCompressInfo *cmpInfo;
    NNSG2dPaletteData *plttData;
    void *data;
    u32 key;

    if (!found && used) {
        for (u8 i = 1; i < 16; i++) {
            if (!gPlttCache.FindByCrc(crc, i)) {
                index = i;
                break;
            }
        }
    }
    cmpInfo = NULL;
    NNS_G2dInitImagePaletteProxy(proxy);
    if (!found) {
        GFileObject file;
        BOOL isCompressed;

        GFile_Open(file.Get(), path);
        if (GFile_IsClosed(file.Get())) {
            return NULL;
        }
        if (compressed) {
            data = new u8[GFile_GetLength(file.Get())];
            GFile_Read(file.Get(), GFile_GetLength(file.Get()), data);
            data = GFile_UncompressLZ(data);
        } else {
            data = new u8[GFile_GetLength(file.Get())];
            GFile_Read(file.Get(), GFile_GetLength(file.Get()), data);
        }
        isCompressed = NNS_G2dGetUnpackedPaletteCompressInfo(data, &cmpInfo);
        if (!NNS_G2dGetUnpackedPaletteData(data, &plttData)) {
            delete (u8 *)data;
            return NULL;
        }
        DC_FlushRange(plttData->pRawData, plttData->szByte);
        if (isCompressed) {
            key = NNS_GfdAllocPlttVram(cmpInfo->numPalette << 5, FALSE, NNS_GFD_ALLOC_FROM_LOW);
            func_0202e3fc(plttData, cmpInfo, NNS_GfdGetPlttKeyAddr(key), NNS_G2D_VRAM_TYPE_3DMAIN, proxy);
        } else {
            key = NNS_GfdAllocPlttVram(plttData->szByte, FALSE, NNS_GFD_ALLOC_FROM_LOW);
            func_0202e438(plttData, NNS_GfdGetPlttKeyAddr(key), NNS_G2D_VRAM_TYPE_3DMAIN, proxy);
        }
        gPlttCache.Add(path, key, data, plttData, crc, index, FALSE);
        *vram = key;
        return data;
    }
    if (!gPlttCache.Use(proxy)) {
        return NULL;
    }
    *vram = gResCache.pltt.last->vram;
    return NULL;
}

// Loads a cell bank file (NCER); not cached.
extern "C" void *func_0202c72c(NNSG2dCellDataBank **bank, const char *path, u32 *transferSize, u32 *crc, u8 *index,
                               u8 compressed)
{
    void *data;

    *crc = MATH_CalcCRC32POSIX(&gResCrcTable, path, func_02021d48(path));
    *index = 0;
    GFileObject file;
    GFile_Open(file.Get(), path);
    if (GFile_IsClosed(file.Get())) {
        return NULL;
    }
    if (compressed) {
        data = new u8[GFile_GetLength(file.Get())];
        GFile_Read(file.Get(), GFile_GetLength(file.Get()), data);
        data = GFile_UncompressLZ(data);
    } else {
        data = new u8[GFile_GetLength(file.Get())];
        GFile_Read(file.Get(), GFile_GetLength(file.Get()), data);
    }
    if (!NNS_G2dGetUnpackedCellBank(data, bank)) {
        delete (u8 *)data;
        return NULL;
    }
    if ((*bank)->pVramTransferData != NULL) {
        *transferSize = (*bank)->pVramTransferData->szByteMax;
    } else {
        *transferSize = 0;
    }
    return data;
}

// Loads an animation bank file (NANR); not cached.
extern "C" void *func_0202c898(NNSG2dAnimBankData **bank, const char *path, u32 *crc, u8 *index, u8 compressed)
{
    void *data;

    *crc = MATH_CalcCRC32POSIX(&gResCrcTable, path, func_02021d48(path));
    *index = 0;
    GFileObject file;
    GFile_Open(file.Get(), path);
    if (GFile_IsClosed(file.Get())) {
        return NULL;
    }
    if (compressed) {
        data = new u8[GFile_GetLength(file.Get())];
        GFile_Read(file.Get(), GFile_GetLength(file.Get()), data);
        data = GFile_UncompressLZ(data);
    } else {
        data = new u8[GFile_GetLength(file.Get())];
        GFile_Read(file.Get(), GFile_GetLength(file.Get()), data);
    }
    if (!NNS_G2dGetUnpackedAnimBank(data, bank)) {
        delete (u8 *)data;
        return NULL;
    }
    return data;
}

// Character data already in memory (`ncgr`), cached under `key`.
extern "C" void func_0202c9e8(NNSG2dImageProxy *proxy, NNSG2dCharacterData **chr, void *ncgr, u32 *vram,
                              const char *key, u32 *crcOut, u8 *indexOut, u32 transferSize)
{
    u32 crc = MATH_CalcCRC32POSIX(&gResCrcTable, key, func_02021d48(key));
    u8 index = 0;
    BOOL found = gCharCache.FindByName(key);
    BOOL used = gCharCache.FindByCrc(crc, index);
    NNSG2dCharacterData *charData;
    u32 vramKey;

    if (!found && used) {
        for (u8 i = 1; i < 16; i++) {
            if (!gCharCache.FindByCrc(crc, i)) {
                index = i;
                break;
            }
        }
    } else if (found && !used) {
        gCharCache.FindByName(key);
        index = gResCache.chr.last->index;
        crc = gResCache.chr.last->crc;
    }
    *crcOut = crc;
    *indexOut = index;
    NNS_G2dInitImageProxy(proxy);
    if (transferSize != 0 || !found) {
        if (!found) {
            NNS_G2dGetUnpackedCharacterData(ncgr, chr);
        }
        charData = *chr;
        if (transferSize != 0) {
            vramKey = NNS_GfdAllocTexVram(transferSize, FALSE, 0);
            if (found) {
                gCharCache.Use(chr, proxy);
            }
            NNS_G2dLoadImageVramTransfer(*chr, NNS_GfdGetTexKeyAddr(vramKey), NNS_G2D_VRAM_TYPE_3DMAIN, proxy);
            if (!found) {
                gCharCache.Add(key, 0xffff, ncgr, &proxy->attr, charData, crc, index, TRUE);
            }
            *vram = vramKey;
            return;
        }
        vramKey = NNS_GfdAllocTexVram(charData->szByte, FALSE, 0);
        if (charData->mappingType == GX_OBJVRAMMODE_CHAR_2D) {
            NNS_G2dLoadImage2DMapping(*chr, NNS_GfdGetTexKeyAddr(vramKey), NNS_G2D_VRAM_TYPE_3DMAIN, proxy);
        } else {
            NNS_G2dLoadImage1DMapping(*chr, NNS_GfdGetTexKeyAddr(vramKey), NNS_G2D_VRAM_TYPE_3DMAIN, proxy);
        }
        gCharCache.Add(key, vramKey, ncgr, &proxy->attr, charData, crc, index, TRUE);
        *vram = vramKey;
        return;
    }
    gCharCache.Use(chr, proxy);
    *vram = gCharCache.last->vram;
    SetImageAttr(proxy, gCharCache.last->attr);
}

// Character data in memory whose VRAM area (`*transferKey`) is managed by the caller.
extern "C" void func_0202cca8(NNSG2dImageProxy *proxy, NNSG2dCharacterData **chr, void *ncgr, u32 *transferKey,
                              u32 *vram, const char *key, u32 *crcOut, u8 *indexOut)
{
    u32 crc = MATH_CalcCRC32POSIX(&gResCrcTable, key, func_02021d48(key));
    u8 index = 0;
    BOOL found = gCharCache.FindByName(key);
    BOOL used = gCharCache.FindByCrc(crc, index);
    NNSG2dCharacterData *charData;
    u32 vramKey;

    if (!found && used) {
        for (u8 i = 1; i < 16; i++) {
            if (!gCharCache.FindByCrc(crc, i)) {
                index = i;
                break;
            }
        }
    } else if (found && !used) {
        gCharCache.FindByName(key);
        index = gResCache.chr.last->index;
        crc = gResCache.chr.last->crc;
    }
    *crcOut = crc;
    *indexOut = index;
    NNS_G2dInitImageProxy(proxy);
    if (!found) {
        NNS_G2dGetUnpackedCharacterData(ncgr, chr);
    }
    charData = *chr;
    vramKey = *transferKey;
    if (found) {
        gCharCache.Use(chr, proxy);
    }
    NNS_G2dLoadImageVramTransfer(*chr, NNS_GfdGetTexKeyAddr(vramKey), NNS_G2D_VRAM_TYPE_3DMAIN, proxy);
    if (!found) {
        gCharCache.Add(key, 0xffff, ncgr, &proxy->attr, charData, crc, index, TRUE);
    }
    *vram = vramKey;
}

// Palette data in memory, loaded with the NitroSystem functions.
extern "C" void func_0202ce30(NNSG2dImagePaletteProxy *proxy, void *nclr, u32 *vram, const char *key)
{
    u32 crc = MATH_CalcCRC32POSIX(&gResCrcTable, key, func_02021d48(key));
    u8 index = 0;
    BOOL found = gPlttCache.FindByName(key);
    BOOL used = gPlttCache.FindByCrc(crc, index);
    NNSG2dPaletteCompressInfo *cmpInfo;
    NNSG2dPaletteData *plttData;
    u32 vramKey;

    if (!found && used) {
        for (u8 i = 1; i < 16; i++) {
            if (!gPlttCache.FindByCrc(crc, i)) {
                index = i;
                break;
            }
        }
    }
    cmpInfo = NULL;
    NNS_G2dInitImagePaletteProxy(proxy);
    if (!found) {
        BOOL isCompressed = NNS_G2dGetUnpackedPaletteCompressInfo(nclr, &cmpInfo);

        NNS_G2dGetUnpackedPaletteData(nclr, &plttData);
        if (isCompressed) {
            vramKey = NNS_GfdAllocPlttVram(cmpInfo->numPalette << 5, FALSE, NNS_GFD_ALLOC_FROM_LOW);
            NNS_G2dLoadPaletteEx(plttData, cmpInfo, NNS_GfdGetPlttKeyAddr(vramKey), NNS_G2D_VRAM_TYPE_3DMAIN, proxy);
        } else {
            vramKey = NNS_GfdAllocPlttVram(plttData->szByte, FALSE, NNS_GFD_ALLOC_FROM_LOW);
            NNS_G2dLoadPalette(plttData, NNS_GfdGetPlttKeyAddr(vramKey), NNS_G2D_VRAM_TYPE_3DMAIN, proxy);
        }
        gPlttCache.Add(key, vramKey, nclr, plttData, crc, index, TRUE);
        *vram = vramKey;
        return;
    }
    gPlttCache.Use(proxy);
    *vram = gResCache.pltt.last->vram;
}

// Cell bank in memory.
extern "C" void func_0202cff4(NNSG2dCellDataBank **bank, void *ncer, u32 *transferSize, const char *key,
                              u32 *crcOut, u8 *indexOut)
{
    u32 crc = MATH_CalcCRC32POSIX(&gResCrcTable, key, func_02021d48(key));
    u8 index = 0;
    BOOL found = gCellCache.FindByName(key);
    BOOL used = gCellCache.FindByCrc(crc, index);

    if (!found && used) {
        for (u8 i = 1; i < 16; i++) {
            if (!gCellCache.FindByCrc(crc, i)) {
                index = i;
                break;
            }
        }
    }
    *crcOut = crc;
    *indexOut = index;
    if (!found) {
        NNSG2dCellDataBank *cells;

        NNS_G2dGetUnpackedCellBank(ncer, bank);
        cells = *bank;
        if (cells->pVramTransferData != NULL) {
            *transferSize = cells->pVramTransferData->szByteMax;
        } else {
            *transferSize = 0;
        }
        gCellCache.Add(key, ncer, *cells, crc, index);
    } else {
        NNSG2dCellDataBank *cells = &gCellCache.last->bank;

        *transferSize = CellTransferSize(cells);
        *bank = cells;
        gCellCache.last->refCount++;
    }
}

// Animation bank in memory.
extern "C" void func_0202d150(NNSG2dAnimBankData **bank, void *nanr, const char *key, u32 *crcOut, u8 *indexOut)
{
    u32 crc = MATH_CalcCRC32POSIX(&gResCrcTable, key, func_02021d48(key));
    u8 index = 0;
    BOOL found = gAnimCache.FindByName(key);
    BOOL used = gAnimCache.FindByCrc(crc, index);

    if (!found && used) {
        for (u8 i = 1; i < 16; i++) {
            if (!gAnimCache.FindByCrc(crc, i)) {
                index = i;
                break;
            }
        }
    }
    *crcOut = crc;
    *indexOut = index;
    if (!found) {
        NNS_G2dGetUnpackedAnimBank(nanr, bank);
        gAnimCache.Add(key, nanr, **bank, crc, index);
    } else {
        *bank = &gAnimCache.last->bank;
        gAnimCache.last->refCount++;
    }
}

extern "C" BOOL func_0202d284(u32 crc, u8 index, u32 vram)
{
    if (gCharCache.FindByCrc(crc, index)) {
        u32 cached = gResCache.chr.last->vram;

        if (gCharCache.Release()) {
            NNS_GfdFreeTexVram(cached);
            if (vram != cached) {
                NNS_GfdFreeTexVram(vram);
            }
            return TRUE;
        }
        if (vram != cached) {
            NNS_GfdFreeTexVram(vram);
        }
        return FALSE;
    }
    NNS_GfdFreeTexVram(vram);
    return TRUE;
}

extern "C" BOOL func_0202d33c(u32 crc, u8 index)
{
    if (!gCharCache.FindByCrc(crc, index)) {
        return TRUE;
    }
    return gCharCache.Release();
}

extern "C" BOOL func_0202d370(u32 vram)
{
    if (gPlttCache.FindByVram(vram)) {
        if (!gPlttCache.Release()) {
            return FALSE;
        }
        NNS_GfdFreePlttVram(vram);
        return TRUE;
    }
    NNS_GfdFreePlttVram(vram);
    return TRUE;
}

extern "C" BOOL func_0202d3d8(u32 crc, u8 index)
{
    if (!gCellCache.FindByCrc(crc, index)) {
        return TRUE;
    }
    return gCellCache.Release();
}

extern "C" BOOL func_0202d40c(u32 crc, u8 index)
{
    if (!gAnimCache.FindByCrc(crc, index)) {
        return TRUE;
    }
    return gAnimCache.Release();
}

void CharCache::Add(const char *name, u32 vram, void *data, const NNSG2dImageAttr *attr,
                    const NNSG2dCharacterData *charData, u32 crc, u8 index, u8 keepData)
{
    for (u32 i = 0; i < 0xc0; i++) {
        if (entries[i].refCount == 0) {
            int len = func_02021d48(name);
            const char *base = func_0202204c(name, '/');

            if (base != NULL) {
                int n = func_02021d48(base) + 1;
                if (n > 0x20) {
                    n = 0x20;
                }
                entries[i].name = new char[n];
                func_02074cd8(entries[i].name, n, base);
            } else {
                int n = func_02021d48(name) + 1;
                if (n > 0x20) {
                    n = 0x20;
                }
                entries[i].name = new char[n];
                func_02074cd8(entries[i].name, n, name);
            }
            entries[i].refCount = 1;
            entries[i].data = data;
            entries[i].vram = vram;
            entries[i].attr = *attr;
            entries[i].charData = *charData;
            entries[i].keepData = keepData;
            entries[i].crc = crc;
            entries[i].index = index;
            return;
        }
    }
}

BOOL CharCache::FindByName(const char *name)
{
    for (u32 i = 0; i < 0xc0; i++) {
        if (entries[i].refCount != 0) {
            const char *base = func_0202204c(name, '/');

            if (strcmp(entries[i].name, base != NULL ? base : name) == 0) {
                last = &entries[i];
                return TRUE;
            }
        }
    }
    return FALSE;
}

BOOL CharCache::Use(NNSG2dCharacterData **chr, NNSG2dImageProxy *proxy)
{
    NNSG2dCharacterData *charData = &last->charData;

    *chr = charData;
    if (charData->mappingType != GX_OBJVRAMMODE_CHAR_2D) {
        proxy->attr.sizeS = (GXTexSizeS)charData->W;
        proxy->attr.sizeT = (GXTexSizeT)charData->H;
    } else {
        proxy->attr.sizeS = (GXTexSizeS)func_0202af48(charData->W);
        proxy->attr.sizeT = (GXTexSizeT)func_0202af94(charData->H);
    }
    proxy->attr.fmt = (GXTexFmt)charData->pixelFmt;
    proxy->attr.bExtendedPlt = FALSE;
    proxy->attr.plttUse = GX_TEXPLTTCOLOR0_TRNS;
    proxy->attr.mappingType = (GXOBJVRamModeChar)charData->mappingType;
    NNS_G2dSetImageLocation(proxy, NNS_G2D_VRAM_TYPE_3DMAIN, NNS_GfdGetTexKeyAddr(last->vram));
    last->refCount++;
    return TRUE;
}

BOOL CharCache::FindByCrc(u32 crc, u8 index)
{
    for (u32 i = 0; i < 0xc0; i++) {
        if (entries[i].refCount != 0 && crc == entries[i].crc && index == entries[i].index) {
            last = &entries[i];
            return TRUE;
        }
    }
    return FALSE;
}

BOOL CharCache::Release(void)
{
    CharEntry *used = last;

    used->refCount--;
    if (used->refCount == 0) {
        CharEntry *entry = last;

        if (!entry->keepData && entry->data != NULL) {
            delete (u8 *)entry->data;
        }
        entry->data = NULL;
        delete[] entry->name;
        entry->name = NULL;
        last = NULL;
        return TRUE;
    }
    return FALSE;
}

void PlttCache::Add(const char *name, u32 vram, void *data, const NNSG2dPaletteData *plttData, u32 crc, u8 index,
                    u8 keepData)
{
    for (u32 i = 0; i < 0xc0; i++) {
        if (entries[i].refCount == 0) {
            int len = func_02021d48(name);
            const char *base = func_0202204c(name, '/');

            if (base != NULL) {
                int n = func_02021d48(base) + 1;
                if (n > 0x20) {
                    n = 0x20;
                }
                entries[i].name = new char[n];
                func_02074cd8(entries[i].name, n, base);
            } else {
                int n = func_02021d48(name) + 1;
                if (n > 0x20) {
                    n = 0x20;
                }
                entries[i].name = new char[n];
                func_02074cd8(entries[i].name, n, name);
            }
            entries[i].refCount = 1;
            entries[i].data = data;
            entries[i].vram = vram;
            entries[i].plttData = *plttData;
            entries[i].keepData = keepData;
            entries[i].crc = crc;
            entries[i].index = index;
            return;
        }
    }
}

BOOL PlttCache::FindByName(const char *name)
{
    for (u32 i = 0; i < 0xc0; i++) {
        if (entries[i].refCount != 0) {
            const char *base = func_0202204c(name, '/');

            if (strcmp(entries[i].name, base != NULL ? base : name) == 0) {
                last = &entries[i];
                return TRUE;
            }
        }
    }
    return FALSE;
}

BOOL PlttCache::Use(NNSG2dImagePaletteProxy *proxy)
{
    PlttEntry *entry = last;

    proxy->fmt = (GXTexFmt)entry->plttData.fmt;
    proxy->bExtendedPlt = entry->plttData.bExtendedPlt;
    NNS_G2dSetImagePaletteLocation(proxy, NNS_G2D_VRAM_TYPE_3DMAIN, NNS_GfdGetPlttKeyAddr(last->vram));
    last->refCount++;
    return TRUE;
}

BOOL PlttCache::FindByVram(u32 vram)
{
    for (u32 i = 0; i < 0xc0; i++) {
        if (entries[i].refCount != 0 && vram == entries[i].vram) {
            last = &entries[i];
            return TRUE;
        }
    }
    return FALSE;
}

BOOL PlttCache::FindByCrc(u32 crc, u8 index)
{
    for (u32 i = 0; i < 0xc0; i++) {
        if (entries[i].refCount != 0 && crc == entries[i].crc && index == entries[i].index) {
            last = &entries[i];
            return TRUE;
        }
    }
    return FALSE;
}

BOOL PlttCache::Release(void)
{
    PlttEntry *used = last;

    used->refCount--;
    if (used->refCount == 0) {
        PlttEntry *entry = last;

        if (!entry->keepData && entry->data != NULL) {
            delete (u8 *)entry->data;
        }
        entry->data = NULL;
        delete[] entry->name;
        entry->name = NULL;
        last = NULL;
        return TRUE;
    }
    return FALSE;
}

void CellCache::Add(const char *name, void *data, NNSG2dCellDataBank bank, u32 crc, u8 index)
{
    for (u32 i = 0; i < 0x80; i++) {
        if (entries[i].refCount == 0) {
            NNSG2dCellDataBank copy = bank;
            int len = func_02021d48(name);
            const char *base = func_0202204c(name, '/');

            if (base != NULL) {
                len = func_02021d48(base) + 1;
                if (len > 0x20) {
                    len = 0x20;
                }
                entries[i].name = new char[len];
                func_02074cd8(entries[i].name, len, base);
            } else {
                len = func_02021d48(name) + 1;
                if (len > 0x20) {
                    len = 0x20;
                }
                entries[i].name = new char[len];
                func_02074cd8(entries[i].name, len, name);
            }
            entries[i].refCount = 1;
            entries[i].data = data;
            entries[i].keepData = TRUE;
            entries[i].crc = crc;
            entries[i].index = index;
            entries[i].bank = copy;
            return;
        }
    }
}

BOOL CellCache::FindByName(const char *name)
{
    for (u32 i = 0; i < 0x80; i++) {
        if (entries[i].refCount != 0) {
            const char *base = func_0202204c(name, '/');

            if (strcmp(entries[i].name, base != NULL ? base : name) == 0) {
                last = &entries[i];
                return TRUE;
            }
        }
    }
    return FALSE;
}

BOOL CellCache::FindByCrc(u32 crc, u8 index)
{
    for (u32 i = 0; i < 0x80; i++) {
        if (entries[i].refCount != 0 && crc == entries[i].crc && index == entries[i].index) {
            last = &entries[i];
            return TRUE;
        }
    }
    return FALSE;
}

BOOL CellCache::Release(void)
{
    CellEntry *used = last;

    used->refCount--;
    if (used->refCount == 0) {
        CellEntry *entry = last;

        if (!entry->keepData && entry->data != NULL) {
            delete (u8 *)entry->data;
        }
        entry->data = NULL;
        delete[] entry->name;
        entry->name = NULL;
        last = NULL;
        return TRUE;
    }
    return FALSE;
}

void AnimCache::Add(const char *name, void *data, NNSG2dAnimBankData bank, u32 crc, u8 index)
{
    for (u32 i = 0; i < 0x80; i++) {
        if (entries[i].refCount == 0) {
            NNSG2dAnimBankData copy = bank;
            int len = func_02021d48(name);
            const char *base = func_0202204c(name, '/');

            if (base != NULL) {
                len = func_02021d48(base) + 1;
                if (len > 0x20) {
                    len = 0x20;
                }
                entries[i].name = new char[len];
                func_02074cd8(entries[i].name, len, base);
            } else {
                len = func_02021d48(name) + 1;
                if (len > 0x20) {
                    len = 0x20;
                }
                entries[i].name = new char[len];
                func_02074cd8(entries[i].name, len, name);
            }
            entries[i].refCount = 1;
            entries[i].data = data;
            entries[i].keepData = TRUE;
            entries[i].crc = crc;
            entries[i].index = index;
            entries[i].bank = copy;
            return;
        }
    }
}

BOOL AnimCache::FindByName(const char *name)
{
    for (u32 i = 0; i < 0x80; i++) {
        if (entries[i].refCount != 0) {
            const char *base = func_0202204c(name, '/');

            if (strcmp(entries[i].name, base != NULL ? base : name) == 0) {
                last = &entries[i];
                return TRUE;
            }
        }
    }
    return FALSE;
}

BOOL AnimCache::FindByCrc(u32 crc, u8 index)
{
    for (u32 i = 0; i < 0x80; i++) {
        if (entries[i].refCount != 0 && crc == entries[i].crc && index == entries[i].index) {
            last = &entries[i];
            return TRUE;
        }
    }
    return FALSE;
}

BOOL AnimCache::Release(void)
{
    AnimEntry *used = last;

    used->refCount--;
    if (used->refCount == 0) {
        AnimEntry *entry = last;

        if (!entry->keepData && entry->data != NULL) {
            delete (u8 *)entry->data;
        }
        entry->data = NULL;
        delete[] entry->name;
        entry->name = NULL;
        last = NULL;
        return TRUE;
    }
    return FALSE;
}

// Character data in memory (no transfer area).
extern "C" void func_0202e00c(NNSG2dImageProxy *proxy, NNSG2dCharacterData **chr, void *ncgr, u32 *vram,
                              const char *key, u32 *crcOut, u8 *indexOut)
{
    u32 crc = MATH_CalcCRC32POSIX(&gResCrcTable, key, func_02021d48(key));
    u8 index = 0;
    BOOL found = gCharCache.FindByName(key);
    BOOL used = gCharCache.FindByCrc(crc, index);

    if (!found && used) {
        for (u8 i = 1; i < 16; i++) {
            if (!gCharCache.FindByCrc(crc, i)) {
                index = i;
                break;
            }
        }
    } else if (found && !used) {
        gCharCache.FindByName(key);
        index = gResCache.chr.last->index;
        crc = gResCache.chr.last->crc;
    }
    *crcOut = crc;
    *indexOut = index;
    NNS_G2dInitImageProxy(proxy);
    if (!found) {
        NNSG2dCharacterData *charData;
        u32 vramKey;
        u32 addr;

        NNS_G2dGetUnpackedCharacterData(ncgr, chr);
        charData = *chr;
        vramKey = NNS_GfdAllocTexVram(charData->szByte, FALSE, 0);
        DC_FlushRange(charData->pRawData, charData->szByte);
        addr = NNS_GfdGetTexKeyAddr(vramKey);
        NNS_GfdRegisterNewVramTransferTask(NNS_GFD_DST_3D_TEX_VRAM, addr, charData->pRawData, charData->szByte);
        NNSi_G2dSetupImageProxyPrams(*chr, addr, NNS_G2D_VRAM_TYPE_3DMAIN, proxy);
        gCharCache.Add(key, vramKey, ncgr, &proxy->attr, charData, crc, index, TRUE);
        *vram = vramKey;
        return;
    }
    gCharCache.Use(chr, proxy);
    *vram = gCharCache.last->vram;
    SetImageAttr(proxy, gCharCache.last->attr);

}

// Palette data in memory, loaded through VRAM transfer tasks. Returns whether it was loaded now.
extern "C" BOOL func_0202e22c(NNSG2dImagePaletteProxy *proxy, void *nclr, u32 *vram, const char *key)
{
    u32 crc = MATH_CalcCRC32POSIX(&gResCrcTable, key, func_02021d48(key));
    u8 index = 0;
    BOOL found = gPlttCache.FindByName(key);
    BOOL used = gPlttCache.FindByCrc(crc, index);
    NNSG2dPaletteCompressInfo *cmpInfo;
    NNSG2dPaletteData *plttData;
    u32 vramKey;

    if (!found && used) {
        for (u8 i = 1; i < 16; i++) {
            if (!gPlttCache.FindByCrc(crc, i)) {
                index = i;
                break;
            }
        }
    }
    cmpInfo = NULL;
    NNS_G2dInitImagePaletteProxy(proxy);
    if (!found) {
        BOOL isCompressed = NNS_G2dGetUnpackedPaletteCompressInfo(nclr, &cmpInfo);

        NNS_G2dGetUnpackedPaletteData(nclr, &plttData);
        if (isCompressed) {
            vramKey = NNS_GfdAllocPlttVram(cmpInfo->numPalette << 5, FALSE, NNS_GFD_ALLOC_FROM_LOW);
            func_0202e3fc(plttData, cmpInfo, NNS_GfdGetPlttKeyAddr(vramKey), NNS_G2D_VRAM_TYPE_3DMAIN, proxy);
        } else {
            vramKey = NNS_GfdAllocPlttVram(plttData->szByte, FALSE, NNS_GFD_ALLOC_FROM_LOW);
            func_0202e438(plttData, NNS_GfdGetPlttKeyAddr(vramKey), NNS_G2D_VRAM_TYPE_3DMAIN, proxy);
        }
        gPlttCache.Add(key, vramKey, nclr, plttData, crc, index, TRUE);
        *vram = vramKey;
    } else {
        gPlttCache.Use(proxy);
        *vram = gResCache.pltt.last->vram;
        return FALSE;
    }
    return TRUE;
}

extern "C" {
// Copies of NNS_G2dLoadPaletteEx / NNS_G2dLoadPalette that go through VRAM transfer tasks.
static void func_0202e3fc(const NNSG2dPaletteData *pltt, const NNSG2dPaletteCompressInfo *cmpInfo, u32 addr,
                          NNS_G2D_VRAM_TYPE type, NNSG2dImagePaletteProxy *proxy)
{
    func_0202e484(pltt, cmpInfo, addr, type);
    proxy->fmt = (GXTexFmt)pltt->fmt;
    proxy->bExtendedPlt = pltt->bExtendedPlt;
    NNS_G2dSetImagePaletteLocation(proxy, type, addr);
}

static void func_0202e438(const NNSG2dPaletteData *pltt, u32 addr, NNS_G2D_VRAM_TYPE type,
                          NNSG2dImagePaletteProxy *proxy)
{
    func_0202e500(pltt, 0, addr, pltt->szByte, type);
    proxy->fmt = (GXTexFmt)pltt->fmt;
    proxy->bExtendedPlt = pltt->bExtendedPlt;
    NNS_G2dSetImagePaletteLocation(proxy, type, addr);
}

static void func_0202e484(const NNSG2dPaletteData *pltt, const NNSG2dPaletteCompressInfo *cmpInfo, u32 addr,
                          NNS_G2D_VRAM_TYPE type)
{
    u16 i;
    u16 num;
    u32 size;

    num = cmpInfo->numPalette;
    size = pltt->fmt == GX_TEXFMT_PLTT16 ? 0x20 : 0x200;
    for (i = 0; i < num; i++) {
        func_0202e500(pltt, size * i, addr + size * ((const u16 *)cmpInfo->pPlttIdxTbl)[i], size, type);
    }
}

static void func_0202e500(const NNSG2dPaletteData *pltt, u32 offset, u32 addr, u32 size, NNS_G2D_VRAM_TYPE type)
{
    u8 *src = (u8 *)pltt->pRawData;

    DC_FlushRange(src, pltt->szByte);
    switch (type) {
    case NNS_G2D_VRAM_TYPE_3DMAIN:
        NNS_GfdRegisterNewVramTransferTask(NNS_GFD_DST_3D_TEX_PLTT, addr, src + offset, size);
        break;
    case NNS_G2D_VRAM_TYPE_2DMAIN:
        break;
    case NNS_G2D_VRAM_TYPE_2DSUB:
        break;
    }
}
}

// Out-of-line copies of the cache classes' inline destructors and the entry classes' inline
// constructors/destructors, and their vtables; kept as asm under their linker names.
extern "C" {
void func_020277d8(void);   // __destroy_arr
void _ZdaPv(void);
void _ZdlPv(void);
void _Z11GFile_CloseP5GFile(void);
void func_0202aec0(void);
void func_0202afe0(void);
void func_0202e574(void);
void func_0202e5e4(void);
void func_0202e654(void);
void func_0202e6c4(void);
void func_0202e710(void);
void func_0202e734(void);
void func_0202e760(void);
void func_0202e78c(void);
void func_0202e7b8(void);
void func_0202e7e4(void);

// GFile's (unused here) vtable copy, then the vtables of the anim/cell/palette/char entry classes and
// of their base class.
void *gResCacheVtables[6][4] = {
    {NULL, NULL, (void *)func_0202aec0, (void *)func_0202afe0},
    {NULL, NULL, (void *)func_0202e574, (void *)func_0202e7e4},
    {NULL, NULL, (void *)func_0202e5e4, (void *)func_0202e7b8},
    {NULL, NULL, (void *)func_0202e654, (void *)func_0202e78c},
    {NULL, NULL, (void *)func_0202e6c4, (void *)func_0202e760},
    {NULL, NULL, (void *)func_0202e710, (void *)func_0202e734},
};
}

extern "C" asm void func_0202e550(void) {
    stmdb sp!, {r4, lr}
    ldr r3, [pc, #0x14]
    mov r1, #0x80
    mov r2, #0x30
    mov r4, r0
    bl func_020277d8
    mov r0, r4
    ldmia sp!, {r4, pc}
    dcd func_0202e574
}
extern "C" asm void func_0202e574(void) {
    stmdb sp!, {r4, lr}
    ldr r1, [pc, #0x14]
    mov r4, r0
    str r1, [r4, #0x0]
    ldr r0, [r4, #0x8]
    bl _ZdaPv
    mov r0, r4
    ldmia sp!, {r4, pc}
    dcd gResCacheVtables+0x58
}
extern "C" asm void func_0202e598(void) {
    ldr r1, [pc, #0x18]
    mov r2, #0x0
    str r1, [r0, #0x0]
    str r2, [r0, #0x10]
    ldr r1, [pc, #0xc]
    str r2, [r0, #0x8]
    str r1, [r0, #0x0]
    bx lr
    dcd gResCacheVtables+0x58
    dcd gResCacheVtables+0x18
}
extern "C" asm void func_0202e5c0(void) {
    stmdb sp!, {r4, lr}
    ldr r3, [pc, #0x14]
    mov r1, #0x80
    mov r2, #0x30
    mov r4, r0
    bl func_020277d8
    mov r0, r4
    ldmia sp!, {r4, pc}
    dcd func_0202e5e4
}
extern "C" asm void func_0202e5e4(void) {
    stmdb sp!, {r4, lr}
    ldr r1, [pc, #0x14]
    mov r4, r0
    str r1, [r4, #0x0]
    ldr r0, [r4, #0x8]
    bl _ZdaPv
    mov r0, r4
    ldmia sp!, {r4, pc}
    dcd gResCacheVtables+0x58
}
extern "C" asm void func_0202e608(void) {
    ldr r1, [pc, #0x18]
    mov r2, #0x0
    str r1, [r0, #0x0]
    str r2, [r0, #0x10]
    ldr r1, [pc, #0xc]
    str r2, [r0, #0x8]
    str r1, [r0, #0x0]
    bx lr
    dcd gResCacheVtables+0x58
    dcd gResCacheVtables+0x28
}
extern "C" asm void func_0202e630(void) {
    stmdb sp!, {r4, lr}
    ldr r3, [pc, #0x14]
    mov r1, #0xc0
    mov r2, #0x2c
    mov r4, r0
    bl func_020277d8
    mov r0, r4
    ldmia sp!, {r4, pc}
    dcd func_0202e654
}
extern "C" asm void func_0202e654(void) {
    stmdb sp!, {r4, lr}
    ldr r1, [pc, #0x14]
    mov r4, r0
    str r1, [r4, #0x0]
    ldr r0, [r4, #0x8]
    bl _ZdaPv
    mov r0, r4
    ldmia sp!, {r4, pc}
    dcd gResCacheVtables+0x58
}
extern "C" asm void func_0202e678(void) {
    ldr r1, [pc, #0x18]
    mov r2, #0x0
    str r1, [r0, #0x0]
    str r2, [r0, #0x10]
    ldr r1, [pc, #0xc]
    str r2, [r0, #0x8]
    str r1, [r0, #0x0]
    bx lr
    dcd gResCacheVtables+0x58
    dcd gResCacheVtables+0x38
}
extern "C" asm void func_0202e6a0(void) {
    stmdb sp!, {r4, lr}
    ldr r3, [pc, #0x14]
    mov r1, #0xc0
    mov r2, #0x4c
    mov r4, r0
    bl func_020277d8
    mov r0, r4
    ldmia sp!, {r4, pc}
    dcd func_0202e6c4
}
extern "C" asm void func_0202e6c4(void) {
    stmdb sp!, {r4, lr}
    ldr r1, [pc, #0x14]
    mov r4, r0
    str r1, [r4, #0x0]
    ldr r0, [r4, #0x8]
    bl _ZdaPv
    mov r0, r4
    ldmia sp!, {r4, pc}
    dcd gResCacheVtables+0x58
}
extern "C" asm void func_0202e6e8(void) {
    ldr r1, [pc, #0x18]
    mov r2, #0x0
    str r1, [r0, #0x0]
    str r2, [r0, #0x10]
    ldr r1, [pc, #0xc]
    str r2, [r0, #0x8]
    str r1, [r0, #0x0]
    bx lr
    dcd gResCacheVtables+0x58
    dcd gResCacheVtables+0x48
}
extern "C" asm void func_0202e710(void) {
    stmdb sp!, {r4, lr}
    ldr r1, [pc, #0x14]
    mov r4, r0
    str r1, [r4, #0x0]
    ldr r0, [r4, #0x8]
    bl _ZdaPv
    mov r0, r4
    ldmia sp!, {r4, pc}
    dcd gResCacheVtables+0x58
}
extern "C" asm void func_0202e734(void) {
    stmdb sp!, {r4, lr}
    ldr r1, [pc, #0x1c]
    mov r4, r0
    str r1, [r4, #0x0]
    ldr r0, [r4, #0x8]
    bl _ZdaPv
    mov r0, r4
    bl _ZdlPv
    mov r0, r4
    ldmia sp!, {r4, pc}
    dcd gResCacheVtables+0x58
}
extern "C" asm void func_0202e760(void) {
    stmdb sp!, {r4, lr}
    ldr r1, [pc, #0x1c]
    mov r4, r0
    str r1, [r4, #0x0]
    ldr r0, [r4, #0x8]
    bl _ZdaPv
    mov r0, r4
    bl _ZdlPv
    mov r0, r4
    ldmia sp!, {r4, pc}
    dcd gResCacheVtables+0x58
}
extern "C" asm void func_0202e78c(void) {
    stmdb sp!, {r4, lr}
    ldr r1, [pc, #0x1c]
    mov r4, r0
    str r1, [r4, #0x0]
    ldr r0, [r4, #0x8]
    bl _ZdaPv
    mov r0, r4
    bl _ZdlPv
    mov r0, r4
    ldmia sp!, {r4, pc}
    dcd gResCacheVtables+0x58
}
extern "C" asm void func_0202e7b8(void) {
    stmdb sp!, {r4, lr}
    ldr r1, [pc, #0x1c]
    mov r4, r0
    str r1, [r4, #0x0]
    ldr r0, [r4, #0x8]
    bl _ZdaPv
    mov r0, r4
    bl _ZdlPv
    mov r0, r4
    ldmia sp!, {r4, pc}
    dcd gResCacheVtables+0x58
}
extern "C" asm void func_0202e7e4(void) {
    stmdb sp!, {r4, lr}
    ldr r1, [pc, #0x1c]
    mov r4, r0
    str r1, [r4, #0x0]
    ldr r0, [r4, #0x8]
    bl _ZdaPv
    mov r0, r4
    bl _ZdlPv
    mov r0, r4
    ldmia sp!, {r4, pc}
    dcd gResCacheVtables+0x58
}

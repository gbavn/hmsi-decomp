#include "nnsys/g2d/g2d_Image.h"
#include "nitro/os/cache.h"

static void InitializeVRamLocation_(NNSG2dVRamLocation *pVramLocation);
static void SetVramLocation_(NNSG2dVRamLocation *pVramLocation, NNS_G2D_VRAM_TYPE type, u32 addr);
static u32 GetVramLocation_(const NNSG2dVRamLocation *pVramLocation, NNS_G2D_VRAM_TYPE type);
static BOOL IsImageReadyToUse_(const NNSG2dVRamLocation *pVramLocation, NNS_G2D_VRAM_TYPE type);

static inline void InitializeVRamLocation_(NNSG2dVRamLocation *pVramLocation) {
    int i;
    for (i = 0; i < NNS_G2D_VRAM_TYPE_MAX; i++) {
        pVramLocation->baseAddrOfVram[i] = NNS_G2D_VRAM_ADDR_NOT_INITIALIZED;
    }
}

static inline void SetVramLocation_(NNSG2dVRamLocation *pVramLocation, NNS_G2D_VRAM_TYPE type, u32 addr) {
    pVramLocation->baseAddrOfVram[type] = addr;
}

static inline u32 GetVramLocation_(const NNSG2dVRamLocation *pVramLocation, NNS_G2D_VRAM_TYPE type) {
    return pVramLocation->baseAddrOfVram[type];
}

static inline BOOL IsImageReadyToUse_(const NNSG2dVRamLocation *pVramLocation, NNS_G2D_VRAM_TYPE type) {
    return (GetVramLocation_(pVramLocation, type) != NNS_G2D_VRAM_ADDR_NOT_INITIALIZED) ? TRUE : FALSE;
}

static inline int GetPow_(u16 num) {
    switch (num) {
    case 1:
        return GX_TEXSIZE_S8;
    case 2:
        return GX_TEXSIZE_S16;
    case 4:
        return GX_TEXSIZE_S32;
    case 8:
        return GX_TEXSIZE_S64;
    case 16:
        return GX_TEXSIZE_S128;
    case 32:
        return GX_TEXSIZE_S256;
    default:
        return GX_TEXSIZE_S8;
    }
}

static inline void CopyCharDataToImageAttr_(const NNSG2dCharacterData *pSrc, NNSG2dImageAttr *pDst) {
    if (pSrc->mappingType == GX_OBJVRAMMODE_CHAR_2D) {
        pDst->sizeS = (GXTexSizeS)(GetPow_(pSrc->W));
        pDst->sizeT = (GXTexSizeT)(GetPow_(pSrc->H));
    } else {
        pDst->sizeS = (GXTexSizeS)pSrc->W;
        pDst->sizeT = (GXTexSizeT)pSrc->H;
    }

    pDst->fmt = (GXTexFmt)pSrc->pixelFmt;
    pDst->bExtendedPlt = FALSE;
    pDst->plttUse = GX_TEXPLTTCOLOR0_TRNS;
    pDst->mappingType = (GXOBJVRamModeChar)pSrc->mappingType;
}

static inline void DoLoadingToVram_(const NNSG2dCharacterData *pSrcData, u32 baseAddr, NNS_G2D_VRAM_TYPE type) {
    DC_FlushRange(pSrcData->pRawData, pSrcData->szByte);

    switch (type) {
    case NNS_G2D_VRAM_TYPE_3DMAIN:
        GX_BeginLoadTex();
        GX_LoadTex((void *)pSrcData->pRawData, baseAddr, pSrcData->szByte);
        GX_EndLoadTex();
        break;
    case NNS_G2D_VRAM_TYPE_2DMAIN:
        GX_LoadOBJ((void *)pSrcData->pRawData, baseAddr, pSrcData->szByte);
        break;
    case NNS_G2D_VRAM_TYPE_2DSUB:
        GXS_LoadOBJ((void *)pSrcData->pRawData, baseAddr, pSrcData->szByte);
        break;
    default:
        break;
    }
}

static inline void SetOBJVRamModeCharacterMapping_(NNS_G2D_VRAM_TYPE vramType, GXOBJVRamModeChar vramMode) {
    switch (vramType) {
    case NNS_G2D_VRAM_TYPE_3DMAIN:
        break;
    case NNS_G2D_VRAM_TYPE_2DMAIN:
        GX_SetOBJVRamModeChar(vramMode);
        break;
    case NNS_G2D_VRAM_TYPE_2DSUB:
        GXS_SetOBJVRamModeChar(vramMode);
        break;
    default:
        break;
    }
}

static inline void SetupImageProxyPrams_(const NNSG2dCharacterData *pSrcData, u32 baseAddr, NNS_G2D_VRAM_TYPE type,
                                         NNSG2dImageProxy *pImgProxy) {
    CopyCharDataToImageAttr_(pSrcData, &pImgProxy->attr);
    NNS_G2dSetImageLocation(pImgProxy, type, baseAddr);
}

static inline void DoLoadingPalette_(const NNSG2dPaletteData *pSrcData, u32 srcOffset, u32 addr, u32 szByte,
                                     NNS_G2D_VRAM_TYPE type) {
    const void *pSrcRawData = (const void *)((const u8 *)pSrcData->pRawData + srcOffset);

    DC_FlushRange(pSrcData->pRawData, pSrcData->szByte);

    switch (type) {
    case NNS_G2D_VRAM_TYPE_2DMAIN:
        if (pSrcData->bExtendedPlt) {
            GX_BeginLoadOBJExtPltt();
            GX_LoadOBJExtPltt(pSrcRawData, addr, szByte);
            GX_EndLoadOBJExtPltt();
        } else {
            GX_LoadOBJPltt(pSrcRawData, addr, szByte);
        }
        break;
    case NNS_G2D_VRAM_TYPE_2DSUB:
        if (pSrcData->bExtendedPlt) {
            GXS_BeginLoadOBJExtPltt();
            GXS_LoadOBJExtPltt(pSrcRawData, addr, szByte);
            GXS_EndLoadOBJExtPltt();
        } else {
            GXS_LoadOBJPltt(pSrcRawData, addr, szByte);
        }
        break;
    case NNS_G2D_VRAM_TYPE_3DMAIN:
        GX_BeginLoadTexPltt();
        GX_LoadTexPltt(pSrcRawData, addr, szByte);
        GX_EndLoadTexPltt();
        break;
    default:
        break;
    }
}

static inline u32 CalcSizePerOnePltt_(const NNSG2dPaletteData *pSrcData) {
    if (pSrcData->fmt == GX_TEXFMT_PLTT16) {
        return 16 * sizeof(u16);
    } else {
        return 256 * sizeof(u16);
    }
}

static inline u16 GetCompressedPlttOriginalIndex_(const NNSG2dPaletteCompressInfo *pCmpInfo, u16 idx) {
    return *(((u16 *)pCmpInfo->pPlttIdxTbl) + idx);
}

static inline void DoLoadingPaletteEx_(const NNSG2dPaletteData *pSrcData, const NNSG2dPaletteCompressInfo *pCmpInfo,
                                       u32 addr, NNS_G2D_VRAM_TYPE type) {
    u16 i;
    u32 offsetAddr;
    const u32 szOnePltt = CalcSizePerOnePltt_(pSrcData);
    const u16 numIdx = pCmpInfo->numPalette;

    for (i = 0; i < numIdx; i++) {
        offsetAddr = GetCompressedPlttOriginalIndex_(pCmpInfo, i) * szOnePltt;
        DoLoadingPalette_(pSrcData, szOnePltt * i, addr + offsetAddr, szOnePltt, type);
    }
}

void NNS_G2dInitImageProxy(NNSG2dImageProxy *pImg) {
    InitializeVRamLocation_(&pImg->vramLocation);
}

void NNS_G2dSetImageLocation(NNSG2dImageProxy *pImg, NNS_G2D_VRAM_TYPE type, u32 addr) {
    SetVramLocation_(&pImg->vramLocation, type, addr);
}

u32 NNS_G2dGetImageLocation(const NNSG2dImageProxy *pImg, NNS_G2D_VRAM_TYPE type) {
    return GetVramLocation_(&pImg->vramLocation, type);
}

BOOL NNS_G2dIsImageReadyToUse(const NNSG2dImageProxy *pImg, NNS_G2D_VRAM_TYPE type) {
    return IsImageReadyToUse_(&pImg->vramLocation, type);
}

void NNS_G2dInitImagePaletteProxy(NNSG2dImagePaletteProxy *pImg) {
    InitializeVRamLocation_(&pImg->vramLocation);
}

void NNS_G2dSetImagePaletteLocation(NNSG2dImagePaletteProxy *pImg, NNS_G2D_VRAM_TYPE type, u32 addr) {
    SetVramLocation_(&pImg->vramLocation, type, addr);
}

u32 NNS_G2dGetImagePaletteLocation(const NNSG2dImagePaletteProxy *pImg, NNS_G2D_VRAM_TYPE type) {
    return GetVramLocation_(&pImg->vramLocation, type);
}

BOOL NNS_G2dIsImagePaletteReadyToUse(const NNSG2dImagePaletteProxy *pImg, NNS_G2D_VRAM_TYPE type) {
    return IsImageReadyToUse_(&pImg->vramLocation, type);
}

void NNS_G2dLoadImage1DMapping(const NNSG2dCharacterData *pSrcData, u32 baseAddr, NNS_G2D_VRAM_TYPE type,
                               NNSG2dImageProxy *pImgProxy) {
    SetOBJVRamModeCharacterMapping_(type, (GXOBJVRamModeChar)pSrcData->mappingType);
    DoLoadingToVram_(pSrcData, baseAddr, type);
    SetupImageProxyPrams_(pSrcData, baseAddr, type, pImgProxy);
}

void NNS_G2dLoadImage2DMapping(const NNSG2dCharacterData *pSrcData, u32 baseAddr, NNS_G2D_VRAM_TYPE type,
                               NNSG2dImageProxy *pImgProxy) {
    SetOBJVRamModeCharacterMapping_(type, (GXOBJVRamModeChar)pSrcData->mappingType);
    DoLoadingToVram_(pSrcData, baseAddr, type);
    SetupImageProxyPrams_(pSrcData, baseAddr, type, pImgProxy);
}

void NNS_G2dLoadImageVramTransfer(const NNSG2dCharacterData *pSrcData, u32 baseAddr, NNS_G2D_VRAM_TYPE type,
                                  NNSG2dImageProxy *pImgProxy) {
    DC_FlushRange(pSrcData->pRawData, pSrcData->szByte);

    SetOBJVRamModeCharacterMapping_(type, (GXOBJVRamModeChar)pSrcData->mappingType);
    CopyCharDataToImageAttr_(pSrcData, &pImgProxy->attr);
    NNS_G2dSetImageLocation(pImgProxy, type, baseAddr);
}

void NNS_G2dLoadPalette(const NNSG2dPaletteData *pSrcData, u32 addr, NNS_G2D_VRAM_TYPE type,
                        NNSG2dImagePaletteProxy *pPltProxy) {
    DoLoadingPalette_(pSrcData, 0, addr, pSrcData->szByte, type);

    pPltProxy->fmt = (GXTexFmt)pSrcData->fmt;
    pPltProxy->bExtendedPlt = pSrcData->bExtendedPlt;
    NNS_G2dSetImagePaletteLocation(pPltProxy, type, addr);
}

void NNS_G2dLoadPaletteEx(const NNSG2dPaletteData *pSrcData, const NNSG2dPaletteCompressInfo *pCmpInfo, u32 addr,
                          NNS_G2D_VRAM_TYPE type, NNSG2dImagePaletteProxy *pPltProxy) {
    DoLoadingPaletteEx_(pSrcData, pCmpInfo, addr, type);

    pPltProxy->fmt = (GXTexFmt)pSrcData->fmt;
    pPltProxy->bExtendedPlt = pSrcData->bExtendedPlt;
    NNS_G2dSetImagePaletteLocation(pPltProxy, type, addr);
}

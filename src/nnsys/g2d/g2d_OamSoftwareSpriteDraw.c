#include "nnsys/g2d/g2d_OAM.h"
#include "nnsys/g2d/g2d_Image.h"
#include "nitro/gx/g3.h"
#include "nnsys/g2d/g2d_Internal.h"

#define NNSI_G2D_SHIFT_SIZEOF_256PLTT 9
#define NNSI_G2D_SHIFT_SIZEOF_16PLTT 5

#define REG_GX_DISPCNT_EXOBJ_MASK 0x00700000
#define REG_GX_DISPCNT_EXOBJ_SHIFT 20

typedef void (*NNS_G2dOamSoftEmuUVFlipCorrectFunc)(fx32 *pRetU0, fx32 *pRetV0, fx32 *pRetU1, fx32 *pRetV1,
                                                   BOOL bFlipH, BOOL bFlipV);


void NNSi_G2dDrawSpriteFast(s16 px, s16 py, fx32 pz, int sx, int sy, fx32 U0, fx32 V0, fx32 U1, fx32 V1);
void NNSi_G2dDrawSpriteWithMtxFast(s16 px, s16 py, fx32 pz, int sx, int sy, const MtxFx22 *baseMtx, fx32 U0, fx32 V0,
                                   fx32 U1, fx32 V1);
void NNSi_G2dDrawSpriteWithMtxDoubleAffineFast(s16 px, s16 py, fx32 pz, int sx, int sy, const MtxFx22 *baseMtx,
                                               fx32 U0, fx32 V0, fx32 U1, fx32 V1);

typedef struct SoftwareSpriteParamCache {
    fx32 u0;
    fx32 v0;
    fx32 u1;
    fx32 v1;
} SoftwareSpriteParamCache;

typedef struct SpriteParams {
    fx32 u0;
    fx32 v0;
    fx32 u1;
    fx32 v1;
    int sx;
    int sy;
} SpriteParams;


static const GXTexSizeS gxTexSizeSTbl[3][4] = {
    { GX_TEXSIZE_S8, GX_TEXSIZE_S16, GX_TEXSIZE_S32, GX_TEXSIZE_S64 },
    { GX_TEXSIZE_S16, GX_TEXSIZE_S32, GX_TEXSIZE_S32, GX_TEXSIZE_S64 },
    { GX_TEXSIZE_S8, GX_TEXSIZE_S8, GX_TEXSIZE_S16, GX_TEXSIZE_S32 },
};

static const GXTexSizeT gxTexSizeTTbl[3][4] = {
    { GX_TEXSIZE_T8, GX_TEXSIZE_T16, GX_TEXSIZE_T32, GX_TEXSIZE_T64 },
    { GX_TEXSIZE_T8, GX_TEXSIZE_T8, GX_TEXSIZE_T16, GX_TEXSIZE_T32 },
    { GX_TEXSIZE_T16, GX_TEXSIZE_T32, GX_TEXSIZE_T32, GX_TEXSIZE_T64 },
};

static SoftwareSpriteParamCache softwareSpreiteParamCache_ = { 0, 0, 0, 0 };
static NNS_G2dOamSoftEmuUVFlipCorrectFunc s_pUVFlipCorrectFunc = NULL;
static BOOL bAutoZOffsetAdd_ = FALSE;
static fx32 zOffset_ = 0;
static fx32 zOffsetStep_ = -FX32_ONE;

static inline u32 GetNumTexChar_(GXTexSizeS texSize) {
    static const u32 texSize_[] = { 1, 2, 4, 8, 16, 32, 64, 128 };
    return texSize_[texSize];
}

static inline void IncreaseAutoZOffset_(void) {
    if (bAutoZOffsetAdd_) {
        zOffset_ += zOffsetStep_;
    }
}

static inline fx32 GetFx32DepthValue_(int z) {
    if (bAutoZOffsetAdd_) {
        return (z << FX32_SHIFT) + zOffset_;
    } else {
        return (z << FX32_SHIFT);
    }
}

static inline void DoFlip_(BOOL bFlipH, BOOL bFlipV, fx32 *pRetU0, fx32 *pRetU1, fx32 *pRetV0, fx32 *pRetV1) {
    fx32 temp;

    if (bFlipH) {
        temp = *pRetU0;
        *pRetU0 = *pRetU1;
        *pRetU1 = temp;
    }

    if (bFlipV) {
        temp = *pRetV0;
        *pRetV0 = *pRetV1;
        *pRetV1 = temp;
    }

    if (s_pUVFlipCorrectFunc) {
        (*s_pUVFlipCorrectFunc)(pRetU0, pRetV0, pRetU1, pRetV1, bFlipH, bFlipV);
    }
}

static inline void CalcUVFor3DDirect2DMap_(const NNSG2dImageAttr *pTexImageAttr, u16 charName, fx32 *pRetU0,
                                           fx32 *pRetV0) {
    if (pTexImageAttr->fmt == GX_TEXFMT_PLTT256) {
        charName >>= 1;
    }

    {
        const u32 numCharPerOneLine = GetNumTexChar_(pTexImageAttr->sizeS);
        *pRetU0 = (fx32)(((charName & (numCharPerOneLine - 1)) << 3) << FX32_SHIFT);
        *pRetV0 = ((charName >> pTexImageAttr->sizeS) << 3) << FX32_SHIFT;
    }
}

static inline void CalcUVFor3DDirect1DMap_(fx32 *pRetU0, fx32 *pRetV0) {
    *pRetU0 = 0;
    *pRetV0 = 0;
}

static inline u32 GetOffsetByteSizeOfPlt_(GXTexFmt pltFmt, BOOL bExtendedPlt, u16 pltNo) {
    if (bExtendedPlt) {
        return (u32)(pltNo << NNSI_G2D_SHIFT_SIZEOF_256PLTT);
    } else {
        if (pltFmt == GX_TEXFMT_PLTT256) {
            return 0;
        } else {
            return (u32)(pltNo << NNSI_G2D_SHIFT_SIZEOF_16PLTT);
        }
    }
}

static inline int GetCharacterNameShiftBit_(GXOBJVRamModeChar objMappingMode) {
    return (objMappingMode & REG_GX_DISPCNT_EXOBJ_MASK) >> REG_GX_DISPCNT_EXOBJ_SHIFT;
}

static inline void SetTextureParamsFor3DDirect1DMap_(const NNSG2dImageAttr *pTexImageAttr, u32 texBaseAddr,
                                                     GXOamShape shape, u16 charName) {
    const u16 shapeBit = (u16)((shape & GX_OAM_ATTR01_SHAPE_MASK) >> GX_OAM_ATTR01_SHAPE_SHIFT);
    const u16 sizeBit = (u16)((shape & GX_OAM_ATTR01_SIZE_MASK) >> GX_OAM_ATTR01_SIZE_SHIFT);
    const int shiftBit = (5 + GetCharacterNameShiftBit_(pTexImageAttr->mappingType));

    reg_G3_TEXIMAGE_PARAM = GX_PACK_TEXIMAGE_PARAM(pTexImageAttr->fmt, GX_TEXGEN_TEXCOORD,
                                                   gxTexSizeSTbl[shapeBit][sizeBit],
                                                   gxTexSizeTTbl[shapeBit][sizeBit], GX_TEXREPEAT_NONE,
                                                   GX_TEXFLIP_NONE, pTexImageAttr->plttUse,
                                                   texBaseAddr + (charName << shiftBit));
}

static inline void SetTextureParamsFor3DDirect2DMap_(const NNSG2dImageAttr *pTexImageAttr, u32 texBaseAddr) {
    reg_G3_TEXIMAGE_PARAM = GX_PACK_TEXIMAGE_PARAM(pTexImageAttr->fmt, GX_TEXGEN_TEXCOORD, pTexImageAttr->sizeS,
                                                   pTexImageAttr->sizeT, GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE,
                                                   pTexImageAttr->plttUse, texBaseAddr);
}

static inline void SetPaletteParamsFor3DDirect_(const NNSG2dImageAttr *pTexImageAttr, u32 pltBaseAddr,
                                                const GXOamAttr *pOam) {
    const static GXTexFmt pltFmtTbl[] = { GX_TEXFMT_PLTT16, GX_TEXFMT_PLTT256 };
    const GXTexFmt pltFmt = pltFmtTbl[pOam->colorMode];
    const u32 pltOffs = GetOffsetByteSizeOfPlt_(pltFmt, pTexImageAttr->bExtendedPlt, (u16)pOam->cParam);

    G3_TexPlttBase(pltBaseAddr + pltOffs, pltFmt);
}

static inline void SetQuadTranslation_(const GXOamAttr *pOam, const int posX, const int posY, const int posZ) {
    if (G2_GetOBJEffect(pOam) == GX_OAM_EFFECT_AFFINE_DOUBLE) {
        const GXOamShape oamShape = NNS_G2dGetOAMSize(pOam);
        const int halfW = NNS_G2dGetOamSizeX(&oamShape) >> 1;
        const int halfH = NNS_G2dGetOamSizeY(&oamShape) >> 1;

        G3_Translate((posX + halfW) << FX32_SHIFT, (posY + halfH) << FX32_SHIFT, GetFx32DepthValue_(posZ));
    } else {
        G3_Translate(posX << FX32_SHIFT, posY << FX32_SHIFT, GetFx32DepthValue_(posZ));
    }
}

static void CalcSpriteParams_(const GXOamAttr *pOam, const NNSG2dImageAttr *pTexImageAttr, u32 texBaseAddr,
                              u32 pltBaseAddr, SpriteParams *pResult) {
    const GXOamShape shapeOam = NNS_G2dGetOAMSize(pOam);
    const u16 charName = (u16)pOam->charNo;

    pResult->sx = NNS_G2dGetOamSizeX(&shapeOam);
    pResult->sy = NNS_G2dGetOamSizeY(&shapeOam);

    if (pTexImageAttr->mappingType == GX_OBJVRAMMODE_CHAR_2D) {
        SetTextureParamsFor3DDirect2DMap_(pTexImageAttr, texBaseAddr);
        CalcUVFor3DDirect2DMap_(pTexImageAttr, charName, &pResult->u0, &pResult->v0);
    } else {
        SetTextureParamsFor3DDirect1DMap_(pTexImageAttr, texBaseAddr, shapeOam, charName);
        CalcUVFor3DDirect1DMap_(&pResult->u0, &pResult->v0);
    }

    pResult->u1 = pResult->u0 + (pResult->sx << FX32_SHIFT);
    pResult->v1 = pResult->v0 + (pResult->sy << FX32_SHIFT);

    DoFlip_(NNSi_G2dGetOamFlipH(pOam), NNSi_G2dGetOamFlipV(pOam), &pResult->u0, &pResult->u1, &pResult->v0,
            &pResult->v1);

    SetPaletteParamsFor3DDirect_(pTexImageAttr, pltBaseAddr, pOam);
}

void NNSi_G2dSetOamSoftEmuAutoZOffsetFlag(BOOL flag) {
    bAutoZOffsetAdd_ = flag;
}

fx32 NNSi_G2dGetOamSoftEmuAutoZOffset(void) {
    return zOffset_;
}

void NNSi_G2dResetOamSoftEmuAutoZOffset(void) {
    zOffset_ = 0;
}

void NNSi_G2dSetOamSoftEmuAutoZOffsetStep(fx32 step) {
    zOffsetStep_ = step;
}

fx32 NNSi_G2dGetOamSoftEmuAutoZOffsetStep(void) {
    return zOffsetStep_;
}

void NNS_G2dDrawOneOam3DDirectFast(const GXOamAttr *pOam, const NNSG2dImageAttr *pTexImageAttr, u32 texBaseAddr,
                                   u32 pltBaseAddr) {
    SpriteParams spriteParams;
    const s16 posX = NNSi_G2dRepeatXinScreenArea(NNSi_G2dGetOamX(pOam));
    const s16 posY = NNSi_G2dRepeatYinScreenArea(NNSi_G2dGetOamY(pOam));

    CalcSpriteParams_(pOam, pTexImageAttr, texBaseAddr, pltBaseAddr, &spriteParams);

    NNSi_G2dDrawSpriteFast(posX, posY, GetFx32DepthValue_(-1), spriteParams.sx, spriteParams.sy, spriteParams.u0,
                           spriteParams.v0, spriteParams.u1, spriteParams.v1);

    IncreaseAutoZOffset_();
}

void NNS_G2dDrawOneOam3DDirectWithPosFast(s16 posX, s16 posY, s16 posZ, const GXOamAttr *pOam,
                                          const NNSG2dImageAttr *pTexImageAttr, u32 texBaseAddr, u32 pltBaseAddr) {
    SpriteParams spriteParams;

    CalcSpriteParams_(pOam, pTexImageAttr, texBaseAddr, pltBaseAddr, &spriteParams);

    SetQuadTranslation_(pOam, posX, posY, posZ);
    G3_Scale(spriteParams.sx << FX32_SHIFT, spriteParams.sy << FX32_SHIFT, FX32_ONE);

    {
        G3_Begin(GX_BEGIN_QUADS);

        G3_TexCoord(spriteParams.u0, spriteParams.v1);
        G3_Vtx10(0, FX32_ONE, 0);

        G3_TexCoord(spriteParams.u1, spriteParams.v1);
        G3_Vtx10(FX32_ONE, FX32_ONE, 0);

        G3_TexCoord(spriteParams.u1, spriteParams.v0);
        G3_Vtx10(FX32_ONE, 0, 0);

        G3_TexCoord(spriteParams.u0, spriteParams.v0);
        G3_Vtx10(0, 0, 0);

        G3_End();
    }

    IncreaseAutoZOffset_();
}

void NNS_G2dDrawOneOam3DDirectWithPosAffineFast(s16 posX, s16 posY, s16 posZ, const GXOamAttr *pOam,
                                                const NNSG2dImageAttr *pTexImageAttr, u32 texBaseAddr,
                                                u32 pltBaseAddr, const MtxFx22 *pMtx) {
    SpriteParams spriteParams;

    CalcSpriteParams_(pOam, pTexImageAttr, texBaseAddr, pltBaseAddr, &spriteParams);

    if (G2_GetOBJEffect(pOam) == GX_OAM_EFFECT_AFFINE) {
        NNSi_G2dDrawSpriteWithMtxFast(posX, posY, GetFx32DepthValue_(posZ), spriteParams.sx, spriteParams.sy, pMtx,
                                      spriteParams.u0, spriteParams.v0, spriteParams.u1, spriteParams.v1);
    } else {
        NNSi_G2dDrawSpriteWithMtxDoubleAffineFast(posX, posY, GetFx32DepthValue_(posZ), spriteParams.sx,
                                                  spriteParams.sy, pMtx, spriteParams.u0, spriteParams.v0,
                                                  spriteParams.u1, spriteParams.v1);
    }

    IncreaseAutoZOffset_();
}

void NNS_G2dSetOamSoftEmuSpriteParamCache(const GXOamAttr *pOam, const NNSG2dImageAttr *pTexImageAttr, u32 texBaseAddr,
                                          u32 pltBaseAddr) {
    SpriteParams spriteParams;

    CalcSpriteParams_(pOam, pTexImageAttr, texBaseAddr, pltBaseAddr, &spriteParams);

    softwareSpreiteParamCache_.u0 = spriteParams.u0;
    softwareSpreiteParamCache_.v0 = spriteParams.v0;
    softwareSpreiteParamCache_.u1 = spriteParams.u1;
    softwareSpreiteParamCache_.v1 = spriteParams.v1;
}

void NNS_G2dDrawOneOam3DDirectUsingParamCacheFast(s16 posX, s16 posY, s16 posZ, const GXOamAttr *pOam) {
    const GXOamShape shapeOam = NNS_G2dGetOAMSize(pOam);
    const int sx = NNS_G2dGetOamSizeX(&shapeOam);
    const int sy = NNS_G2dGetOamSizeY(&shapeOam);
    const fx32 u0 = softwareSpreiteParamCache_.u0, u1 = softwareSpreiteParamCache_.u1,
               v0 = softwareSpreiteParamCache_.v0, v1 = softwareSpreiteParamCache_.v1;

    SetQuadTranslation_(pOam, posX, posY, posZ);
    G3_Scale(sx << FX32_SHIFT, sy << FX32_SHIFT, FX32_ONE);

    G3_Begin(GX_BEGIN_QUADS);

    G3_TexCoord(u0, v1);
    G3_Vtx10(0, FX32_ONE, 0);

    G3_TexCoord(u1, v1);
    G3_Vtx10(FX32_ONE, FX32_ONE, 0);

    G3_TexCoord(u1, v0);
    G3_Vtx10(FX32_ONE, 0, 0);

    G3_TexCoord(u0, v0);
    G3_Vtx10(0, 0, 0);

    G3_End();

    IncreaseAutoZOffset_();
}

void NNS_G2dSetOamSoftEmuUVFlipCorrectFunc(NNS_G2dOamSoftEmuUVFlipCorrectFunc pFunc) {
    s_pUVFlipCorrectFunc = pFunc;
}

void NNS_G2dResetOamSoftEmuUVFlipCorrectFunc(void) {
    s_pUVFlipCorrectFunc = NULL;
}

#ifndef NNSYS_G2D_INTERNAL_H
#define NNSYS_G2D_INTERNAL_H

// Inline helpers shared by the G2D library (g2di_OamUtil.h, g2di_Mtx32.h, g2d_Oam_data.h, g2d_Cell_data.h).

#include "nitro/gx/gx_oam.h"
#include "nnsys/g2d/g2d_SRTControl.h"
#include "nnsys/g2d/fmt/g2d_Cell_data.h"

#define NNS_G2D_CELL_MAX_X 255
#define NNS_G2D_CELL_MAX_Y 127

extern const u16 NNSi_objSizeWTbl[3][4];
extern const u16 NNSi_objSizeHTbl[3][4];

static inline GXOamShape NNS_G2dGetOAMSize(const GXOamAttr *pOam) {
    return (GXOamShape)(pOam->attr01 & (GX_OAM_ATTR01_SHAPE_MASK | GX_OAM_ATTR01_SIZE_MASK));
}

static inline int NNS_G2dGetOamSizeX(const GXOamShape *pShape) {
    const u32 shapeBit = (*pShape & GX_OAM_ATTR01_SHAPE_MASK) >> GX_OAM_ATTR01_SHAPE_SHIFT;
    const u32 sizeBit = (*pShape & GX_OAM_ATTR01_SIZE_MASK) >> GX_OAM_ATTR01_SIZE_SHIFT;
    return NNSi_objSizeWTbl[shapeBit][sizeBit];
}

static inline int NNS_G2dGetOamSizeY(const GXOamShape *pShape) {
    const u32 shapeBit = (*pShape & GX_OAM_ATTR01_SHAPE_MASK) >> GX_OAM_ATTR01_SHAPE_SHIFT;
    const u32 sizeBit = (*pShape & GX_OAM_ATTR01_SIZE_MASK) >> GX_OAM_ATTR01_SIZE_SHIFT;
    return NNSi_objSizeHTbl[shapeBit][sizeBit];
}

static inline s16 NNSi_G2dGetOamX(const GXOamAttr *oamAttr) {
    s16 val = (s16)((GX_OAM_ATTR01_X_MASK & oamAttr->attr01) >> GX_OAM_ATTR01_X_SHIFT);
    return val;
}

static inline s16 NNSi_G2dGetOamY(const GXOamAttr *oamAttr) {
    s16 val = (s16)((GX_OAM_ATTR01_Y_MASK & oamAttr->attr01) >> GX_OAM_ATTR01_Y_SHIFT);
    return val;
}

static inline s16 NNSi_G2dRepeatXinScreenArea(s16 srcX) {
    if (srcX > 0x0FF) {
        srcX |= 0xFF00;
    }
    return srcX;
}

static inline s16 NNSi_G2dRepeatYinScreenArea(s16 srcY) {
    if (srcY > 0xBF) {
        srcY |= 0xFF00;
    }
    return srcY;
}

static inline BOOL NNSi_G2dGetOamFlipH(const GXOamAttr *oamAttr) {
    return (BOOL)(oamAttr->flipH);
}

static inline BOOL NNSi_G2dGetOamFlipV(const GXOamAttr *oamAttr) {
    return (BOOL)(oamAttr->flipV);
}

static inline s16 NNS_G2dRepeatXinCellSpace(s16 srcX) {
    if (srcX > NNS_G2D_CELL_MAX_X) {
        srcX |= 0xFF00;
    }
    return srcX;
}

static inline s16 NNS_G2dRepeatYinCellSpace(s16 srcY) {
    if (srcY > NNS_G2D_CELL_MAX_Y) {
        srcY |= 0xFF00;
    }
    return srcY;
}

static inline void NNS_G2dCopyCellAsOamAttr(const NNSG2dCellData *pCell, u16 idx, GXOamAttr *pDst) {
    const NNSG2dCellOAMAttrData *pSrc = pCell->pOamAttrArray + idx;

    pDst->attr0 = pSrc->attr0;
    pDst->attr1 = pSrc->attr1;
    pDst->attr2 = pSrc->attr2;
}

static inline void NNS_G2dGetOamTransFx32(const GXOamAttr *pOam, NNSG2dFVec2 *pRet) {
    pRet->x = (fx32)(NNS_G2dRepeatXinCellSpace(NNSi_G2dGetOamX(pOam)) * FX32_ONE);
    pRet->y = (fx32)(NNS_G2dRepeatYinCellSpace(NNSi_G2dGetOamY(pOam)) * FX32_ONE);
}

// This game's NitroSystem was built with NNS_G2D_ASSUME_DOUBLEAFFINE_OBJPOS_ADJUSTED.
static inline void NNSi_G2dRemovePositionAdjustmentFromDoubleAffineOBJ(const GXOamAttr *pOam, NNSG2dFVec2 *pV) {
    if (G2_GetOBJEffect(pOam) == GX_OAM_EFFECT_AFFINE_DOUBLE) {
        const GXOamShape oamShape = NNS_G2dGetOAMSize(pOam);
        const int halfW = NNS_G2dGetOamSizeX(&oamShape) << (FX32_SHIFT - 1);
        const int halfH = NNS_G2dGetOamSizeY(&oamShape) << (FX32_SHIFT - 1);

        pV->x += halfW;
        pV->y += halfH;
    }
}

static inline void NNSi_G2dAdjustDifferenceOfRotateOrientation(const GXOamAttr *pOam, const MtxFx22 *pM,
                                                               NNSG2dFVec2 *pV, BOOL adjustDoubleAffine) {
    const GXOamShape oamShape = NNS_G2dGetOAMSize(pOam);
    const int halfW = NNS_G2dGetOamSizeX(&oamShape) >> 1;
    const int halfH = NNS_G2dGetOamSizeY(&oamShape) >> 1;

    pV->x += -(halfW << FX32_SHIFT) + pM->_00 * halfW + pM->_10 * halfH;
    pV->y += -(halfH << FX32_SHIFT) + pM->_01 * halfW + pM->_11 * halfH;

    if (adjustDoubleAffine) {
        pV->x -= halfW << FX32_SHIFT;
        pV->y -= halfH << FX32_SHIFT;
    }
}

static inline void NNSi_G2dGetMtxTrans(const MtxFx32 *m, NNSG2dFVec2 *pRet) {
    pRet->x = m->_20;
    pRet->y = m->_21;
}

static inline void NNSi_G2dGetMtxRS(const MtxFx32 *m, MtxFx22 *pRet) {
    pRet->_00 = m->_00;
    pRet->_01 = m->_01;
    pRet->_10 = m->_10;
    pRet->_11 = m->_11;
}

static inline void MulMtx22(const MtxFx22 *m, const NNSG2dFVec2 *pSrc, NNSG2dFVec2 *pDst) {
    NNSG2dFVec2 temp;
    NNSG2dFVec2 *pV = pDst;

    if (pDst == pSrc) {
        pV = &temp;
    }

    pV->x = (fx32)((((s64)(m->_00) * pSrc->x + 0x800LL) + ((s64)(m->_10) * pSrc->y + 0x800LL)) >> FX32_SHIFT);
    pV->y = (fx32)((((s64)(m->_01) * pSrc->x + 0x800LL) + ((s64)(m->_11) * pSrc->y + 0x800LL)) >> FX32_SHIFT);

    if (pV == &temp) {
        *pDst = temp;
    }
}

#endif

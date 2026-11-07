#include "nnsys/g2d/g2d_RendererCore.h"
#include "nnsys/g2d/g2d_Internal.h"
#include "nnsys/g2d/g2d_Image.h"
#include "nnsys/g2d/g2d_CellTransferManager.h"
#include "nitro/gx/g3.h"

#define NNSi_G2D_RNDCORE_DUMMY_FLAG FALSE
#define NNSi_G2D_RNDCORE_OAMENTORY_SUCCESSFUL TRUE

#define NNS_G2D_OAM_AFFINE_IDX_NONE 0xFFFE

#define REG_GX_DISPCNT_EXOBJ_MASK 0x00700000
#define REG_GX_DISPCNT_EXOBJ_SHIFT 20

typedef enum OAM_FLIP {
    OAM_FLIP_NONE = 0,
    OAM_FLIP_H = 0x01,
    OAM_FLIP_V = 0x02,
    OAM_FLIP_HV = 0x03,
    OAM_FLIP_MAX = NNS_G2D_OAMFLIP_PATTERN_NUM
} OAM_FLIP;

#define TO_OAM_FLIP(bFlipH, bFlipV) (OAM_FLIP)((bFlipV << 1) | (bFlipH))

void NNS_G2dDrawOneOam3DDirectWithPosFast(s16 posX, s16 posY, s16 posZ, const GXOamAttr *pOam,
                                          const NNSG2dImageAttr *pTexImageAttr, u32 texBaseAddr, u32 pltBaseAddr);

static NNSG2dRndCoreInstance *pTheInstance_ = NULL;

static const MtxFx32 mtxIdentity_ = { FX32_ONE, 0, 0, FX32_ONE, 0, 0 };

static inline u16 GetMtxCacheAffineIndex_(const NNSG2dRndCore2DMtxCache *pMtxCache, NNSG2dSurfaceType surfaceType,
                                          OAM_FLIP flipType) {
    return pMtxCache->affineIndex[flipType][surfaceType - NNS_G2D_SURFACETYPE_MAIN2D];
}

static inline void SetMtxCacheAffineIndex_(NNSG2dRndCore2DMtxCache *pMtxCache, NNSG2dSurfaceType surfaceType,
                                           OAM_FLIP flipType, u16 affineIndex) {
    pMtxCache->affineIndex[flipType][surfaceType - NNS_G2D_SURFACETYPE_MAIN2D] = affineIndex;
}

static inline BOOL IsMtxCacheRegisteredAsAffineParams_(const u16 affineIdx) {
    return (affineIdx != MtxCache_NOT_AVAILABLE) ? TRUE : FALSE;
}

static inline const MtxFx32 *RndCoreGetCurrentMtx_(void) {
    if (pTheInstance_->pCurrentMxt) {
        return pTheInstance_->pCurrentMxt;
    } else {
        return &mtxIdentity_;
    }
}

static inline BOOL IsRndCore2DAffineTransformed_(const NNSG2dRndCoreInstance *pRnd) {
    return (BOOL)(pRnd->flipFlag == NNS_G2D_RENDERERFLIP_NONE && pRnd->pCurrentMtxCacheFor2D != NULL);
}

static inline int GetShiftToConvByteTo2DChar_(GXOBJVRamModeChar mappingType) {
    int shift = (REG_GX_DISPCNT_EXOBJ_MASK & mappingType) >> REG_GX_DISPCNT_EXOBJ_SHIFT;
    return shift;
}

static inline u32 GetNum2DCharacter_(u32 sizeByte, GXOBJVRamModeChar mappingType) {
    const int shiftBit = 5 + GetShiftToConvByteTo2DChar_(mappingType);
    u32 numChar = sizeByte >> shiftBit;
    return numChar;
}

static inline u32 GetTexBaseAddr3D_(const NNSG2dImageProxy *pImgProxy) {
    if (NNS_G2dIsImageReadyToUse(pImgProxy, NNS_G2D_VRAM_TYPE_3DMAIN)) {
        return NNS_G2dGetImageLocation(pImgProxy, NNS_G2D_VRAM_TYPE_3DMAIN);
    } else {
        return 0;
    }
}

static inline u32 GetPltBaseAddr3D_(const NNSG2dImagePaletteProxy *pPltProxy) {
    if (NNS_G2dIsImagePaletteReadyToUse(pPltProxy, NNS_G2D_VRAM_TYPE_3DMAIN)) {
        return NNS_G2dGetImagePaletteLocation(pPltProxy, NNS_G2D_VRAM_TYPE_3DMAIN);
    } else {
        return 0;
    }
}

static inline NNS_G2D_VRAM_TYPE SurfaceTypeToVramType_(NNSG2dSurfaceType surfaceType) {
    return (NNS_G2D_VRAM_TYPE)surfaceType;
}

static inline u32 GetCharacterBase2D_(const NNSG2dImageProxy *pImgProxy, NNSG2dSurfaceType type) {
    const NNS_G2D_VRAM_TYPE vramType = SurfaceTypeToVramType_(type);

    if (NNS_G2dIsImageReadyToUse(pImgProxy, vramType)) {
        u32 baseAddr = NNS_G2dGetImageLocation(pImgProxy, vramType);
        return GetNum2DCharacter_(baseAddr, pImgProxy->attr.mappingType);
    } else {
        return 0;
    }
}

static inline void HandleCellCallBackFunc_(NNSG2dRndCoreDrawCellCallBack pFunc, const NNSG2dCellData *pCell) {
    if (pFunc) {
        (*pFunc)(pTheInstance_, pCell);
    }
}

static inline void HandleCellOamBackFunc_(NNSG2dRndCoreDrawOamCallBack pFunc, const NNSG2dCellData *pCell,
                                          u16 oamIdx) {
    if (pFunc) {
        (*pFunc)(pTheInstance_, pCell, oamIdx);
    }
}

static inline void MakeFlipMtx_(const MtxFx22 *pMtxSrc, MtxFx22 *pMtxDst, OAM_FLIP type) {
    *pMtxDst = *pMtxSrc;

    if (type & OAM_FLIP_H) {
        pMtxDst->_00 = -pMtxDst->_00;
        pMtxDst->_01 = -pMtxDst->_01;
    }

    if (type & OAM_FLIP_V) {
        pMtxDst->_10 = -pMtxDst->_10;
        pMtxDst->_11 = -pMtxDst->_11;
    }
}

static inline u16 LoadMtxCacheAsAffineParams_(NNSG2dRndCore2DMtxCache *pMtxCache, const NNSG2dRndCoreSurface *pSurface,
                                              OAM_FLIP type) {
    u16 affineIdx = GetMtxCacheAffineIndex_(pMtxCache, pSurface->type, type);

    if (!IsMtxCacheRegisteredAsAffineParams_(affineIdx)) {
        if (type == OAM_FLIP_NONE) {
            affineIdx = (*pTheInstance_->pFuncOamAffineRegister)(&pMtxCache->m22);
        } else {
            MtxFx22 mtxTemp;
            MakeFlipMtx_(&pMtxCache->m22, &mtxTemp, type);
            affineIdx = (*pTheInstance_->pFuncOamAffineRegister)(&mtxTemp);
        }
        SetMtxCacheAffineIndex_(pMtxCache, pSurface->type, type, affineIdx);
    }
    return affineIdx;
}

static inline NNS_G2D_VRAM_TYPE ConvertSurfaceTypeToVramType_(NNSG2dSurfaceType surfaceType) {
    return (NNS_G2D_VRAM_TYPE)(surfaceType);
}

static inline u16 GetFlipedOBJPosX_(GXOamAttr *pOam, const GXOamShape shape) {
    return NNS_G2dIsRndCoreFlipH(pTheInstance_) ?
               (u16)(-NNS_G2dRepeatXinCellSpace((s16)pOam->x) - NNS_G2dGetOamSizeX(&shape)) :
               (u16)NNS_G2dRepeatXinCellSpace((s16)pOam->x);
}

static inline u16 GetFlipedOBJPosY_(GXOamAttr *pOam, const GXOamShape shape) {
    return NNS_G2dIsRndCoreFlipV(pTheInstance_) ?
               (u16)(-NNS_G2dRepeatYinCellSpace((s16)pOam->y) - NNS_G2dGetOamSizeY(&shape)) :
               (u16)NNS_G2dRepeatYinCellSpace((s16)pOam->y);
}

static inline void OverwriteOamFlipFlag_(GXOamAttr *pOam) {
    const BOOL bFlipH = NNS_G2dIsRndCoreFlipH(pTheInstance_) ^ NNSi_G2dGetOamFlipH(pOam);
    const BOOL bFlipV = NNS_G2dIsRndCoreFlipV(pTheInstance_) ^ NNSi_G2dGetOamFlipV(pOam);

    pOam->attr01 &= ~GX_OAM_ATTR01_HF_MASK;
    pOam->attr01 |= bFlipH << GX_OAM_ATTR01_HF_SHIFT;

    pOam->attr01 &= ~GX_OAM_ATTR01_VF_MASK;
    pOam->attr01 |= bFlipV << GX_OAM_ATTR01_VF_SHIFT;
}

static void DoAffineTransforme_(const MtxFx22 *pMtxSR, GXOamAttr *pOam, NNSG2dFVec2 *pBaseTrans) {
    NNSG2dFVec2 objTrans;
    GXOamEffect effectTypeAfter;

    NNS_G2dGetOamTransFx32(pOam, &objTrans);
    NNSi_G2dRemovePositionAdjustmentFromDoubleAffineOBJ(pOam, &objTrans);

    MulMtx22(pMtxSR, &objTrans, &objTrans);

    if (pTheInstance_->affineOverwriteMode != NNS_G2D_RND_AFFINE_OVERWRITE_NONE) {
        if (pTheInstance_->affineOverwriteMode == NNS_G2D_RND_AFFINE_OVERWRITE_DOUBLE) {
            effectTypeAfter = GX_OAM_EFFECT_AFFINE_DOUBLE;
        } else {
            effectTypeAfter = GX_OAM_EFFECT_AFFINE;
        }
        G2_SetOBJEffect(pOam, effectTypeAfter, 0);
    }

    {
        const BOOL bShouldAdjust = G2_GetOBJEffect(pOam) == GX_OAM_EFFECT_AFFINE_DOUBLE;
        NNSi_G2dAdjustDifferenceOfRotateOrientation(pOam, pMtxSR, &objTrans, bShouldAdjust);
    }

    objTrans.x += pBaseTrans->x;
    objTrans.y += pBaseTrans->y;

    G2_SetOBJPosition(pOam, objTrans.x >> FX32_SHIFT, objTrans.y >> FX32_SHIFT);
}

static inline void DoFlipTransforme_(GXOamAttr *pOam, NNSG2dFVec2 *pBaseTrans) {
    if (pTheInstance_->flipFlag != NNS_G2D_RENDERERFLIP_NONE) {
        const GXOamShape shape = NNS_G2dGetOAMSize(pOam);

        OverwriteOamFlipFlag_(pOam);

        if (NNS_G2dIsRndCoreFlipH(pTheInstance_)) {
            pOam->x = -pOam->x - NNS_G2dGetOamSizeX(&shape);
        }

        if (NNS_G2dIsRndCoreFlipV(pTheInstance_)) {
            pOam->y = (u8)(-pOam->y - NNS_G2dGetOamSizeY(&shape));
        }
    }

    pOam->x += pBaseTrans->x >> FX32_SHIFT;
    pOam->y += pBaseTrans->y >> FX32_SHIFT;
}

static void DrawCellToSurface2D_(const NNSG2dRndCoreSurface *pSurface, const NNSG2dCellData *pCell) {
    NNSG2dFVec2 baseTrans;
    MtxFx22 mtxSR;
    const MtxFx32 *pCurrMtx = RndCoreGetCurrentMtx_();

    NNSi_G2dGetMtxTrans(pCurrMtx, &baseTrans);

    baseTrans.x -= pSurface->viewRect.posTopLeft.x;
    baseTrans.y -= pSurface->viewRect.posTopLeft.y;

    {
        const u32 baseCharOffset = pTheInstance_->base2DCharOffset;
        const BOOL bAffined = IsRndCore2DAffineTransformed_(pTheInstance_);
        u16 i;
        GXOamAttr *pTempOam = &pTheInstance_->currentOam;
        u16 oamAffinIdx = NNS_G2D_OAM_AFFINE_IDX_NONE;

        for (i = 0; i < pCell->numOAMAttrs; i++) {
            pTheInstance_->bDrawEnable = TRUE;

            NNS_G2dCopyCellAsOamAttr(pCell, i, pTempOam);

            HandleCellOamBackFunc_(pSurface->pBeforeDrawOamBackFunc, pCell, i);

            if (!pTheInstance_->bDrawEnable) {
                continue;
            }

            pTempOam->charNo += baseCharOffset;

            if (bAffined) {
                oamAffinIdx = LoadMtxCacheAsAffineParams_(
                    pTheInstance_->pCurrentMtxCacheFor2D, pSurface,
                    TO_OAM_FLIP(NNSi_G2dGetOamFlipH(pTempOam), NNSi_G2dGetOamFlipV(pTempOam)));

                NNSi_G2dGetMtxRS(pCurrMtx, &mtxSR);
                DoAffineTransforme_(&mtxSR, pTempOam, &baseTrans);
            } else {
                oamAffinIdx = NNS_G2D_OAM_AFFINE_IDX_NONE;
                DoFlipTransforme_(pTempOam, &baseTrans);
            }

            if (NNSi_G2D_RNDCORE_OAMENTORY_SUCCESSFUL !=
                (*pTheInstance_->pFuncOamRegister)(pTempOam, oamAffinIdx, NNSi_G2D_RNDCORE_DUMMY_FLAG)) {
                return;
            }

            HandleCellOamBackFunc_(pSurface->pAfterDrawOamBackFunc, pCell, i);
        }
    }
}

#ifdef NONMATCHING
static void DrawOamToSurface3D_(GXOamAttr *pOam) {
    G3_LoadMtx43(&pTheInstance_->mtxFor3DGE);

    if (pTheInstance_->flipFlag != NNS_G2D_RENDERERFLIP_NONE) {
        const GXOamShape shape = NNS_G2dGetOAMSize(pOam);
        const s16 posX = (s16)GetFlipedOBJPosX_(pOam, shape);
        const s16 posY = (s16)GetFlipedOBJPosY_(pOam, shape);
        const s16 posZ = -1;

        OverwriteOamFlipFlag_(pOam);

        NNS_G2dDrawOneOam3DDirectWithPosFast(posX, posY, posZ, pOam,
                                             &((const NNSG2dImageProxy *)pTheInstance_->pImgProxy)->attr,
                                             pTheInstance_->baseTexAddr3D, pTheInstance_->basePltAddr3D);
    } else {
        const s16 posX = (s16)NNS_G2dRepeatXinCellSpace((s16)pOam->x);
        const s16 posY = (s16)NNS_G2dRepeatYinCellSpace((s16)pOam->y);
        const s16 posZ = -1;

        NNS_G2dDrawOneOam3DDirectWithPosFast(posX, posY, posZ, pOam,
                                             &((const NNSG2dImageProxy *)pTheInstance_->pImgProxy)->attr,
                                             pTheInstance_->baseTexAddr3D, pTheInstance_->basePltAddr3D);
    }
}
#else
// NONMATCHING: kept as assembly until the C version matches.
static asm void DrawOamToSurface3D_(GXOamAttr *pOam) {
    stmdb sp!, {r3, r4, r5, r6, r7, r8, lr}
    sub sp, sp, #0xc
    ldr r1, =pTheInstance_
    mov r7, r0
    ldr r0, [r1, #0x0]
    add r0, r0, #0x40
    bl G3_LoadMtx43
    ldr r0, =pTheInstance_
    ldr r0, [r0, #0x0]
    ldr r1, [r0, #0x24]
    cmp r1, #0x0
    beq _02005528
    ldr r2, [r7, #0x0]
    ldr r1, =0xc000c000
    and r4, r2, r1
    bl NNS_G2dIsRndCoreFlipH
    cmp r0, #0x0
    ldr r0, [r7, #0x0]
    beq _020053f4
    mov r0, r0, lsl #0x7
    mov r3, r0, lsr #0x17
    and r2, r4, #0xc000
    and r1, r4, #0xc0000000
    mov r1, r1, lsr #0x1e
    ldr r0, =NNSi_objSizeWTbl
    mov r2, r2, asr #0xe
    mov r1, r1, lsl #0x1
    add r0, r0, r2, lsl #0x3
    cmp r3, #0xff
    ldrh r1, [r1, r0]
    orrgt r0, r3, #0xff00
    movgt r0, r0, lsl #0x10
    movgt r3, r0, asr #0x10
    add r0, r3, r1
    rsb r0, r0, #0x0
    b _0200540c
_020053f4:
    mov r0, r0, lsl #0x7
    mov r0, r0, lsr #0x17
    cmp r0, #0xff
    orrgt r0, r0, #0xff00
    movgt r0, r0, lsl #0x10
    movgt r0, r0, asr #0x10
_0200540c:
    mov r0, r0, lsl #0x10
    mov r1, r0, lsr #0x10
    ldr r0, =pTheInstance_
    mov r1, r1, lsl #0x10
    ldr r0, [r0, #0x0]
    mov r5, r1, asr #0x10
    bl NNS_G2dIsRndCoreFlipV
    cmp r0, #0x0
    ldr r0, [r7, #0x0]
    beq _02005478
    mov r0, r0, lsl #0x18
    mov r3, r0, lsr #0x18
    and r2, r4, #0xc000
    and r1, r4, #0xc0000000
    mov r1, r1, lsr #0x1e
    ldr r0, =NNSi_objSizeHTbl
    mov r2, r2, asr #0xe
    mov r1, r1, lsl #0x1
    add r0, r0, r2, lsl #0x3
    cmp r3, #0x7f
    ldrh r1, [r1, r0]
    orrgt r0, r3, #0xff00
    movgt r0, r0, lsl #0x10
    movgt r3, r0, asr #0x10
    add r0, r3, r1
    rsb r0, r0, #0x0
    b _02005490
_02005478:
    mov r0, r0, lsl #0x18
    mov r0, r0, lsr #0x18
    cmp r0, #0x7f
    orrgt r0, r0, #0xff00
    movgt r0, r0, lsl #0x10
    movgt r0, r0, asr #0x10
_02005490:
    mov r0, r0, lsl #0x10
    mov r4, r0, lsr #0x10
    ldr r0, =pTheInstance_
    ldr r1, [r7, #0x0]
    ldr r0, [r0, #0x0]
    mov r6, r1, lsl #0x3
    bl NNS_G2dIsRndCoreFlipH
    ldr r2, [r7, #0x0]
    ldr r1, =pTheInstance_
    eor r6, r0, r6, lsr #0x1f
    ldr r0, [r1, #0x0]
    mov r8, r2, lsl #0x2
    bl NNS_G2dIsRndCoreFlipV
    ldr r1, [r7, #0x0]
    eor r3, r0, r8, lsr #0x1f
    bic r2, r1, #0x10000000
    orr r2, r2, r6, lsl #0x1c
    bic r2, r2, #0x20000000
    mov r1, r4, lsl #0x10
    orr r3, r2, r3, lsl #0x1d
    str r2, [r7, #0x0]
    str r3, [r7, #0x0]
    ldr r2, =pTheInstance_
    mov r0, r5
    ldr r5, [r2, #0x0]
    mov r1, r1, asr #0x10
    ldr r4, [r5, #0x8]
    mov r3, r7
    add r4, r4, #0xc
    str r4, [sp, #0x0]
    ldr r4, [r5, #0x14]
    mvn r2, #0x0
    str r4, [sp, #0x4]
    ldr r4, [r5, #0x18]
    str r4, [sp, #0x8]
    bl NNS_G2dDrawOneOam3DDirectWithPosFast
    add sp, sp, #0xc
    ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
_02005528:
    ldr r2, =pTheInstance_
    ldr r0, [r7, #0x0]
    ldr r5, [r2, #0x0]
    mov r0, r0, lsl #0x7
    ldr r1, [r7, #0x0]
    ldr r4, [r5, #0x8]
    mov r0, r0, lsr #0x17
    cmp r0, #0xff
    orrgt r0, r0, #0xff00
    mov r1, r1, lsl #0x18
    add r4, r4, #0xc
    movgt r0, r0, lsl #0x10
    str r4, [sp, #0x0]
    ldr r4, [r5, #0x14]
    mov r1, r1, lsr #0x18
    movgt r0, r0, asr #0x10
    cmp r1, #0x7f
    orrgt r1, r1, #0xff00
    movgt r1, r1, lsl #0x10
    str r4, [sp, #0x4]
    ldr r4, [r5, #0x18]
    movgt r1, r1, asr #0x10
    mov r3, r7
    mvn r2, #0x0
    str r4, [sp, #0x8]
    bl NNS_G2dDrawOneOam3DDirectWithPosFast
    add sp, sp, #0xc
    ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
}
#endif

static void DrawCellToSurface3D_(const NNSG2dRndCoreSurface *pSurface, const NNSG2dCellData *pCell) {
    u16 i = 0;
    GXOamAttr *pTempOam = &pTheInstance_->currentOam;

    for (i = 0; i < pCell->numOAMAttrs; i++) {
        pTheInstance_->bDrawEnable = TRUE;

        NNS_G2dCopyCellAsOamAttr(pCell, i, pTempOam);

        HandleCellOamBackFunc_(pSurface->pBeforeDrawOamBackFunc, pCell, i);

        if (pTheInstance_->bDrawEnable) {
            DrawOamToSurface3D_(pTempOam);
        }

        HandleCellOamBackFunc_(pSurface->pAfterDrawOamBackFunc, pCell, i);
    }
}

static inline void DrawCellImpl_(const NNSG2dCellData *pCell, u32 cellVramTransferHandle) {
    const NNSG2dRndCoreSurface *pSurface = pTheInstance_->pCurrentTargetSurface;

    if (!pSurface->bActive) {
        return;
    }

    pTheInstance_->bDrawEnable = TRUE;

    HandleCellCallBackFunc_(pSurface->pBeforeDrawCellBackFunc, pCell);

    if (pTheInstance_->bDrawEnable) {
        if (cellVramTransferHandle != NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE) {
            NNSi_G2dSetVramTransferCellDrawnFlag(cellVramTransferHandle,
                                                 ConvertSurfaceTypeToVramType_(pSurface->type), TRUE);
        }

        switch (pSurface->type) {
        case NNS_G2D_SURFACETYPE_MAIN3D:
            DrawCellToSurface3D_(pSurface, pCell);
            break;
        case NNS_G2D_SURFACETYPE_MAIN2D:
        case NNS_G2D_SURFACETYPE_SUB2D:
            DrawCellToSurface2D_(pSurface, pCell);
            break;
        case NNS_G2D_SURFACETYPE_MAX:
        default:
            break;
        }
    }

    HandleCellCallBackFunc_(pSurface->pAfterDrawCellBackFunc, pCell);
}

void NNS_G2dInitRndCore(NNSG2dRndCoreInstance *pRnd) {
    MI_CpuFill16(pRnd, 0x0, sizeof(NNSG2dRndCoreInstance));

    pRnd->pCurrentTargetSurface = NULL;
    pRnd->affineOverwriteMode = NNS_G2D_RND_AFFINE_OVERWRITE_DOUBLE;
    pRnd->pImgProxy = NULL;
    pRnd->pPltProxy = NULL;
    pRnd->flipFlag = NNS_G2D_RENDERERFLIP_NONE;
    pRnd->bDrawEnable = TRUE;
    pRnd->pCurrentMtxCacheFor2D = NULL;
}

void NNS_G2dSetRndCoreImageProxy(NNSG2dRndCoreInstance *pRnd, const NNSG2dImageProxy *pImgProxy,
                                 const NNSG2dImagePaletteProxy *pPltProxy) {
    pRnd->pImgProxy = pImgProxy;
    pRnd->pPltProxy = pPltProxy;
}

void NNS_G2dSetRndCoreOamRegisterFunc(NNSG2dRndCoreInstance *pRnd, NNSG2dOamRegisterFunction pFuncOamRegister,
                                      NNSG2dAffineRegisterFunction pFuncOamAffineRegister) {
    pRnd->pFuncOamRegister = pFuncOamRegister;
    pRnd->pFuncOamAffineRegister = pFuncOamAffineRegister;
}

void NNS_G2dSetRndCoreCurrentMtx3D(const MtxFx32 *pCurrentMxt) {
    NNSG2dRndCoreInstance *pRnd = pTheInstance_;

    pRnd->pCurrentMxt = pCurrentMxt;

    pRnd->mtxFor3DGE._00 = pCurrentMxt->_00;
    pRnd->mtxFor3DGE._01 = pCurrentMxt->_01;
    pRnd->mtxFor3DGE._02 = 0;

    pRnd->mtxFor3DGE._10 = pCurrentMxt->_10;
    pRnd->mtxFor3DGE._11 = pCurrentMxt->_11;
    pRnd->mtxFor3DGE._12 = 0;

    pRnd->mtxFor3DGE._20 = 0;
    pRnd->mtxFor3DGE._21 = 0;
    pRnd->mtxFor3DGE._22 = FX32_ONE;

    pRnd->mtxFor3DGE._30 = pCurrentMxt->_20;
    pRnd->mtxFor3DGE._31 = pCurrentMxt->_21;
    pRnd->mtxFor3DGE._32 = pRnd->zFor3DSoftwareSprite;
}

void NNS_G2dSetRndCoreCurrentMtx2D(const MtxFx32 *pMtx, NNSG2dRndCore2DMtxCache *pCurrentMtxCacheFor2D) {
    NNSG2dRndCoreInstance *pRnd = pTheInstance_;

    pRnd->pCurrentMxt = pMtx;
    pRnd->pCurrentMtxCacheFor2D = pCurrentMtxCacheFor2D;
}

void NNS_G2dSetRndCore3DSoftSpriteZvalue(NNSG2dRndCoreInstance *pRnd, fx32 z) {
    pRnd->zFor3DSoftwareSprite = z;
}

void NNS_G2dSetRndCoreSurface(NNSG2dRndCoreInstance *pRnd, NNSG2dRndCoreSurface *pSurface) {
    pRnd->pCurrentTargetSurface = pSurface;
}

BOOL NNS_G2dIsRndCoreFlipH(const NNSG2dRndCoreInstance *pRnd) {
    return (BOOL)((pRnd->flipFlag & NNS_G2D_RENDERERFLIP_H) != 0);
}

BOOL NNS_G2dIsRndCoreFlipV(const NNSG2dRndCoreInstance *pRnd) {
    return (BOOL)((pRnd->flipFlag & NNS_G2D_RENDERERFLIP_V) != 0);
}

void NNS_G2dRndCoreBeginRendering(NNSG2dRndCoreInstance *pRnd) {
    pTheInstance_ = pRnd;

    if (pRnd->pCurrentTargetSurface->type == NNS_G2D_SURFACETYPE_MAIN3D) {
        G3_MtxMode(GX_MTXMODE_PROJECTION);
        G3_PushMtx();
        G3_Translate(-pRnd->pCurrentTargetSurface->viewRect.posTopLeft.x,
                     -pRnd->pCurrentTargetSurface->viewRect.posTopLeft.y, 0);
        G3_MtxMode(GX_MTXMODE_POSITION);

        pRnd->baseTexAddr3D = GetTexBaseAddr3D_(pTheInstance_->pImgProxy);
        pRnd->basePltAddr3D = GetPltBaseAddr3D_(pTheInstance_->pPltProxy);
    } else {
        pTheInstance_->base2DCharOffset =
            GetCharacterBase2D_(pTheInstance_->pImgProxy, pTheInstance_->pCurrentTargetSurface->type);
    }
}

void NNS_G2dRndCoreEndRendering(void) {
    if (pTheInstance_->pCurrentTargetSurface->type == NNS_G2D_SURFACETYPE_MAIN3D) {
        G3_MtxMode(GX_MTXMODE_PROJECTION);
        G3_PopMtx(1);
        G3_MtxMode(GX_MTXMODE_POSITION);
    } else {
        pTheInstance_->base2DCharOffset = 0;
        pTheInstance_->pCurrentMxt = NULL;
        pTheInstance_->pCurrentMtxCacheFor2D = NULL;
    }

    pTheInstance_ = NULL;
}

void NNS_G2dRndCoreDrawCell(const NNSG2dCellData *pCell) {
    DrawCellImpl_(pCell, NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE);
}

void NNS_G2dRndCoreDrawCellVramTransfer(const NNSG2dCellData *pCell, u32 cellVramTransferHandle) {
    DrawCellImpl_(pCell, cellVramTransferHandle);
}

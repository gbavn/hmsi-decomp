#ifndef NNSYS_G2D_CELLANIMATION_H
#define NNSYS_G2D_CELLANIMATION_H

#include "nnsys/g2d/g2d_Animation.h"
#include "nnsys/g2d/g2d_SRTControl.h"
#include "nnsys/g2d/fmt/g2d_Cell_data.h"

#define NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE 0xFFFFFFFF

#define NNS_G2D_ANIMELEM_MASK 0xFF

typedef enum NNSG2dAnimationElement {
    NNS_G2D_ANIMELEMENT_INDEX = 0x0,
    NNS_G2D_ANIMELEMENT_INDEX_SRT,
    NNS_G2D_ANIMELEMENT_INDEX_T,
    NNS_G2D_ANIMELEMENT_MAX
} NNSG2dAnimationElement;

typedef struct NNSG2dAnimDataSRT {
    u16 index;
    u16 rotZ;
    fx32 sx;
    fx32 sy;
    s16 px;
    s16 py;
} NNSG2dAnimDataSRT;

typedef struct NNSG2dAnimDataT {
    u16 index;
    u16 pad_;
    s16 px;
    s16 py;
} NNSG2dAnimDataT;

typedef struct NNSG2dCellAnimation {
    NNSG2dAnimController animCtrl;
    const NNSG2dCellData *pCurrentCell;
    const NNSG2dCellDataBank *pCellDataBank;
    u32 cellTransferStateHandle;
    NNSG2dSRTControl srtCtrl;
} NNSG2dCellAnimation;

static inline NNSG2dAnimationElement NNSi_G2dGetAnimSequenceElementType(u32 animType) {
    return (NNSG2dAnimationElement)(animType & NNS_G2D_ANIMELEM_MASK);
}

static inline BOOL NNS_G2dCellDataBankHasVramTransferData(const NNSG2dCellDataBank *pCellBank) {
    return (BOOL)(pCellBank->pVramTransferData != NULL);
}

static inline const NNSG2dCellVramTransferData *NNSi_G2dGetCellVramTransferData(const NNSG2dCellDataBank *pCellBank,
                                                                                u16 cellIdx) {
    const NNSG2dVramTransferData *pTransferData = pCellBank->pVramTransferData;
    return &pTransferData->pCellTransferDataArray[cellIdx];
}

static inline BOOL NNSi_G2dIsCellAnimVramTransferHandleValid(const NNSG2dCellAnimation *pCellAnim) {
    return (BOOL)(pCellAnim->cellTransferStateHandle != NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE);
}

static inline void NNSi_G2dSetCellAnimVramTransferHandle(NNSG2dCellAnimation *pCellAnim, u32 handle) {
    pCellAnim->cellTransferStateHandle = handle;
}

static inline NNSG2dAnimController *NNS_G2dGetCellAnimationAnimCtrl(NNSG2dCellAnimation *pCellAnim) {
    return &pCellAnim->animCtrl;
}

static inline void NNS_G2dStartAnimCtrl(NNSG2dAnimController *pAnimCtrl) {
    pAnimCtrl->bActive = TRUE;
}

void NNS_G2dInitCellAnimation(NNSG2dCellAnimation *pCellAnim, const NNSG2dAnimSequence *pAnimSeq,
                              const NNSG2dCellDataBank *pCellDataBank);
void NNS_G2dInitCellAnimationVramTransfered(NNSG2dCellAnimation *pCellAnim, const NNSG2dAnimSequence *pAnimSeq,
                                            const NNSG2dCellDataBank *pCellBank, u32 vramStateHandle, u32 dstAddr3D,
                                            u32 dstAddr2DMain, u32 dstAddr2DSub, const void *pSrcNCGR,
                                            const void *pSrcNCBR, u32 szSrcData);
void NNS_G2dSetCellAnimationSequence(NNSG2dCellAnimation *pCellAnim, const NNSG2dAnimSequence *pAnimSeq);
void NNS_G2dSetCellAnimationSequenceNoReset(NNSG2dCellAnimation *pCellAnim, const NNSG2dAnimSequence *pAnimSeq);
void NNS_G2dTickCellAnimation(NNSG2dCellAnimation *pCellAnim, fx32 frames);
void NNS_G2dRestartCellAnimation(NNSG2dCellAnimation *pCellAnim);

void NNSi_G2dInitCellTransferState(u32 handle, u32 dstAddr3D, u32 dstAddr2DMain, u32 dstAddr2DSub, u32 szDst,
                                   const void *pSrcNCGR, const void *pSrcNCBR, u32 szSrcData);
void NNS_G2dSetCellTransferStateRequested(u32 handle, u32 srcOffset, u32 szByte);

#endif

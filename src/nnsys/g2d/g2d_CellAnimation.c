#include "nnsys/g2d/g2d_CellAnimation.h"
#include "nnsys/g2d/load/g2d_NCE_load.h"

static void ApplyCurrentAnimResult_(NNSG2dCellAnimation *pCellAnim) {
    const NNSG2dAnimDataSRT *pAnimResult = NULL;
    const NNSG2dCellDataBank *pCellBank = NULL;

    if (pCellAnim->animCtrl.pActiveCurrent->frames == 0) {
        return;
    }

    pAnimResult = (const NNSG2dAnimDataSRT *)NNS_G2dGetAnimCtrlCurrentElement(&pCellAnim->animCtrl);
    pAnimResult = (const NNSG2dAnimDataSRT *)NNS_G2dGetAnimCtrlCurrentElement(&pCellAnim->animCtrl);
    pCellBank = pCellAnim->pCellDataBank;

    pCellAnim->pCurrentCell = NNS_G2dGetCellDataByIdx(pCellBank, pAnimResult->index);

    {
        const NNSG2dAnimationElement elemType =
            NNSi_G2dGetAnimSequenceElementType(pCellAnim->animCtrl.pAnimSequence->animType);

        NNSi_G2dSrtcInitControl(&pCellAnim->srtCtrl, NNS_G2D_SRTCONTROLTYPE_SRT);

        if (elemType != NNS_G2D_ANIMELEMENT_INDEX) {
            if (elemType == NNS_G2D_ANIMELEMENT_INDEX_T) {
                const NNSG2dAnimDataT *pAnmResT = (const NNSG2dAnimDataT *)pAnimResult;
                NNSi_G2dSrtcSetTrans(&pCellAnim->srtCtrl, pAnmResT->px, pAnmResT->py);
            } else {
                NNSi_G2dSrtcSetSRTScale(&pCellAnim->srtCtrl, pAnimResult->sx, pAnimResult->sy);
                NNSi_G2dSrtcSetSRTRotZ(&pCellAnim->srtCtrl, pAnimResult->rotZ);
                NNSi_G2dSrtcSetTrans(&pCellAnim->srtCtrl, pAnimResult->px, pAnimResult->py);
            }
        }
    }

    if (NNS_G2dCellDataBankHasVramTransferData(pCellBank) && NNSi_G2dIsCellAnimVramTransferHandleValid(pCellAnim)) {
        const NNSG2dCellVramTransferData *pCellTransferData =
            NNSi_G2dGetCellVramTransferData(pCellBank, pAnimResult->index);

        NNS_G2dSetCellTransferStateRequested(pCellAnim->cellTransferStateHandle, pCellTransferData->srcDataOffset,
                                             pCellTransferData->szByte);
    }
}

static inline void InitCellAnimationImpl_(NNSG2dCellAnimation *pCellAnim, const NNSG2dAnimSequence *pAnimSeq,
                                          const NNSG2dCellDataBank *pCellDataBank, u32 cellTransferStateHandle) {
    pCellAnim->pCellDataBank = pCellDataBank;
    pCellAnim->cellTransferStateHandle = cellTransferStateHandle;

    NNSi_G2dSrtcInitControl(&pCellAnim->srtCtrl, NNS_G2D_SRTCONTROLTYPE_SRT);
    NNS_G2dInitAnimCtrl(&pCellAnim->animCtrl);
    NNS_G2dSetCellAnimationSequence(pCellAnim, pAnimSeq);
}

void NNS_G2dInitCellAnimation(NNSG2dCellAnimation *pCellAnim, const NNSG2dAnimSequence *pAnimSeq,
                              const NNSG2dCellDataBank *pCellDataBank) {
    InitCellAnimationImpl_(pCellAnim, pAnimSeq, pCellDataBank, NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE);
}

void NNS_G2dInitCellAnimationVramTransfered(NNSG2dCellAnimation *pCellAnim, const NNSG2dAnimSequence *pAnimSeq,
                                            const NNSG2dCellDataBank *pCellBank, u32 vramStateHandle, u32 dstAddr3D,
                                            u32 dstAddr2DMain, u32 dstAddr2DSub, const void *pSrcNCGR,
                                            const void *pSrcNCBR, u32 szSrcData) {
    {
        const NNSG2dVramTransferData *pVramData = (const NNSG2dVramTransferData *)pCellBank->pVramTransferData;

        NNSi_G2dInitCellTransferState(vramStateHandle, dstAddr3D, dstAddr2DMain, dstAddr2DSub, pVramData->szByteMax,
                                      pSrcNCGR, pSrcNCBR, szSrcData);
        NNSi_G2dSetCellAnimVramTransferHandle(pCellAnim, vramStateHandle);
    }
    InitCellAnimationImpl_(pCellAnim, pAnimSeq, pCellBank, vramStateHandle);
}

void NNS_G2dSetCellAnimationSequence(NNSG2dCellAnimation *pCellAnim, const NNSG2dAnimSequence *pAnimSeq) {
    NNS_G2dBindAnimCtrl(&pCellAnim->animCtrl, pAnimSeq);
    ApplyCurrentAnimResult_(pCellAnim);
}

void NNS_G2dSetCellAnimationSequenceNoReset(NNSG2dCellAnimation *pCellAnim, const NNSG2dAnimSequence *pAnimSeq) {
    {
        const u16 frameIdx = NNS_G2dGetAnimCtrlCurrentFrame(&pCellAnim->animCtrl);

        pCellAnim->animCtrl.pAnimSequence = pAnimSeq;

        if (!NNS_G2dSetAnimCtrlCurrentFrameNoResetCurrentTime(&pCellAnim->animCtrl, frameIdx)) {
            NNS_G2dResetAnimCtrlState(&pCellAnim->animCtrl);
        }
    }
    ApplyCurrentAnimResult_(pCellAnim);
}

void NNS_G2dTickCellAnimation(NNSG2dCellAnimation *pCellAnim, fx32 frames) {
    if (NNS_G2dTickAnimCtrl(&pCellAnim->animCtrl, frames)) {
        ApplyCurrentAnimResult_(pCellAnim);
    }
}

void NNS_G2dRestartCellAnimation(NNSG2dCellAnimation *pCellAnim) {
    NNSG2dAnimController *pAnmCtrl = NULL;

    pAnmCtrl = NNS_G2dGetCellAnimationAnimCtrl(pCellAnim);
    NNS_G2dResetAnimCtrlState(pAnmCtrl);
    NNS_G2dStartAnimCtrl(pAnmCtrl);
    ApplyCurrentAnimResult_(pCellAnim);
}

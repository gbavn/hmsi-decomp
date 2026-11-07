#include "nnsys/g2d/load/g2d_NAN_load.h"
#include "nnsys/g2d/g2d_Load.h"

static BOOL GetUnpackedAnimBankImpl(void *pNanrFile, NNSG2dAnimBankData **ppAnimBank) {
    NNSG2dBinaryFileHeader *pBinFile = (NNSG2dBinaryFileHeader *)pNanrFile;
    NNSG2dAnimBankDataBlock *pAnimBankBlk =
        (NNSG2dAnimBankDataBlock *)NNS_G2dFindBinaryBlock(pBinFile, NNS_G2D_BLKSIG_ANIMBANK);

    if (pAnimBankBlk) {
        NNS_G2dUnpackNAN(&pAnimBankBlk->animBankData);
        *ppAnimBank = &pAnimBankBlk->animBankData;
        return TRUE;
    }

    *ppAnimBank = NULL;
    return FALSE;
}

BOOL NNS_G2dGetUnpackedAnimBank(void *pNanrFile, NNSG2dAnimBankData **ppAnimBank) {
    return GetUnpackedAnimBankImpl(pNanrFile, ppAnimBank);
}

void NNS_G2dUnpackNAN(NNSG2dAnimBankData *pData) {
    u16 i, j;

    pData->pSequenceArrayHead = NNS_G2D_UNPACK_OFFSET_PTR(pData->pSequenceArrayHead, pData);
    pData->pFrameArrayHead = NNS_G2D_UNPACK_OFFSET_PTR(pData->pFrameArrayHead, pData);
    pData->pAnimContents = NNS_G2D_UNPACK_OFFSET_PTR(pData->pAnimContents, pData);

    {
        NNSG2dAnimSequenceData *pSeq = pData->pSequenceArrayHead;
        NNSG2dAnimFrameData *pFrameBase = pData->pFrameArrayHead;
        void *pContentsBase = pData->pAnimContents;

        for (i = 0; i < pData->numSequences; i++) {
            pSeq[i].pAnmFrameArray = NNS_G2D_UNPACK_OFFSET_PTR(pSeq[i].pAnmFrameArray, pFrameBase);

            for (j = 0; j < pSeq[i].numFrames; j++) {
                pSeq[i].pAnmFrameArray[j].pContent =
                    NNS_G2D_UNPACK_OFFSET_PTR(pSeq[i].pAnmFrameArray[j].pContent, pContentsBase);
            }
        }
    }

    if (pData->pExtendedData != NULL) {
        pData->pExtendedData = NNS_G2D_UNPACK_OFFSET_PTR(pData->pExtendedData, pData);
        {
            u32 i = 0;
            u32 j = 0;
            NNSG2dUserExDataBlock *pExBlk = (NNSG2dUserExDataBlock *)pData->pExtendedData;
            NNSG2dUserExAnimAttrBank *pAnmExAttrBank = (NNSG2dUserExAnimAttrBank *)(pExBlk + 1);

            pAnmExAttrBank->pAnmSeqAttrArray =
                NNS_G2D_UNPACK_OFFSET_PTR(pAnmExAttrBank->pAnmSeqAttrArray, pAnmExAttrBank);

            for (i = 0; i < pAnmExAttrBank->numSequences; i++) {
                NNSG2dUserExAnimSequenceAttr *pSeqAttr = &pAnmExAttrBank->pAnmSeqAttrArray[i];

                pSeqAttr->pAttr = NNS_G2D_UNPACK_OFFSET_PTR(pSeqAttr->pAttr, pAnmExAttrBank);
                pSeqAttr->pAnmFrmAttrArray = NNS_G2D_UNPACK_OFFSET_PTR(pSeqAttr->pAnmFrmAttrArray, pAnmExAttrBank);
                for (j = 0; j < pSeqAttr->numFrames; j++) {
                    NNSG2dUserExAnimFrameAttr *pFrm = &pSeqAttr->pAnmFrmAttrArray[j];
                    pFrm->pAttr = NNS_G2D_UNPACK_OFFSET_PTR(pFrm->pAttr, pAnmExAttrBank);
                }
            }
        }
    }
}

const NNSG2dAnimSequenceData *NNS_G2dGetAnimSequenceByIdx(const NNSG2dAnimBankData *pAnimBank, u16 idx) {
    if (pAnimBank->numSequences > idx) {
        return &pAnimBank->pSequenceArrayHead[idx];
    } else {
        return NULL;
    }
}

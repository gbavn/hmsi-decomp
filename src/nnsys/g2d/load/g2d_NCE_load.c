#include "nnsys/g2d/load/g2d_NCE_load.h"
#include "nnsys/g2d/g2d_Load.h"

static void *GetPtrOamArrayHead(const NNSG2dCellDataBank *pCellBank) {
    if (NNS_G2dCellBankHasBR(pCellBank)) {
        return (void *)((NNSG2dCellDataWithBR *)pCellBank->pCellDataArrayHead + pCellBank->numCells);
    } else {
        return (void *)(pCellBank->pCellDataArrayHead + pCellBank->numCells);
    }
}

static void UnpackExtendedData(void *pExData) {
    NNSi_G2dUnpackUserExCellAttrBank((NNSG2dUserExCellAttrBank *)((NNSG2dUserExDataBlock *)pExData + 1));
}

BOOL NNS_G2dGetUnpackedCellBank(void *pNcerFile, NNSG2dCellDataBank **ppCellBank) {
    NNSG2dBinaryFileHeader *pBinFile = (NNSG2dBinaryFileHeader *)pNcerFile;
    NNSG2dCellDataBankBlock *pCellBankBlk =
        (NNSG2dCellDataBankBlock *)NNS_G2dFindBinaryBlock(pBinFile, NNS_G2D_BLKSIG_CELLBANK);

    if (pCellBankBlk) {
        NNS_G2dUnpackNCE(&pCellBankBlk->cellDataBank);
        *ppCellBank = &pCellBankBlk->cellDataBank;
        return TRUE;
    }

    *ppCellBank = NULL;
    return FALSE;
}

const NNSG2dCellData *NNS_G2dGetCellDataByIdx(const NNSG2dCellDataBank *pCellData, u16 idx) {
    if (idx >= pCellData->numCells) {
        return NULL;
    }
    if (NNS_G2dCellBankHasBR(pCellData)) {
        return (const NNSG2dCellData *)&((NNSG2dCellDataWithBR *)pCellData->pCellDataArrayHead)[idx];
    } else {
        return &pCellData->pCellDataArrayHead[idx];
    }
}

void NNS_G2dUnpackNCE(NNSG2dCellDataBank *pCellData) {
    u16 i;

    pCellData->pCellDataArrayHead = NNS_G2D_UNPACK_OFFSET_PTR(pCellData->pCellDataArrayHead, pCellData);

    {
        void *pOamArrayHead = GetPtrOamArrayHead(pCellData);

        for (i = 0; i < pCellData->numCells; i++) {
            NNSG2dCellData *pCell = (NNSG2dCellData *)NNS_G2dGetCellDataByIdx(pCellData, i);
            pCell->pOamAttrArray = NNS_G2D_UNPACK_OFFSET_PTR(pCell->pOamAttrArray, pOamArrayHead);
        }
    }

    if (pCellData->pVramTransferData != NULL) {
        NNSG2dVramTransferData *pVramTransferData =
            NNS_G2D_UNPACK_OFFSET_PTR(pCellData->pVramTransferData, pCellData);
        pCellData->pVramTransferData = pVramTransferData;
        pVramTransferData->pCellTransferDataArray =
            NNS_G2D_UNPACK_OFFSET_PTR(pVramTransferData->pCellTransferDataArray, pVramTransferData);
        pCellData->pVramTransferData = pVramTransferData;
    }

    if (pCellData->pExtendedData != NULL) {
        pCellData->pExtendedData = NNS_G2D_UNPACK_OFFSET_PTR(pCellData->pExtendedData, pCellData);
        UnpackExtendedData(pCellData->pExtendedData);
    }
}

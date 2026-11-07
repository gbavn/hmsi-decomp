#include "nnsys/g2d/load/g2d_NCG_load.h"
#include "nnsys/g2d/g2d_Load.h"

BOOL NNS_G2dGetUnpackedCharacterData(void *pNcgrFile, NNSG2dCharacterData **ppCharData) {
    NNSG2dBinaryFileHeader *pBinFile = (NNSG2dBinaryFileHeader *)pNcgrFile;
    NNSG2dCharacterDataBlock *pBinBlk =
        (NNSG2dCharacterDataBlock *)NNS_G2dFindBinaryBlock(pBinFile, NNS_G2D_BINBLK_SIG_CHARACTERDATA);

    if (pBinBlk) {
        NNS_G2dUnpackNCG(&pBinBlk->characterData);
        *ppCharData = &pBinBlk->characterData;
        return TRUE;
    } else {
        *ppCharData = NULL;
        return FALSE;
    }
}

void NNS_G2dUnpackNCG(NNSG2dCharacterData *pCharData) {
    pCharData->pRawData = NNS_G2D_UNPACK_OFFSET_PTR(pCharData->pRawData, pCharData);
}

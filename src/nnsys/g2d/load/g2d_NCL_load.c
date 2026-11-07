#include "nnsys/g2d/load/g2d_NCL_load.h"
#include "nnsys/g2d/g2d_Load.h"

BOOL NNS_G2dGetUnpackedPaletteData(void *pNclrFile, NNSG2dPaletteData **ppPltData) {
    NNSG2dBinaryFileHeader *pBinFile = (NNSG2dBinaryFileHeader *)pNclrFile;
    NNSG2dPaletteDataBlock *pBinBlk =
        (NNSG2dPaletteDataBlock *)NNS_G2dFindBinaryBlock(pBinFile, NNS_G2D_BINBLK_SIG_PALETTEDATA);

    if (pBinBlk) {
        NNS_G2dUnpackNCL(&pBinBlk->paletteData);
        *ppPltData = &pBinBlk->paletteData;
        return TRUE;
    } else {
        *ppPltData = NULL;
        return FALSE;
    }
}

BOOL NNS_G2dGetUnpackedPaletteCompressInfo(void *pNclrFile, NNSG2dPaletteCompressInfo **ppPltCmpInfo) {
    NNSG2dBinaryFileHeader *pBinFile = (NNSG2dBinaryFileHeader *)pNclrFile;
    NNSG2dPaletteCompressDataBlock *pBinBlk =
        (NNSG2dPaletteCompressDataBlock *)NNS_G2dFindBinaryBlock(pBinFile, NNS_G2D_BINBLK_SIG_PALETTECOMPINFO);

    if (pBinBlk) {
        NNSi_G2dUnpackNCLCmpInfo(&pBinBlk->plttCmpInfo);
        *ppPltCmpInfo = &pBinBlk->plttCmpInfo;
        return TRUE;
    } else {
        *ppPltCmpInfo = NULL;
        return FALSE;
    }
}

void NNSi_G2dUnpackNCLCmpInfo(NNSG2dPaletteCompressInfo *pPlttCmpInfo) {
    pPlttCmpInfo->pPlttIdxTbl = NNS_G2D_UNPACK_OFFSET_PTR(pPlttCmpInfo->pPlttIdxTbl, pPlttCmpInfo);
}

void NNS_G2dUnpackNCL(NNSG2dPaletteData *pPlttData) {
    pPlttData->pRawData = NNS_G2D_UNPACK_OFFSET_PTR(pPlttData->pRawData, pPlttData);
}

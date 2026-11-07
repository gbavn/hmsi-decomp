#ifndef NNSYS_G2D_LOAD_NCE_LOAD_H
#define NNSYS_G2D_LOAD_NCE_LOAD_H

#include "nnsys/g2d/fmt/g2d_Cell_data.h"

BOOL NNS_G2dGetUnpackedCellBank(void *pNcerFile, NNSG2dCellDataBank **ppCellBank);
void NNS_G2dUnpackNCE(NNSG2dCellDataBank *pCellData);
const NNSG2dCellData *NNS_G2dGetCellDataByIdx(const NNSG2dCellDataBank *pCellData, u16 idx);

#endif

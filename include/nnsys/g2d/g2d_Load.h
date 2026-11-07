#ifndef NNSYS_G2D_LOAD_H
#define NNSYS_G2D_LOAD_H

#include "nnsys/g2d/fmt/g2d_Common_data.h"
#include "nnsys/g2d/fmt/g2d_Cell_data.h"

NNSG2dBinaryBlockHeader *NNS_G2dFindBinaryBlock(NNSG2dBinaryFileHeader *pBinFileHeader, u32 signature);
void NNSi_G2dUnpackUserExCellAttrBank(NNSG2dUserExCellAttrBank *pCellAttrBank);

#endif

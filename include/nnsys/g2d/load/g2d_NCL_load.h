#ifndef NNSYS_G2D_LOAD_NCL_LOAD_H
#define NNSYS_G2D_LOAD_NCL_LOAD_H

#include "nnsys/g2d/fmt/g2d_Character_data.h"

BOOL NNS_G2dGetUnpackedPaletteData(void *pNclrFile, NNSG2dPaletteData **ppPltData);
BOOL NNS_G2dGetUnpackedPaletteCompressInfo(void *pNclrFile, NNSG2dPaletteCompressInfo **ppPltCmpInfo);
void NNSi_G2dUnpackNCLCmpInfo(NNSG2dPaletteCompressInfo *pPlttCmpInfo);
void NNS_G2dUnpackNCL(NNSG2dPaletteData *pPlttData);

#endif

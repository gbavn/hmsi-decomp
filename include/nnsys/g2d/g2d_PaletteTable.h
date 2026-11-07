#ifndef NNSYS_G2D_PALETTETABLE_H
#define NNSYS_G2D_PALETTETABLE_H

#include "types.h"

#define NNS_G2D_NUM_COLOR_PALETTE 16

typedef struct NNSG2dPaletteSwapTable {
    u16 paletteIndex[NNS_G2D_NUM_COLOR_PALETTE];
} NNSG2dPaletteSwapTable;

u16 NNS_G2dGetPaletteTableValue(const NNSG2dPaletteSwapTable *pPlttTbl, u16 beforeIdx);

#endif

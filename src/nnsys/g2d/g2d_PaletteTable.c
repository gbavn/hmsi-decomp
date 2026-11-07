#include "nnsys/g2d/g2d_PaletteTable.h"

u16 NNS_G2dGetPaletteTableValue(const NNSG2dPaletteSwapTable *pPlttTbl, u16 beforeIdx) {
    const u16 afterIdx = pPlttTbl->paletteIndex[beforeIdx];
    return afterIdx;
}

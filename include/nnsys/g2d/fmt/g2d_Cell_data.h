#ifndef NNSYS_G2D_FMT_CELL_DATA_H
#define NNSYS_G2D_FMT_CELL_DATA_H

#include "nnsys/g2d/fmt/g2d_Common_data.h"

#define NNS_G2D_BLKSIG_CELLBANK (u32)('CEBK')
#define NNS_G2D_CELLBK_ATTR_CELLWITHBR 0x1

typedef struct NNSG2dCellVramTransferData {
    u32 srcDataOffset;
    u32 szByte;
} NNSG2dCellVramTransferData;

typedef struct NNSG2dVramTransferData {
    u32 szByteMax;
    NNSG2dCellVramTransferData *pCellTransferDataArray;
} NNSG2dVramTransferData;

typedef struct NNSG2dCellOAMAttrData {
    u16 attr0;
    u16 attr1;
    u16 attr2;
} NNSG2dCellOAMAttrData;

typedef struct NNSG2dCellBoundingRectS16 {
    s16 maxX;
    s16 maxY;
    s16 minX;
    s16 minY;
} NNSG2dCellBoundingRectS16;

typedef struct NNSG2dCellData {
    u16 numOAMAttrs;
    u16 cellAttr;
    NNSG2dCellOAMAttrData *pOamAttrArray;
} NNSG2dCellData;

typedef struct NNSG2dCellDataWithBR {
    NNSG2dCellData cellData;
    NNSG2dCellBoundingRectS16 boundingRect;
} NNSG2dCellDataWithBR;

typedef struct NNSG2dCellDataBank {
    u16 numCells;
    u16 cellBankAttr;
    NNSG2dCellData *pCellDataArrayHead;
    u32 mappingMode;
    NNSG2dVramTransferData *pVramTransferData;
    void *pStringBank;
    void *pExtendedData;
} NNSG2dCellDataBank;

typedef struct NNSG2dCellDataBankBlock {
    NNSG2dBinaryBlockHeader blockHeader;
    NNSG2dCellDataBank cellDataBank;
} NNSG2dCellDataBankBlock;

typedef struct NNSG2dUserExCellAttr {
    u32 *pAttr;
} NNSG2dUserExCellAttr;

typedef struct NNSG2dUserExCellAttrBank {
    u16 numCells;
    u16 numAttribute;
    NNSG2dUserExCellAttr *pCellAttrArray;
} NNSG2dUserExCellAttrBank;

static inline BOOL NNS_G2dCellBankHasBR(const NNSG2dCellDataBank *pCellBank) {
    return (pCellBank->cellBankAttr & NNS_G2D_CELLBK_ATTR_CELLWITHBR);
}

#endif

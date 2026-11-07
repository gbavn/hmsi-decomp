#ifndef NNSYS_G2D_FMT_CHARACTER_DATA_H
#define NNSYS_G2D_FMT_CHARACTER_DATA_H

#include "nnsys/g2d/fmt/g2d_Common_data.h"

#define NNS_G2D_BINBLK_SIG_CHARACTERDATA (u32)('CHAR')
#define NNS_G2D_BINBLK_SIG_PALETTEDATA (u32)('PLTT')
#define NNS_G2D_BINBLK_SIG_PALETTECOMPINFO (u32)('PCMP')

typedef struct NNSG2dCharacterData {
    u16 H;
    u16 W;
    u32 pixelFmt;
    u32 mappingType;
    u32 characterFmt;
    u32 szByte;
    void *pRawData;
} NNSG2dCharacterData;

typedef struct NNSG2dCharacterDataBlock {
    NNSG2dBinaryBlockHeader blockHeader;
    NNSG2dCharacterData characterData;
} NNSG2dCharacterDataBlock;

typedef struct NNSG2dPaletteCompressInfo {
    u16 numPalette;
    u16 pad16;
    void *pPlttIdxTbl;
} NNSG2dPaletteCompressInfo;

typedef struct NNSG2dPaletteData {
    u32 fmt;
    BOOL bExtendedPlt;
    u32 szByte;
    void *pRawData;
} NNSG2dPaletteData;

typedef struct NNSG2dPaletteCompressDataBlock {
    NNSG2dBinaryBlockHeader blockHeader;
    NNSG2dPaletteCompressInfo plttCmpInfo;
} NNSG2dPaletteCompressDataBlock;

typedef struct NNSG2dPaletteDataBlock {
    NNSG2dBinaryBlockHeader blockHeader;
    NNSG2dPaletteData paletteData;
} NNSG2dPaletteDataBlock;

#endif

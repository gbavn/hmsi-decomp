#ifndef NNSYS_G2D_FMT_COMMON_DATA_H
#define NNSYS_G2D_FMT_COMMON_DATA_H

#include "types.h"

#define NNS_G2D_UNPACK_OFFSET_PTR(ptr, base) (void *)((u32)(ptr) + (u32)(base))

typedef struct NNSG2dBinaryFileHeader {
    u32 signature;
    u16 byteOrder;
    u16 version;
    u32 fileSize;
    u16 headerSize;
    u16 dataBlocks;
} NNSG2dBinaryFileHeader;

typedef struct NNSG2dBinaryBlockHeader {
    u32 kind;
    u32 size;
} NNSG2dBinaryBlockHeader;

typedef struct NNSG2dUserExDataBlock {
    u32 blkTypeID;
    u32 blkSize;
} NNSG2dUserExDataBlock;

#endif

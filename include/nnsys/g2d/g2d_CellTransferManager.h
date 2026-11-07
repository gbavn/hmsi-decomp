#ifndef NNSYS_G2D_CELLTRANSFERMANAGER_H
#define NNSYS_G2D_CELLTRANSFERMANAGER_H

#include "nnsys/g2d/g2d_Image.h"
#include "nnsys/gfd/gfd_VramTransferManager.h"

#define NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE (u32)0xFFFFFFFF
#define NNS_G2D_VRAM_ADDR_NONE 0xFFFFFFFF

typedef BOOL (*VramTransferTaskRegisterFuncPtr)(NNS_GFD_DST_TYPE type, u32 dstAddr, void *pSrc, u32 szByte);

typedef struct NNSG2dCellTransferState {
    NNSG2dVRamLocation dstVramLocation;
    u32 szDst;
    const void *pSrcNCGR;
    const void *pSrcNCBR;
    u32 szSrcData;
    BOOL bActive;
    u32 bDrawn;
    u32 bTransferRequested;
    u32 srcOffset;
    u32 szByte;
} NNSG2dCellTransferState;

void NNS_G2dInitCellTransferStateManager(NNSG2dCellTransferState *pCellStateArray, u32 numCellState,
                                         VramTransferTaskRegisterFuncPtr pTaskRegisterFunc);
u32 NNS_G2dGetNewCellTransferStateHandle(void);
void NNS_G2dFreeCellTransferStateHandle(u32 handle);
void NNS_G2dUpdateCellTransferStateManager(void);
void NNS_G2dSetCellTransferStateRequested(u32 handle, u32 srcOffset, u32 szByte);
NNSG2dCellTransferState *NNSi_G2dGetCellTransferState(u32 handle);
void NNSi_G2dInitCellTransferState(u32 handle, u32 dstAddr3D, u32 dstAddr2DMain, u32 dstAddr2DSub, u32 szDst,
                                   const void *pSrcNCGR, const void *pSrcNCBR, u32 szSrcData);

static inline void NNSi_G2dSetCellTransferStateRequestFlag(NNSG2dCellTransferState *pState, NNS_G2D_VRAM_TYPE type,
                                                           BOOL flag) {
    pState->bTransferRequested = (pState->bTransferRequested & ~(0x1 << type)) | (flag << type);
}

static inline BOOL NNSi_G2dGetCellTransferStateRequestFlag(const NNSG2dCellTransferState *pState,
                                                           NNS_G2D_VRAM_TYPE type) {
    return (BOOL)(pState->bTransferRequested & (0x1 << type));
}

static inline void NNSi_G2dSetCellTransferStateCellDrawnFlag(NNSG2dCellTransferState *pState, NNS_G2D_VRAM_TYPE type,
                                                             BOOL flag) {
    pState->bDrawn = (pState->bDrawn & ~(0x1 << type)) | (flag << type);
}

static inline BOOL NNSi_G2dGetCellTransferStateCellDrawnFlag(const NNSG2dCellTransferState *pState,
                                                             NNS_G2D_VRAM_TYPE type) {
    return (BOOL)(pState->bDrawn & (0x1 << type));
}

static inline void NNSi_G2dSetVramTransferCellDrawnFlag(u32 handle, NNS_G2D_VRAM_TYPE type, BOOL flag) {
    NNSG2dCellTransferState *pState = NNSi_G2dGetCellTransferState(handle);
    NNSi_G2dSetCellTransferStateCellDrawnFlag(pState, type, flag);
}

#endif

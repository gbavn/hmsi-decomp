#include "nnsys/g2d/g2d_CellTransferManager.h"

static NNSG2dCellTransferState *s_pCellStateArray = NULL;
static u32 s_numCellState = 0;
VramTransferTaskRegisterFuncPtr s_pTaskRegisterFunc = NULL;

static inline NNSG2dCellTransferState *GetValidCellTransferState_(u32 validHandle) {
    return &s_pCellStateArray[validHandle];
}

static inline BOOL ShouldRegisterAsVramTransferTask_(const NNSG2dCellTransferState *pState, NNS_G2D_VRAM_TYPE type) {
    return (BOOL)(NNSi_G2dGetCellTransferStateRequestFlag(pState, type) &&
                  NNSi_G2dGetCellTransferStateCellDrawnFlag(pState, type));
}

static inline void ResetCellTransferStateDrawnFlag_(NNSG2dCellTransferState *pState) {
    pState->bDrawn = 0x0;
}

static inline const void *GetVramTransferSrc_(const NNSG2dCellTransferState *pState, NNS_G2D_VRAM_TYPE type) {
    if (type == NNS_G2D_VRAM_TYPE_3DMAIN) {
        return pState->pSrcNCBR;
    } else {
        return pState->pSrcNCGR;
    }
}

static inline NNS_GFD_DST_TYPE ConvertVramType_(NNS_G2D_VRAM_TYPE type) {
    const static NNS_GFD_DST_TYPE cvtTbl[] = {
        NNS_GFD_DST_3D_TEX_VRAM,
        NNS_GFD_DST_2D_OBJ_CHAR_MAIN,
        NNS_GFD_DST_2D_OBJ_CHAR_SUB,
    };
    return cvtTbl[type];
}

static inline BOOL MakeVramTransferTask_(const NNSG2dCellTransferState *pState, NNS_G2D_VRAM_TYPE type) {
    const NNS_GFD_DST_TYPE dstType = ConvertVramType_(type);
    return (*s_pTaskRegisterFunc)(dstType, NNSi_G2dGetVramLocation(&pState->dstVramLocation, type),
                                  (u8 *)GetVramTransferSrc_(pState, type) + pState->srcOffset, pState->szByte);
}

static inline void ResetCellTransferState_(NNSG2dCellTransferState *pState) {
    NNSi_G2dInitializeVRamLocation(&pState->dstVramLocation);

    pState->szDst = 0;
    pState->pSrcNCGR = NULL;
    pState->pSrcNCBR = NULL;
    pState->szSrcData = 0;
    pState->bActive = FALSE;

    pState->bDrawn = 0x0;
    pState->bTransferRequested = 0x0;
    pState->srcOffset = 0x0;
    pState->szByte = 0x0;
}

NNSG2dCellTransferState *NNSi_G2dGetCellTransferState(u32 handle) {
    return GetValidCellTransferState_(handle);
}

void NNSi_G2dInitCellTransferState(u32 handle, u32 dstAddr3D, u32 dstAddr2DMain, u32 dstAddr2DSub, u32 szDst,
                                   const void *pSrcNCGR, const void *pSrcNCBR, u32 szSrcData) {
    NNSG2dCellTransferState *pState = GetValidCellTransferState_(handle);

    {
        NNSG2dVRamLocation *pImg = &pState->dstVramLocation;

        NNSi_G2dInitializeVRamLocation(pImg);

        if (dstAddr3D != NNS_G2D_VRAM_ADDR_NONE) {
            NNSi_G2dSetVramLocation(pImg, NNS_G2D_VRAM_TYPE_3DMAIN, dstAddr3D);
        }
        if (dstAddr2DMain != NNS_G2D_VRAM_ADDR_NONE) {
            NNSi_G2dSetVramLocation(pImg, NNS_G2D_VRAM_TYPE_2DMAIN, dstAddr2DMain);
        }
        if (dstAddr2DSub != NNS_G2D_VRAM_ADDR_NONE) {
            NNSi_G2dSetVramLocation(pImg, NNS_G2D_VRAM_TYPE_2DSUB, dstAddr2DSub);
        }
    }

    pState->szDst = szDst;
    pState->pSrcNCGR = pSrcNCGR;
    pState->pSrcNCBR = pSrcNCBR;
    pState->szSrcData = szSrcData;
}

void NNS_G2dInitCellTransferStateManager(NNSG2dCellTransferState *pCellStateArray, u32 numCellState,
                                         VramTransferTaskRegisterFuncPtr pTaskRegisterFunc) {
    s_pTaskRegisterFunc = pTaskRegisterFunc;
    s_pCellStateArray = pCellStateArray;
    s_numCellState = numCellState;

    {
        u32 i;
        for (i = 0; i < numCellState; i++) {
            ResetCellTransferState_(&pCellStateArray[i]);
        }
    }
}

void NNS_G2dUpdateCellTransferStateManager(void) {
    u32 i;

    for (i = 0; i < s_numCellState; i++) {
        NNS_G2D_VRAM_TYPE type;
        NNSG2dCellTransferState *pState = &s_pCellStateArray[i];

        if (pState->bActive) {
            for (type = NNS_G2D_VRAM_TYPE_3DMAIN; type < NNS_G2D_VRAM_TYPE_MAX; type++) {
                if (ShouldRegisterAsVramTransferTask_(pState, type)) {
                    if (MakeVramTransferTask_(pState, type)) {
                        NNSi_G2dSetCellTransferStateRequestFlag(pState, type, FALSE);
                    }
                }
            }
            ResetCellTransferStateDrawnFlag_(pState);
        }
    }
}

void NNS_G2dSetCellTransferStateRequested(u32 handle, u32 srcOffset, u32 szByte) {
    NNSG2dCellTransferState *pState = NNSi_G2dGetCellTransferState(handle);

    pState->bTransferRequested = 0xFFFFFFFF;
    pState->srcOffset = srcOffset;
    pState->szByte = szByte;
}

u32 NNS_G2dGetNewCellTransferStateHandle(void) {
    u32 i = 0;

    for (i = 0; i < s_numCellState; i++) {
        if (s_pCellStateArray[i].bActive != TRUE) {
            s_pCellStateArray[i].bActive = TRUE;
            return i;
        }
    }
    return NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE;
}

void NNS_G2dFreeCellTransferStateHandle(u32 handle) {
    ResetCellTransferState_(GetValidCellTransferState_(handle));
}

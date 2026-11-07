#ifndef NNSYS_G2D_OAM_H
#define NNSYS_G2D_OAM_H

#include "nitro/gx/gx_oam.h"

#define NNS_G2D_OAM_AFFINE_IDX_NONE 0xFFFE

typedef enum NNSG2dOamType {
    NNS_G2D_OAMTYPE_MAIN = 0,
    NNS_G2D_OAMTYPE_SUB,
    NNS_G2D_OAMTYPE_SOFTWAREEMULATION,
    NNS_G2D_OAMTYPE_INVALID,
    NNS_G2D_OAMTYPE_MAX
} NNSG2dOamType;

typedef struct NNSG2dOAMManageArea {
    u16 fromIdx;
    u16 toIdx;
    u16 currentIdx;
} NNSG2dOAMManageArea;

typedef struct NNSG2dOamManagerInstance {
    NNSG2dOamType type;
    NNSG2dOAMManageArea managedAttrArea;
    NNSG2dOAMManageArea managedAffineArea;
    u16 GUID;
    u16 pad16;
    BOOL bFastTransferEnable;
    fx32 spriteZoffsetStep;
} NNSG2dOamManagerInstance;

void NNS_G2dInitOamManagerModule(void);
BOOL NNS_G2dGetNewManagerInstance(NNSG2dOamManagerInstance *pMan, u16 from, u16 to, NNSG2dOamType type);
BOOL NNS_G2dEntryOamManagerOamWithAffineIdx(NNSG2dOamManagerInstance *pMan, const GXOamAttr *pOam, u16 affineIdx);
u16 NNS_G2dEntryOamManagerAffine(NNSG2dOamManagerInstance *pMan, const MtxFx22 *mtx);
void NNS_G2dApplyOamManagerToHW(NNSG2dOamManagerInstance *pMan);
void NNS_G2dResetOamManagerBuffer(NNSG2dOamManagerInstance *pMan);

#endif

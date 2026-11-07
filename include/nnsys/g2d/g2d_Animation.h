#ifndef NNSYS_G2D_ANIMATION_H
#define NNSYS_G2D_ANIMATION_H

#include "nitro/fx/fx.h"
#include "nnsys/g2d/fmt/g2d_Anim_data.h"

typedef NNSG2dAnimFrameData NNSG2dAnimFrame;
typedef NNSG2dAnimSequenceData NNSG2dAnimSequence;

typedef enum NNSG2dAnmCallbackType {
    NNS_G2D_ANMCALLBACKTYPE_NONE = 0,
    NNS_G2D_ANMCALLBACKTYPE_LAST_FRM,
    NNS_G2D_ANMCALLBACKTYPE_SPEC_FRM,
    NNS_G2D_ANMCALLBACKTYPE_EVER_FRM,
    AnmCallbackType_MAX
} NNSG2dAnmCallbackType;

typedef void (*NNSG2dAnmCallBackPtr)(u32 param, fx32 currentFrame);

typedef struct NNSG2dCallBackFunctor {
    NNSG2dAnmCallbackType type;
    u32 param;
    NNSG2dAnmCallBackPtr pFunc;
    u16 frameIdx;
    u16 pad16;
} NNSG2dCallBackFunctor;

typedef struct NNSG2dAnimController {
    const NNSG2dAnimFrame *pCurrent;
    const NNSG2dAnimFrame *pActiveCurrent;
    BOOL bReverse;
    BOOL bActive;
    fx32 currentTime;
    fx32 speed;
    NNSG2dAnimationPlayMode overriddenPlayMode;
    const NNSG2dAnimSequence *pAnimSequence;
    NNSG2dCallBackFunctor callbackFunctor;
} NNSG2dAnimController;

void *NNS_G2dGetAnimCtrlCurrentElement(NNSG2dAnimController *pAnimCtrl);
BOOL NNS_G2dTickAnimCtrl(NNSG2dAnimController *pAnimCtrl, fx32 frames);
BOOL NNS_G2dSetAnimCtrlCurrentFrameNoResetCurrentTime(NNSG2dAnimController *pAnimCtrl, u16 index);
u16 NNS_G2dGetAnimCtrlCurrentFrame(const NNSG2dAnimController *pAnimCtrl);
void NNS_G2dInitAnimCtrl(NNSG2dAnimController *pAnimCtrl);
void NNS_G2dInitAnimCallBackFunctor(NNSG2dCallBackFunctor *pCallBack);
void NNS_G2dResetAnimCtrlState(NNSG2dAnimController *pAnimCtrl);
void NNS_G2dBindAnimCtrl(NNSG2dAnimController *pAnimCtrl, const NNSG2dAnimSequence *pAnimSequence);

static inline void NNS_G2dStopAnimCtrl(NNSG2dAnimController *pAnimCtrl) {
    pAnimCtrl->bActive = FALSE;
}

#endif

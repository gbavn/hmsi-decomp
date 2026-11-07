#include "nnsys/g2d/g2d_Animation.h"
#include "nitro/fx/fx_inline.h"
#include <stdlib.h>

static inline const NNSG2dAnimFrame *GetFrameBegin(const NNSG2dAnimSequence *pSequence) {
    return pSequence->pAnmFrameArray;
}

static inline const NNSG2dAnimFrame *GetFrameEnd(const NNSG2dAnimSequence *pSequence) {
    return pSequence->pAnmFrameArray + (pSequence->numFrames);
}

static inline const NNSG2dAnimFrame *GetFrameLoopBegin(const NNSG2dAnimSequence *pSequence) {
    return pSequence->pAnmFrameArray + pSequence->loopStartFrameIdx;
}

static inline u16 GetCurrentFrameIdx(const NNSG2dAnimController *pAnimCtrl) {
    return (u16)(((u32)pAnimCtrl->pCurrent - (u32)pAnimCtrl->pAnimSequence->pAnmFrameArray) /
                 sizeof(NNSG2dAnimFrameData));
}

static inline NNSG2dAnimationPlayMode GetAnimationPlayMode(const NNSG2dAnimController *pAnimCtrl) {
    if (pAnimCtrl->overriddenPlayMode != NNS_G2D_ANIMATIONPLAYMODE_INVALID) {
        return pAnimCtrl->overriddenPlayMode;
    } else {
        return pAnimCtrl->pAnimSequence->playMode;
    }
}

static inline BOOL IsLoopAnimSequence(const NNSG2dAnimController *pAnimCtrl) {
    const NNSG2dAnimationPlayMode playMode = GetAnimationPlayMode(pAnimCtrl);
    return (playMode == NNS_G2D_ANIMATIONPLAYMODE_FORWARD_LOOP ||
            playMode == NNS_G2D_ANIMATIONPLAYMODE_REVERSE_LOOP) ? TRUE : FALSE;
}

static inline BOOL IsReversePlayAnim(const NNSG2dAnimController *pAnimCtrl) {
    const NNSG2dAnimationPlayMode playMode = GetAnimationPlayMode(pAnimCtrl);
    return (playMode == NNS_G2D_ANIMATIONPLAYMODE_REVERSE ||
            playMode == NNS_G2D_ANIMATIONPLAYMODE_REVERSE_LOOP) ? TRUE : FALSE;
}

static inline BOOL IsAnimCtrlMovingForward(const NNSG2dAnimController *pAnimCtrl) {
    return (pAnimCtrl->speed > 0) ^ (pAnimCtrl->bReverse) ? TRUE : FALSE;
}

static inline BOOL ShouldAnmCtrlMoveNext(NNSG2dAnimController *pAnimCtrl) {
    if (pAnimCtrl->bActive && (pAnimCtrl->currentTime >= FX32_ONE * (int)pAnimCtrl->pCurrent->frames)) {
        return TRUE;
    }
    return FALSE;
}

static inline void CallbackFuncHandling(const NNSG2dCallBackFunctor *pFunctor, u16 currentFrameIdx) {
    switch (pFunctor->type) {
    case NNS_G2D_ANMCALLBACKTYPE_SPEC_FRM:
        if (currentFrameIdx == pFunctor->frameIdx) {
            (*pFunctor->pFunc)(pFunctor->param, currentFrameIdx);
        }
        break;
    case NNS_G2D_ANMCALLBACKTYPE_EVER_FRM:
        (*pFunctor->pFunc)(pFunctor->param, currentFrameIdx);
        break;
    }
}

static inline BOOL IsReachStartEdge(const NNSG2dAnimController *pAnimCtrl, const NNSG2dAnimFrame *pFrame) {
    return (pFrame <= (GetFrameLoopBegin(pAnimCtrl->pAnimSequence) - 1)) ? TRUE : FALSE;
}

static inline BOOL IsReachEdge(const NNSG2dAnimController *pAnimCtrl, const NNSG2dAnimFrame *pFrame) {
    if (IsAnimCtrlMovingForward(pAnimCtrl)) {
        return (pFrame >= GetFrameEnd(pAnimCtrl->pAnimSequence)) ? TRUE : FALSE;
    } else {
        return IsReachStartEdge(pAnimCtrl, pFrame);
    }
}

static inline void MoveNext(NNSG2dAnimController *pAnimCtrl) {
    if (IsAnimCtrlMovingForward(pAnimCtrl)) {
        pAnimCtrl->pCurrent++;
    } else {
        pAnimCtrl->pCurrent--;
    }
}

static inline void SequenceEdgeHandleCommon(NNSG2dAnimController *pAnimCtrl) {
    if (pAnimCtrl->callbackFunctor.type == NNS_G2D_ANMCALLBACKTYPE_LAST_FRM) {
        (*pAnimCtrl->callbackFunctor.pFunc)(pAnimCtrl->callbackFunctor.param, pAnimCtrl->currentTime);
    }

    if (!IsLoopAnimSequence(pAnimCtrl)) {
        NNS_G2dStopAnimCtrl(pAnimCtrl);
    } else {
        NNS_G2dResetAnimCtrlState(pAnimCtrl);
    }
}

static inline void SequenceEdgeHandleReverse(NNSG2dAnimController *pAnimCtrl) {
    pAnimCtrl->bReverse = pAnimCtrl->bReverse ^ TRUE;

    if (IsReachStartEdge(pAnimCtrl, pAnimCtrl->pCurrent)) {
        SequenceEdgeHandleCommon(pAnimCtrl);
    }
}

static inline void SequenceEdgeHandleNormal(NNSG2dAnimController *pAnimCtrl) {
    SequenceEdgeHandleCommon(pAnimCtrl);
}

static inline void ValidateAnimFrame(NNSG2dAnimController *pAnimCtrl, const NNSG2dAnimFrame **pFrame) {
    if (*pFrame > GetFrameEnd(pAnimCtrl->pAnimSequence) - 1) {
        *pFrame = GetFrameEnd(pAnimCtrl->pAnimSequence) - 1;
    } else if (*pFrame < GetFrameBegin(pAnimCtrl->pAnimSequence)) {
        *pFrame = GetFrameBegin(pAnimCtrl->pAnimSequence);
    }
}

static void SequenceEdgeHandle(NNSG2dAnimController *pAnimCtrl) {
    if (IsReversePlayAnim(pAnimCtrl)) {
        SequenceEdgeHandleReverse(pAnimCtrl);
    } else {
        SequenceEdgeHandleNormal(pAnimCtrl);
    }
    ValidateAnimFrame(pAnimCtrl, &pAnimCtrl->pCurrent);
}

static BOOL SetAnimCtrlCurrentFrameImpl(NNSG2dAnimController *pAnimCtrl, u16 index) {
    if (index < pAnimCtrl->pAnimSequence->numFrames) {
        pAnimCtrl->pCurrent = &pAnimCtrl->pAnimSequence->pAnmFrameArray[index];
        if (pAnimCtrl->pCurrent->frames != 0) {
            pAnimCtrl->pActiveCurrent = pAnimCtrl->pCurrent;
        }
        return TRUE;
    }
    return FALSE;
}

void *NNS_G2dGetAnimCtrlCurrentElement(NNSG2dAnimController *pAnimCtrl) {
    return (void *)pAnimCtrl->pActiveCurrent->pContent;
}

BOOL NNS_G2dTickAnimCtrl(NNSG2dAnimController *pAnimCtrl, fx32 frames) {
    BOOL bChangeFrame = FALSE;

    if (pAnimCtrl->bActive != TRUE) {
        return FALSE;
    }

    pAnimCtrl->currentTime += abs(FX_Mul(pAnimCtrl->speed, frames));

    while (ShouldAnmCtrlMoveNext(pAnimCtrl)) {
        bChangeFrame = TRUE;

        pAnimCtrl->currentTime -= FX32_ONE * (int)pAnimCtrl->pCurrent->frames;

        MoveNext(pAnimCtrl);

        if (IsReachEdge(pAnimCtrl, pAnimCtrl->pCurrent)) {
            SequenceEdgeHandle(pAnimCtrl);
        }

        if (pAnimCtrl->pCurrent->frames != 0) {
            pAnimCtrl->pActiveCurrent = pAnimCtrl->pCurrent;
        }

        if (pAnimCtrl->callbackFunctor.type != NNS_G2D_ANMCALLBACKTYPE_NONE) {
            CallbackFuncHandling(&pAnimCtrl->callbackFunctor, GetCurrentFrameIdx(pAnimCtrl));
        }
    }

    return bChangeFrame;
}

BOOL NNS_G2dSetAnimCtrlCurrentFrameNoResetCurrentTime(NNSG2dAnimController *pAnimCtrl, u16 index) {
    return SetAnimCtrlCurrentFrameImpl(pAnimCtrl, index);
}

u16 NNS_G2dGetAnimCtrlCurrentFrame(const NNSG2dAnimController *pAnimCtrl) {
    return GetCurrentFrameIdx(pAnimCtrl);
}

void NNS_G2dInitAnimCtrl(NNSG2dAnimController *pAnimCtrl) {
    NNS_G2dInitAnimCallBackFunctor(&pAnimCtrl->callbackFunctor);

    pAnimCtrl->pCurrent = NULL;
    pAnimCtrl->pActiveCurrent = NULL;

    pAnimCtrl->bReverse = FALSE;
    pAnimCtrl->bActive = TRUE;

    pAnimCtrl->currentTime = 0;
    pAnimCtrl->speed = FX32_ONE;

    pAnimCtrl->overriddenPlayMode = NNS_G2D_ANIMATIONPLAYMODE_INVALID;
    pAnimCtrl->pAnimSequence = NULL;
}

void NNS_G2dInitAnimCallBackFunctor(NNSG2dCallBackFunctor *pCallBack) {
    pCallBack->type = NNS_G2D_ANMCALLBACKTYPE_NONE;
    pCallBack->param = 0x0;
    pCallBack->pFunc = NULL;
    pCallBack->frameIdx = 0;
}

void NNS_G2dResetAnimCtrlState(NNSG2dAnimController *pAnimCtrl) {
    if (IsAnimCtrlMovingForward(pAnimCtrl)) {
        pAnimCtrl->pCurrent = GetFrameLoopBegin(pAnimCtrl->pAnimSequence);
    } else {
        pAnimCtrl->pCurrent = GetFrameEnd(pAnimCtrl->pAnimSequence) - 1;
    }

    pAnimCtrl->pActiveCurrent = pAnimCtrl->pCurrent;
    pAnimCtrl->currentTime = 0;

    (void)NNS_G2dTickAnimCtrl(pAnimCtrl, 0);
}

void NNS_G2dBindAnimCtrl(NNSG2dAnimController *pAnimCtrl, const NNSG2dAnimSequence *pAnimSequence) {
    pAnimCtrl->pAnimSequence = pAnimSequence;
    NNS_G2dResetAnimCtrlState(pAnimCtrl);
}

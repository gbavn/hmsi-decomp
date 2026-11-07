#ifndef NNSYS_G2D_SRTCONTROL_H
#define NNSYS_G2D_SRTCONTROL_H

#include "nitro/fx/fx.h"

typedef struct NNSG2dFVec2 {
    fx32 x;
    fx32 y;
} NNSG2dFVec2;

typedef struct NNSG2dSVec2 {
    s16 x;
    s16 y;
} NNSG2dSVec2;

typedef struct {
    fx32 _00, _01, _10, _11, _20, _21;
} MtxFx32;

typedef enum {
    NNS_G2D_SRTCONTROLTYPE_INVALID,
    NNS_G2D_SRTCONTROLTYPE_SRT,
    NNS_G2D_SRTCONTROLTYPE_MTX2D,
    NNS_G2D_SRTCONTROLTYPE_MTX3D,
    NNS_G2D_SRTCONTROLTYPE_MAX
} NNSG2dSRTControlType;

typedef enum NNSG2dAffineEnable {
    NNS_G2D_AFFINEENABLE_NONE = 0x00,
    NNS_G2D_AFFINEENABLE_SCALE = 0x02,
    NNS_G2D_AFFINEENABLE_ROTATE = 0x04,
    NNS_G2D_AFFINEENABLE_TRANS = 0x08,
    NNS_G2D_AFFINEENABLE_MAX = 0x10
} NNSG2dAffineEnable;

typedef union {
    struct {
        NNSG2dFVec2 scale;
        NNSG2dSVec2 trans;
        u16 rotZ;
        u16 SRT_EnableFlag;
    };
    MtxFx32 mtx;
} NNSG2dSRTData;

typedef struct {
    NNSG2dSRTControlType type;
    NNSG2dSRTData srtData;
} NNSG2dSRTControl;

static inline void NNSi_G2dSrtcAffineFlagON(NNSG2dSRTControl *pCtrl, NNSG2dAffineEnable flag) {
    pCtrl->srtData.SRT_EnableFlag |= flag;
}

void NNSi_G2dSrtcSetTrans(NNSG2dSRTControl *pCtrl, s16 x, s16 y);
void NNSi_G2dSrtcSetSRTRotZ(NNSG2dSRTControl *pCtrl, u16 rotZ);
void NNSi_G2dSrtcSetSRTScale(NNSG2dSRTControl *pCtrl, fx32 x, fx32 y);
void NNSi_G2dSrtcInitControl(NNSG2dSRTControl *pCtrl, NNSG2dSRTControlType type);
void NNSi_G2dSrtcSetInitialValue(NNSG2dSRTControl *pCtrl);

#endif

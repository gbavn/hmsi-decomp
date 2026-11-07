#include "nnsys/g2d/g2d_SRTControl.h"
#include "nitro/mi/memory.h"

void NNSi_G2dSrtcSetTrans(NNSG2dSRTControl *pCtrl, s16 x, s16 y) {
    if (pCtrl->type == NNS_G2D_SRTCONTROLTYPE_SRT) {
        NNSi_G2dSrtcAffineFlagON(pCtrl, NNS_G2D_AFFINEENABLE_TRANS);
        pCtrl->srtData.trans.x = x;
        pCtrl->srtData.trans.y = y;
    }
}

void NNSi_G2dSrtcSetSRTRotZ(NNSG2dSRTControl *pCtrl, u16 rotZ) {
    if (pCtrl->type == NNS_G2D_SRTCONTROLTYPE_SRT) {
        NNSi_G2dSrtcAffineFlagON(pCtrl, NNS_G2D_AFFINEENABLE_ROTATE);
        pCtrl->srtData.rotZ = rotZ;
    }
}

void NNSi_G2dSrtcSetSRTScale(NNSG2dSRTControl *pCtrl, fx32 x, fx32 y) {
    if (pCtrl->type == NNS_G2D_SRTCONTROLTYPE_SRT) {
        NNSi_G2dSrtcAffineFlagON(pCtrl, NNS_G2D_AFFINEENABLE_SCALE);
        pCtrl->srtData.scale.x = x;
        pCtrl->srtData.scale.y = y;
    }
}

void NNSi_G2dSrtcInitControl(NNSG2dSRTControl *pCtrl, NNSG2dSRTControlType type) {
    pCtrl->type = type;
    NNSi_G2dSrtcSetInitialValue(pCtrl);
}

void NNSi_G2dSrtcSetInitialValue(NNSG2dSRTControl *pCtrl) {
    MI_CpuFill16(&pCtrl->srtData, 0, sizeof(NNSG2dSRTData));
    pCtrl->srtData.scale.x = FX32_ONE;
    pCtrl->srtData.scale.y = FX32_ONE;
}

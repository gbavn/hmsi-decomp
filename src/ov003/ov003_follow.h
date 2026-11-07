#ifndef OV003_FOLLOW_H
#define OV003_FOLLOW_H

#include <nitro/fx/fx.h>

class IslandCamera;

// Camera state shared with the joint callbacks.
struct FollowCam {
    u8 high;
    u8 low;
    u8 dirty;
    f32 ofsZ;
    f32 ofsX;
    IslandCamera *camera;
    f32 ofsY;
    VecFx32 pos;
    VecFx32 target;
    VecFx32 prevPos;
    VecFx32 delta;
};

// The callbacks' state: in the original probably two statics of IslandView.cpp (see IslandViewData.cpp).
struct FollowState {
    FollowCam cam;
    MtxFx33 jointRot;
};

extern FollowState sFollowState;

#endif

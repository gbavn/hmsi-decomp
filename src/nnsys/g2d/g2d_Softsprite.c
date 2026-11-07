#include "nitro/gx/g3.h"

void NNS_G2dSetupSoftwareSpriteCamera(void) {
    G3_ViewPort(0, 0, 255, 191);
    {
        {
            G3_OrthoW(FX32_ONE * 0, FX32_ONE * 192, FX32_ONE * 0, FX32_ONE * 256,
                      FX32_ONE * -1024, FX32_ONE * 1024, FX32_ONE * 1024, NULL);
            G3_StoreMtx(0);
        }
        {
            VecFx32 Eye = { 0, 0, 0 };
            VecFx32 vUp = { 0, FX32_ONE, 0 };
            VecFx32 at = { 0, 0, -FX32_ONE };

            G3_LookAt(&Eye, &vUp, &at, NULL);
        }
        G3_MtxMode(GX_MTXMODE_POSITION);
    }
}

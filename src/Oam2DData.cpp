// The OAM manager used by Oam2D.cpp's callbacks. In the original a static of Oam2D.cpp placed before
// the vblank_count of <nitro/gx/gx.h>; our mwcc puts vblank_count first. Layout workaround.
struct NNSG2dOamManagerInstance;

NNSG2dOamManagerInstance *gOamManager;

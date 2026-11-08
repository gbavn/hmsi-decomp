// The OAM registration callbacks of the 2D render surfaces (ARM9 0x0202bed0..0x0202bf9c).
#include <nnsys/g2d.h>

// gOamManager: defined in Oam2DData.cpp (layout workaround, see there).
extern NNSG2dOamManagerInstance *gOamManager;

static BOOL RegisterOam(const GXOamAttr *oam, u16 affineIdx, BOOL doubleAffine)
{
    return NNS_G2dEntryOamManagerOamWithAffineIdx(gOamManager, oam, affineIdx);
}

static u16 RegisterAffine(const MtxFx22 *mtx)
{
    return NNS_G2dEntryOamManagerAffine(gOamManager, mtx);
}

extern "C" void func_0202bf10(NNSG2dRendererInstance *renderer, NNSG2dRenderSurface *surface,
                              const NNSG2dImageProxy *image, const NNSG2dImagePaletteProxy *palette)
{
    NNS_G2dInitRenderer(renderer);
    NNS_G2dInitRenderSurface(surface);
    surface->viewRect.posTopLeft.x = 0;
    surface->viewRect.posTopLeft.y = 0;
    surface->viewRect.sizeView.x = FX32_CONST(256);
    surface->viewRect.sizeView.y = FX32_CONST(192);
    NNS_G2dAddRendererTargetSurface(renderer, surface);
    NNS_G2dSetRendererImageProxy(renderer, image, palette);
    surface->pFuncOamRegister = RegisterOam;
    surface->pFuncOamAffineRegister = RegisterAffine;
    surface->type = NNS_G2D_SURFACETYPE_MAIN3D;
}

extern "C" void func_0202bf8c(NNSG2dOamManagerInstance *oam)
{
    gOamManager = oam;
}

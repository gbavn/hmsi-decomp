// Overlay 3, part 1: the island scene task.
#include <nitro/gx.h>
#include "ov003_island.h"

IslandScene::IslandScene()
    : mView(NULL)
{
    mNextScene = 9;
    InitGraphics();
    func_02035ed4(0, 0);
    func_02035f30();
    func_02035e44(0x18);
    func_02035e8c(0x18, 0);
    mCamera.Setup();
    func_02037238();
}

#ifdef NONMATCHING
IslandScene::~IslandScene()
{
    func_02036048(0);
    func_0202bf9c();
    if (mView != NULL) {
        delete mView;
    }
    func_02029fe8(data_020ec870);
    func_0202bbf4(gRenderer2D);
    gRenderer3D->Begin();
    gSceneControl->Request(mNextScene);
}
#else
// The original has the deleting destructor (D0) before the complete one (D1); mwcc always emits D1
// first, so both are kept as assembly under their linker names, in ROM order.
extern "C" {
void _ZN12IslandCameraD1Ev(void);
void _ZN4TaskD2Ev(void);
void _ZN4TaskdlEPv(void);
void _ZN10Renderer3D5BeginEv(void);
void _ZN12SceneControl7RequestEi(void);
void _ZN11IslandSceneD1Ev(void);
void _ZN11IslandSceneD0Ev(void);
void _ZN11IslandScene6UpdateEv(void);
void _ZN11IslandScene4DrawEv(void);
void _ZN11IslandScene5Func4Ev(void);

// With the destructor in asm mwcc no longer emits the vtable, so it is spelled out here.
void *_ZTV11IslandScene[7] = {
    NULL, NULL, (void *)_ZN11IslandSceneD1Ev, (void *)_ZN11IslandSceneD0Ev, (void *)_ZN11IslandScene6UpdateEv,
    (void *)_ZN11IslandScene4DrawEv, (void *)_ZN11IslandScene5Func4Ev,
};
}
extern "C" asm void _ZN11IslandSceneD0Ev(void) {
    stmdb sp!, {r4, lr}
    ldr r1, [pc, #0x7c]
    mov r4, r0
    str r1, [r4, #0x0]
    mov r0, #0x0
    bl func_02036048
    bl func_0202bf9c
    ldr r0, [r4, #0x14]
    cmp r0, #0x0
    beq _02155d24
    ldr r1, [r0, #0x0]
    ldr r1, [r1, #0x4]
    blx r1
_02155d24:
    ldr r0, [pc, #0x50]
    ldr r0, [r0, #0x0]
    bl func_02029fe8
    ldr r0, [pc, #0x48]
    ldr r0, [r0, #0x0]
    bl func_0202bbf4
    ldr r0, [pc, #0x40]
    ldr r0, [r0, #0x0]
    bl _ZN10Renderer3D5BeginEv
    ldr r0, [pc, #0x38]
    ldr r1, [r4, #0x110]
    ldr r0, [r0, #0x0]
    bl _ZN12SceneControl7RequestEi
    add r0, r4, #0x18
    bl _ZN12IslandCameraD1Ev
    mov r0, r4
    bl _ZN4TaskD2Ev
    mov r0, r4
    bl _ZN4TaskdlEPv
    mov r0, r4
    ldmia sp!, {r4, pc}
    dcd _ZTV11IslandScene+8
    dcd data_020ec870
    dcd gRenderer2D
    dcd gRenderer3D
    dcd gSceneControl
}

extern "C" asm void _ZN11IslandSceneD1Ev(void) {
    stmdb sp!, {r4, lr}
    ldr r1, [pc, #0x74]
    mov r4, r0
    str r1, [r4, #0x0]
    mov r0, #0x0
    bl func_02036048
    bl func_0202bf9c
    ldr r0, [r4, #0x14]
    cmp r0, #0x0
    beq _02155dc0
    ldr r1, [r0, #0x0]
    ldr r1, [r1, #0x4]
    blx r1
_02155dc0:
    ldr r0, [pc, #0x48]
    ldr r0, [r0, #0x0]
    bl func_02029fe8
    ldr r0, [pc, #0x40]
    ldr r0, [r0, #0x0]
    bl func_0202bbf4
    ldr r0, [pc, #0x38]
    ldr r0, [r0, #0x0]
    bl _ZN10Renderer3D5BeginEv
    ldr r0, [pc, #0x30]
    ldr r1, [r4, #0x110]
    ldr r0, [r0, #0x0]
    bl _ZN12SceneControl7RequestEi
    add r0, r4, #0x18
    bl _ZN12IslandCameraD1Ev
    mov r0, r4
    bl _ZN4TaskD2Ev
    mov r0, r4
    ldmia sp!, {r4, pc}
    dcd _ZTV11IslandScene+8
    dcd data_020ec870
    dcd gRenderer2D
    dcd gRenderer3D
    dcd gSceneControl
}
#endif

BOOL IslandScene::Update()
{
    if (mView == NULL) {
        GX_SetMasterBrightness(-16);
        GXS_SetMasterBrightness(-16);
        mView = new IslandView(&mCamera, &mCamera);
        func_02037284(1, 1, 1);
    }
    if (!func_020374f8(1) && !func_020374f8(1)) {
        mFading = FALSE;
    }
    if (IsDone() && !mFading) {
        return FALSE;
    }
    BeginFrame();
    if (mView != NULL && !mView->Update()) {
        StartFadeOut();
        gRenderer3D->Begin();
        func_0203357c(data_020f57c0, 0x1D5, 1);
        mNextScene = 1;
    }
    func_02037410();
    Func0215612c();
    return TRUE;
}

void IslandScene::StartFadeOut()
{
    func_02037284(6, 1, 1);
    mFading = TRUE;
}

void IslandScene::Draw()
{
    SetupLight();
    mCamera.Apply();
    if (!IsDone()) {
        if (mView != NULL) {
            mView->Draw();
        }
    } else {
        gRenderer3D->Begin();
    }
    gRenderer3D->Flush();
}

void IslandScene::Func4()
{
}

void *IslandScene::operator new(size_t size)
{
    return Task::operator new(size, data_020f578c.mSceneManager);
}

void IslandScene::InitGraphics()
{
    GX_DisableBankForBG();
    GX_DisableBankForOBJ();
    GX_DisableBankForBGExtPltt();
    GX_DisableBankForOBJExtPltt();
    GX_DisableBankForTex();
    GX_DisableBankForTexPltt();
    GX_DisableBankForClearImage();
    FX_Init();
    NNS_G3dInit();
    Gfx_Dummy0();
    Gfx_InitMain3D();
    GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);
    OS_WaitVBlankIntr();
}

void IslandScene::BeginFrame()
{
    gRenderer3D->Begin();
    G3X_Reset();
    G3X_ResetMtxStack();
    func_02035d88();
}

void IslandScene::SetupLight()
{
    G3_MaterialColorDiffAmb(GX_RGB(31, 31, 31), GX_RGB(31, 31, 31), TRUE);
    G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16), GX_RGB(0, 0, 0), FALSE);
    G3_PolygonAttr(GX_LIGHTMASK_0123, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 0, 31,
                   GX_POLYGON_ATTR_MISC_FAR_CLIPPING | GX_POLYGON_ATTR_MISC_DISP_1DOT);
    G3_Begin(GX_BEGIN_TRIANGLES);
    G3_End();
    NNS_G3dGlbLightVector(GX_LIGHTID_0, 0, -FX32_ONE, 0);
    NNS_G3dGlbLightVector(GX_LIGHTID_1, 0, -FX32_ONE, -FX32_ONE);
    NNS_G3dGlbLightVector(GX_LIGHTID_2, FX32_ONE - 1, FX32_ONE - 1, -FX32_ONE);
    NNS_G3dGlbLightVector(GX_LIGHTID_3, -FX32_ONE, FX32_ONE - 1, -FX32_ONE);
    NNS_G3dGlbLightColor(GX_LIGHTID_0, GX_RGB(31, 31, 31));
    NNS_G3dGlbLightColor(GX_LIGHTID_1, GX_RGB(31, 31, 31));
    NNS_G3dGlbLightColor(GX_LIGHTID_2, GX_RGB(31, 31, 31));
    NNS_G3dGlbLightColor(GX_LIGHTID_3, GX_RGB(31, 31, 31));
}

BOOL IslandScene::IsDone()
{
    return mNextScene != 9;
}

void IslandScene::Func0215612c()
{
}

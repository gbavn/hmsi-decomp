// Overlay 8, part 1: the sheep-shearing scene (camera, light, task managers). Names are ours.
#include <nitro/fx/fx_trig.h>
#include <nitro/gx.h>
#include "ov008_sheep.h"
#include "input.h"

// Defined in SheepSceneManagers.cpp (layout workaround, see there).
extern TaskManager *sTaskManagers[3];


void Gfx_Dummy0(void);       // 0x0202837c
void Gfx_InitMain3D(void);   // 0x02028384
void Gfx_Dummy1(void);       // 0x02028380
void Gfx_InitSub(void);      // 0x02028424


#ifdef NONMATCHING
// Matches except that the original addressed the (then file-static) manager array through two
// literal-pool entries; see SheepSceneManagers.cpp.
SheepScene::SheepScene()
{
    int i;

    InitGraphics();
    for (i = 0; i < 2; i++) {
        sTaskManagers[i] = NULL;
    }
    sTaskManagers[0] = new TaskManager(sizeof(SheepModel), 1);
    new (sTaskManagers[0]) SheepModel;
    sTaskManagers[1] = new TaskManager(sizeof(SheepWool), 16);

    mFovySin = 0x579;
    mFovyCos = 0xF09;
    mAspect = 0x1555;
    mNear = FX32_ONE;
    mFar = 0x190000;
    mCamDist = 0x30000;
    mCamTarget.x = 0;
    mCamTarget.y = 0x1800;
    mCamTarget.z = 0;
    mCamAngle = 0;
    unk9A = 0;
    unk9C = 0;
    {
        fx32 cos = FX_CosIdx(mCamAngle);
        fx32 sin = FX_SinIdx(mCamAngle);

        mCamPos.x = cos * (mCamDist >> FX32_SHIFT);
        mCamPos.y = sin * (mCamDist >> FX32_SHIFT);
        mCamPos.z = cos * (mCamDist >> FX32_SHIFT);
    }
    mCamPos.x = 0;
    mCamPos.y = 0x6000;
    mCamPos.z = 0xC000;
    mCamUp.x = 0;
    mCamUp.y = FX32_ONE;
    mCamUp.z = 0;
}
#else
// Linker names of the functions/data the asm below refers to.
extern "C" {
void _ZN10SheepModelC1Ev(void);
void _ZN10SheepScene12InitGraphicsEv(void);
void _ZN11TaskManagerC1Emi(void);
void _ZN4TaskC2Ev(void);
void _ZN4TasknwEmP11TaskManager(void);
void _Znwm(void);
extern char _ZTV10SheepScene[];
}
// NONMATCHING: kept as assembly until the C version matches.
asm SheepScene::SheepScene() {
    stmdb sp!, {r4, lr}
    mov r4, r0
    bl _ZN4TaskC2Ev
    ldr r1, [pc, #0x150]
    mov r0, r4
    str r1, [r4, #0x0]
    bl _ZN10SheepScene12InitGraphicsEv
    mov r2, #0x0
    ldr r0, [pc, #0x140]
    mov r1, r2
_02155ca8:
    str r1, [r0, r2, lsl #0x2]
    add r2, r2, #0x1
    cmp r2, #0x2
    blt _02155ca8
    mov r0, #0x1c
    bl _Znwm
    movs r1, r0
    beq _02155cd8
    mov r1, #0x18
    mov r2, #0x1
    bl _ZN11TaskManagerC1Emi
    mov r1, r0
_02155cd8:
    ldr r2, [pc, #0x10c]
    mov r0, #0x18
    str r1, [r2, #0x0]
    bl _ZN4TasknwEmP11TaskManager
    cmp r0, #0x0
    beq _02155cf4
    bl _ZN10SheepModelC1Ev
_02155cf4:
    mov r0, #0x1c
    bl _Znwm
    cmp r0, #0x0
    beq _02155d10
    ldr r1, [pc, #0xe4]
    mov r2, #0x10
    bl _ZN11TaskManagerC1Emi
_02155d10:
    ldr r1, [pc, #0xd4]
    ldr r2, [pc, #0xd8]
    str r0, [r1, #0x4]
    str r2, [r4, #0x68]
    add r1, r2, #0x990
    ldr r0, [pc, #0xcc]
    str r1, [r4, #0x6c]
    str r0, [r4, #0x70]
    mov r2, #0x1000
    str r2, [r4, #0x74]
    mov r0, #0x190000
    str r0, [r4, #0x78]
    mov r0, #0x30000
    str r0, [r4, #0x94]
    mov r1, #0x0
    str r1, [r4, #0x20]
    mov r0, #0x1800
    str r0, [r4, #0x24]
    str r1, [r4, #0x28]
    strh r1, [r4, #0x98]
    strh r1, [r4, #0x9a]
    strh r1, [r4, #0x9c]
    ldrh r0, [r4, #0x98]
    ldr r3, [r4, #0x94]
    ldr lr, [pc, #0x84]
    mov r0, r0, asr #0x4
    mov ip, r0, lsl #0x1
    add r0, ip, #0x1
    mov r0, r0, lsl #0x1
    ldrsh r0, [lr, r0]
    mov r3, r3, asr #0xc
    mov ip, ip, lsl #0x1
    mul r3, r0, r3
    str r3, [r4, #0x14]
    ldr r3, [r4, #0x94]
    ldrsh lr, [lr, ip]
    mov r3, r3, asr #0xc
    mov ip, #0x6000
    mul r3, lr, r3
    str r3, [r4, #0x18]
    ldr lr, [r4, #0x94]
    mov r3, #0xc000
    mov lr, lr, asr #0xc
    mul lr, r0, lr
    str lr, [r4, #0x1c]
    str r1, [r4, #0x14]
    str ip, [r4, #0x18]
    str r3, [r4, #0x1c]
    str r1, [r4, #0x2c]
    str r2, [r4, #0x30]
    mov r0, r4
    str r1, [r4, #0x34]
    ldmia sp!, {r4, pc}
    dcd _ZTV10SheepScene+8
    dcd sTaskManagers
    dcd sTaskManagers
    dcd 0x444
    dcd 0x579
    dcd 0x1555
    dcd FX_SinCosTable_
}
#endif

SheepScene::~SheepScene()
{
}

BOOL SheepScene::Update()
{
    BeginFrame();
    G3_MtxMode(GX_MTXMODE_PROJECTION);
    NNS_G3dGlbPerspectiveW(mFovySin, mFovyCos, mAspect, mNear, mFar, FX32_ONE);
    NNS_G3dGlbLookAt(&mCamPos, &mCamUp, &mCamTarget);
    if (gInput.touchFlags & INPUT_TOUCH_PRESSED) {
        new (sTaskManagers[1]) SheepWool;
    }
    sTaskManagers[0]->Update();
    sTaskManagers[1]->Update();
    return TRUE;
}

void SheepScene::Draw()
{
    SetupLight();
    sTaskManagers[0]->Draw();
    sTaskManagers[1]->Draw();
    gRenderer3D->Flush();
}

void SheepScene::Func4()
{
}

void SheepScene::InitGraphics()
{
    GX_DispOff();
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
    Gfx_Dummy1();
    Gfx_InitSub();
    GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);
    mLightR = 31;
    mLightG = 31;
    mLightB = 31;
    OS_WaitVBlankIntr();
}

void SheepScene::BeginFrame()
{
    gRenderer3D->Begin();
    G3X_Reset();
    G3X_ResetMtxStack();
}

void SheepScene::SetupLight()
{
    G3_MaterialColorDiffAmb(GX_RGB(mLightR, mLightG, mLightB), GX_RGB(31, 31, 31), TRUE);
    G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16), GX_RGB(0, 0, 0), FALSE);
    G3_PolygonAttr(GX_LIGHTMASK_NONE, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 0, 31,
                   GX_POLYGON_ATTR_MISC_FAR_CLIPPING | GX_POLYGON_ATTR_MISC_DISP_1DOT);
    G3_Begin(GX_BEGIN_TRIANGLES);
    G3_End();
    NNS_G3dGlbLightVector(GX_LIGHTID_0, 0, -FX32_ONE, -FX32_ONE);
    NNS_G3dGlbLightColor(GX_LIGHTID_0, GX_RGB(mLightR, mLightG, mLightB));
    G3X_SetClearColor(GX_RGB(0, 31, 8), 31, 0x7FFF, 63, FALSE);
}

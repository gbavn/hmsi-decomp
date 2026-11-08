// Overlay 5, part 6: the title overlay's main scene.
#include <nnsys/g3d.h>
#include <nnsys/g2d/g2d_Softsprite.h>
#include "ov005_title.h"
#include "Model3D.h"

void Gfx_Dummy0(void);       // 0x0202837c
void Gfx_InitMain3D(void);   // 0x02028384
void Gfx_Dummy1(void);       // 0x02028380
void Gfx_InitSub(void);      // 0x02028424

// sTitleManagers: defined in TitleMainManagers.cpp (layout workaround, see there).

#ifdef NONMATCHING
TitleMain::TitleMain()
{
    int i;

    data_020ec698 = 0;
    func_020709bc(data_020f57c0);
    InitGraphics();
    func_02035ed4(0, 0);
    func_02037238();
    GX_SetMasterBrightness(16);
    GXS_SetMasterBrightness(16);
    for (i = 0; i < 9; i++) {
        sTitleManagers[i] = NULL;
    }
    sTitleManagers[6] = new TaskManager(0x18, 1);
    sTitleManagers[1] = new TaskManager(0x18, 1);
    sTitleManagers[0] = new TaskManager(0x24, 1);
    sTitleManagers[2] = new TaskManager(0x38, 1);
    sTitleManagers[3] = new TaskManager(0x30, 1);
    sTitleManagers[4] = new TaskManager(0x420, 1);
    sTitleManagers[5] = new TaskManager(0x2c, 1);
    sTitleManagers[7] = new TaskManager(0x20, 1);
    sTitleManagers[8] = new TaskManager(0x38, 1);
    new (sTitleManagers[6]) TitleFlow(this);
    TitleFlow::ChangeScene(0x40, 0);
    mState = 0;
    data_020f57d0 = 1;
}
#else
// Linker names used by the asm below.
extern "C" {
extern char _ZTV9TitleMain[];
void _ZN11TaskManagerC1Emi(void);
void _ZN4TaskC2Ev(void);
void _ZN4TasknwEmP11TaskManager(void);
void _ZN9TitleFlow11ChangeSceneEhh(void);
void _ZN9TitleFlowC1EP9TitleMain(void);
void _ZN9TitleMain12InitGraphicsEv(void);
void _Znwm(void);
}
// NONMATCHING: kept as assembly until the C version matches.
asm TitleMain::TitleMain() {
    stmdb sp!, {r4, lr}
    mov r4, r0
    bl _ZN4TaskC2Ev
    ldr r1, [pc, #0x1ec]
    ldr r0, [pc, #0x1ec]
    str r1, [r4, #0x0]
    ldr r0, [r0, #0x0]
    ldr r1, [pc, #0x1e4]
    mov r2, #0x0
    strb r2, [r1, #0x0]
    bl func_020709bc
    mov r0, r4
    bl _ZN9TitleMain12InitGraphicsEv
    mov r0, #0x0
    mov r1, r0
    bl func_02035ed4
    bl func_02037238
    ldr r0, [pc, #0x1c0]
    mov r1, #0x10
    bl GXx_SetMasterBrightness_
    ldr r0, [pc, #0x1b8]
    mov r1, #0x10
    bl GXx_SetMasterBrightness_
    mov r2, #0x0
    ldr r0, [pc, #0x1ac]
    mov r1, r2
_02158d88:
    str r1, [r0, r2, lsl #0x2]
    add r2, r2, #0x1
    cmp r2, #0x9
    blt _02158d88
    mov r0, #0x1c
    bl _Znwm
    cmp r0, #0x0
    beq _02158db4
    mov r1, #0x18
    mov r2, #0x1
    bl _ZN11TaskManagerC1Emi
_02158db4:
    ldr r1, [pc, #0x17c]
    str r0, [r1, #0x18]
    mov r0, #0x1c
    bl _Znwm
    cmp r0, #0x0
    beq _02158dd8
    mov r1, #0x18
    mov r2, #0x1
    bl _ZN11TaskManagerC1Emi
_02158dd8:
    ldr r1, [pc, #0x158]
    str r0, [r1, #0x4]
    mov r0, #0x1c
    bl _Znwm
    cmp r0, #0x0
    beq _02158dfc
    mov r1, #0x24
    mov r2, #0x1
    bl _ZN11TaskManagerC1Emi
_02158dfc:
    ldr r1, [pc, #0x134]
    str r0, [r1, #0x0]
    mov r0, #0x1c
    bl _Znwm
    cmp r0, #0x0
    beq _02158e20
    mov r1, #0x38
    mov r2, #0x1
    bl _ZN11TaskManagerC1Emi
_02158e20:
    ldr r1, [pc, #0x110]
    str r0, [r1, #0x8]
    mov r0, #0x1c
    bl _Znwm
    cmp r0, #0x0
    beq _02158e44
    mov r1, #0x30
    mov r2, #0x1
    bl _ZN11TaskManagerC1Emi
_02158e44:
    ldr r1, [pc, #0xec]
    str r0, [r1, #0xc]
    mov r0, #0x1c
    bl _Znwm
    cmp r0, #0x0
    beq _02158e68
    mov r1, #0x420
    mov r2, #0x1
    bl _ZN11TaskManagerC1Emi
_02158e68:
    ldr r1, [pc, #0xc8]
    str r0, [r1, #0x10]
    mov r0, #0x1c
    bl _Znwm
    cmp r0, #0x0
    beq _02158e8c
    mov r1, #0x2c
    mov r2, #0x1
    bl _ZN11TaskManagerC1Emi
_02158e8c:
    ldr r1, [pc, #0xa4]
    str r0, [r1, #0x14]
    mov r0, #0x1c
    bl _Znwm
    cmp r0, #0x0
    beq _02158eb0
    mov r1, #0x20
    mov r2, #0x1
    bl _ZN11TaskManagerC1Emi
_02158eb0:
    ldr r1, [pc, #0x80]
    str r0, [r1, #0x1c]
    mov r0, #0x1c
    bl _Znwm
    cmp r0, #0x0
    beq _02158ed4
    mov r1, #0x38
    mov r2, #0x1
    bl _ZN11TaskManagerC1Emi
_02158ed4:
    ldr r1, [pc, #0x5c]
    str r0, [r1, #0x20]
    ldr r1, [r1, #0x18]
    mov r0, #0x18
    bl _ZN4TasknwEmP11TaskManager
    cmp r0, #0x0
    beq _02158ef8
    mov r1, r4
    bl _ZN9TitleFlowC1EP9TitleMain
_02158ef8:
    mov r0, #0x40
    mov r1, #0x0
    bl _ZN9TitleFlow11ChangeSceneEhh
    mov r0, #0x0
    ldr r1, [pc, #0x2c]
    strb r0, [r4, #0x14]
    mov r2, #0x1
    mov r0, r4
    strb r2, [r1, #0x0]
    ldmia sp!, {r4, pc}
    dcd _ZTV9TitleMain+8
    dcd data_020f57c0
    dcd data_020ec698
    dcd 0x400006c
    dcd 0x400106c
    dcd sTitleManagers
    dcd sTitleManagers
    dcd data_020f57d0
}
#endif

TitleMain::~TitleMain()
{
    NNS_GfdResetLnkTexVramState();
    NNS_GfdResetLnkPlttVramState();
    NNS_GfdDumpLnkTexVramManager();
    NNS_GfdDumpLnkPlttVramManager();
}

#ifdef NONMATCHING
BOOL TitleMain::Update()
{
    int i;

    BeginFrame();
    func_02035d88();
    sTitleManagers[6]->Update();
    sTitleManagers[5]->Update();
    sTitleManagers[1]->Update();
    sTitleManagers[0]->Update();
    sTitleManagers[2]->Update();
    sTitleManagers[3]->Update();
    sTitleManagers[4]->Update();
    sTitleManagers[7]->Update();
    sTitleManagers[8]->Update();
    switch (mState) {
    case 0:
        break;
    case 1:
        if (func_020374f8(1) || func_020374f8(0)) {
            break;
        }
        for (i = 0; i < 9; i++) {
            if (sTitleManagers[i] != NULL) {
                sTitleManagers[i]->KillAll();
                sTitleManagers[i]->Update();
            }
        }
        func_02029fe8(data_020ec870);
        func_0202bbf4(gRenderer2D);
        func_02035ed4(1, 0);
        mState = 2;
        break;
    case 2:
        if (func_020374f8(1) || func_020374f8(0)) {
            break;
        }
        {
            void *obj = data_020f57c0;
            switch (mChoice) {
            case 0:
                func_02034c60(obj, 0);
                break;
            case 1:
                func_02034c60(obj, 1);
                break;
            }
            func_02034c10(obj);
        }
        gSceneControl->Request(1);
        for (i = 0; i < 9; i++) {
            if (sTitleManagers[i] != NULL) {
                delete sTitleManagers[i];
            }
        }
        data_020ec698 = 1;
        return FALSE;
    }
    func_0202bd70(gRenderer2D);
    func_02037410();
    return TRUE;
}
#else
// Linker names used by the asm below.
extern "C" {
void _ZN11TaskManager6UpdateEv(void);
void _ZN11TaskManager7KillAllEv(void);
void _ZN12SceneControl7RequestEi(void);
void _ZN9TitleMain10BeginFrameEv(void);
}
// NONMATCHING: kept as assembly until the C version matches.
asm BOOL TitleMain::Update() {
    stmdb sp!, {r4, r5, r6, lr}
    mov r4, r0
    bl _ZN9TitleMain10BeginFrameEv
    bl func_02035d88
    ldr r0, [pc, #0x1cc]
    ldr r0, [r0, #0x18]
    bl _ZN11TaskManager6UpdateEv
    ldr r0, [pc, #0x1c0]
    ldr r0, [r0, #0x14]
    bl _ZN11TaskManager6UpdateEv
    ldr r0, [pc, #0x1b4]
    ldr r0, [r0, #0x4]
    bl _ZN11TaskManager6UpdateEv
    ldr r0, [pc, #0x1a8]
    ldr r0, [r0, #0x0]
    bl _ZN11TaskManager6UpdateEv
    ldr r0, [pc, #0x19c]
    ldr r0, [r0, #0x8]
    bl _ZN11TaskManager6UpdateEv
    ldr r0, [pc, #0x190]
    ldr r0, [r0, #0xc]
    bl _ZN11TaskManager6UpdateEv
    ldr r0, [pc, #0x184]
    ldr r0, [r0, #0x10]
    bl _ZN11TaskManager6UpdateEv
    ldr r0, [pc, #0x178]
    ldr r0, [r0, #0x1c]
    bl _ZN11TaskManager6UpdateEv
    ldr r0, [pc, #0x16c]
    ldr r0, [r0, #0x20]
    bl _ZN11TaskManager6UpdateEv
    ldrb r0, [r4, #0x14]
    cmp r0, #0x0
    beq _0215917c
    cmp r0, #0x1
    beq _0215904c
    cmp r0, #0x2
    beq _021590c8
    b _0215917c
_0215904c:
    mov r0, #0x1
    bl func_020374f8
    cmp r0, #0x0
    bne _0215917c
    mov r0, #0x0
    bl func_020374f8
    cmp r0, #0x0
    bne _0215917c
    ldr r5, [pc, #0x124]
    mov r6, #0x0
_02159074:
    ldr r0, [r5, r6, lsl #0x2]
    cmp r0, #0x0
    beq _0215908c
    bl _ZN11TaskManager7KillAllEv
    ldr r0, [r5, r6, lsl #0x2]
    bl _ZN11TaskManager6UpdateEv
_0215908c:
    add r6, r6, #0x1
    cmp r6, #0x9
    blt _02159074
    ldr r0, [pc, #0xfc]
    ldr r0, [r0, #0x0]
    bl func_02029fe8
    ldr r0, [pc, #0xf4]
    ldr r0, [r0, #0x0]
    bl func_0202bbf4
    mov r0, #0x1
    mov r1, #0x0
    bl func_02035ed4
    mov r0, #0x2
    strb r0, [r4, #0x14]
    b _0215917c
_021590c8:
    mov r0, #0x1
    bl func_020374f8
    cmp r0, #0x0
    bne _0215917c
    mov r0, #0x0
    bl func_020374f8
    cmp r0, #0x0
    bne _0215917c
    ldrb r1, [r4, #0x15]
    ldr r0, [pc, #0xb0]
    cmp r1, #0x0
    ldr r4, [r0, #0x0]
    beq _02159108
    cmp r1, #0x1
    beq _02159118
    b _02159124
_02159108:
    mov r0, r4
    mov r1, #0x0
    bl func_02034c60
    b _02159124
_02159118:
    mov r0, r4
    mov r1, #0x1
    bl func_02034c60
_02159124:
    mov r0, r4
    bl func_02034c10
    ldr r0, [pc, #0x74]
    mov r1, #0x1
    ldr r0, [r0, #0x0]
    bl _ZN12SceneControl7RequestEi
    ldr r4, [pc, #0x54]
    mov r5, #0x0
_02159144:
    ldr r0, [r4, r5, lsl #0x2]
    cmp r0, #0x0
    beq _0215915c
    ldr r1, [r0, #0x0]
    ldr r1, [r1, #0x4]
    blx r1
_0215915c:
    add r5, r5, #0x1
    cmp r5, #0x9
    blt _02159144
    ldr r0, [pc, #0x3c]
    mov r1, #0x1
    strb r1, [r0, #0x0]
    mov r0, #0x0
    ldmia sp!, {r4, r5, r6, pc}
_0215917c:
    ldr r0, [pc, #0x1c]
    ldr r0, [r0, #0x0]
    bl func_0202bd70
    bl func_02037410
    mov r0, #0x1
    ldmia sp!, {r4, r5, r6, pc}
    dcd sTitleManagers
    dcd sTitleManagers
    dcd data_020ec870
    dcd gRenderer2D
    dcd data_020f57c0
    dcd gSceneControl
    dcd data_020ec698
}
#endif

void TitleMain::Draw()
{
    SetupLight();
    sTitleManagers[5]->Draw();
    sTitleManagers[6]->Draw();
    sTitleManagers[1]->Draw();
    sTitleManagers[0]->Draw();
    sTitleManagers[2]->Draw();
    sTitleManagers[3]->Draw();
    sTitleManagers[4]->Draw();
    sTitleManagers[7]->Draw();
    sTitleManagers[8]->Draw();
    NNS_G2dSetupSoftwareSpriteCamera();
    func_0202a1a0(data_020ec870, 0);
    func_0202abe4(data_020ec870);
    func_0202bdd4(gRenderer2D);
    gRenderer3D->Flush();
}

void TitleMain::Func4()
{
    sTitleManagers[6]->Func4();
    sTitleManagers[1]->Func4();
    sTitleManagers[0]->Func4();
    sTitleManagers[2]->Func4();
    sTitleManagers[3]->Func4();
    sTitleManagers[4]->Func4();
    sTitleManagers[7]->Func4();
    sTitleManagers[8]->Func4();
}

void TitleMain::InitGraphics()
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
    func_02028530();
    Gfx_Dummy1();
    Gfx_InitSub();
    GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);
    mLightR = 31;
    mLightG = 31;
    mLightB = 31;
    OS_WaitVBlankIntr();
}

void TitleMain::BeginFrame()
{
    func_02029fe8(data_020ec870);
    func_0202bbf4(gRenderer2D);
    gRenderer3D->Begin();
    G3X_Reset();
    G3X_ResetMtxStack();
}

void TitleMain::SetupLight()
{
    G3_MaterialColorDiffAmb(GX_RGB(mLightR, mLightG, mLightB), GX_RGB(31, 31, 31), TRUE);
    G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16), GX_RGB(0, 0, 0), FALSE);
    G3_PolygonAttr(GX_LIGHTMASK_NONE, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 0, 31,
                   GX_POLYGON_ATTR_MISC_FAR_CLIPPING | GX_POLYGON_ATTR_MISC_DISP_1DOT);
    G3_Begin(GX_BEGIN_TRIANGLES);
    G3_End();
    NNS_G3dGlbLightVector(GX_LIGHTID_0, 0, -FX32_ONE, -FX32_ONE);
    NNS_G3dGlbLightColor(GX_LIGHTID_0, GX_RGB(mLightR, mLightG, mLightB));
    G3X_SetClearColor(GX_RGB(31, 31, 31), 31, 0x7FFF, 63, FALSE);
}

void TitleMain::StartGame(int choice)
{
    mChoice = choice;
    mState = 1;
    func_02037284(6, 1, 1);
    func_02037284(6, 1, 0);
}

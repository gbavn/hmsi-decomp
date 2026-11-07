// Overlay 8, part 1: the sheep-shearing scene (camera, light, task managers). Names are ours.
#include <nitro/fx/fx_trig.h>
#include <nitro/gx.h>
#include "ov008_sheep.h"
#include "input.h"

static TaskManager *sTaskManagers[2];


void Gfx_Dummy0(void);       // 0x0202837c
void Gfx_InitMain3D(void);   // 0x02028384
void Gfx_Dummy1(void);       // 0x02028380
void Gfx_InitSub(void);      // 0x02028424


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

// Overlay 8, part 3: wool tufts that fly out, fall and fade. Names are ours.
#include <nitro/fx/fx_trig.h>
#include "ov008_sheep.h"
#include "input.h"

SheepWool::SheepWool()
{
    int i;

    mBase = Model3D_Load("rom/mdls/HITUJI_wool.nsbmd");
    for (i = 0; i < WOOL_COUNT; i++) {
        mParts[i] = new Model3D;
        *mParts[i] = *mBase;
        mParts[i]->trans.x = 0;
        mParts[i]->trans.y = 0x800;
        mParts[i]->trans.z = 0;
        mAlpha[i] = 0x1F000;
    }
    for (i = 0; i < WOOL_COUNT; i++) {
        int angle = RandRange(0, 0xFFFF);
        int speed = RandRange(2, 6);

        mVel[i].x = 0;
        mVel[i].y = FX32_CONST(0.2f * speed);
        mVel[i].z = 0;
        mDir[i].x = FX_SinIdx(angle) >> 1;
        mDir[i].y = 0;
        mDir[i].z = FX_CosIdx(angle) >> 1;
        mVisible[i] = TRUE;
    }
    mGravity.x = 0;
    mGravity.y = 0xA4;
    mGravity.z = 0;
}

SheepWool::~SheepWool()
{
    int i;

    Model3D_Release(mBase);
    delete[] (u8 *)mBase->unk54;
    delete mBase;
    for (i = 0; i < WOOL_COUNT; i++) {
        if (mParts[i] == NULL) {
            break;
        }
        Model3D_Release(mParts[i]);
        delete mParts[i];
    }
}

BOOL SheepWool::Update()
{
    int i;
    u8 done;

    for (i = 0; i < WOOL_COUNT; i++) {
        VEC_Add(&mParts[i]->trans, &mDir[i], &mParts[i]->trans);
        VEC_Add(&mParts[i]->trans, &mVel[i], &mParts[i]->trans);
        VEC_Subtract(&mVel[i], &mGravity, &mVel[i]);
        if (mAlpha[i] > 0) {
            mAlpha[i] -= 0x800;
        }
        if (mAlpha[i] < 0) {
            mAlpha[i] = 0;
        }
        if (mAlpha[i] < 0xF000) {
            NNS_G3dMdlSetMdlAlpha(mParts[i]->resMdl, 0, mAlpha[i] >> FX32_SHIFT);
        }
    }
    done = 0;
    for (i = 0; i < WOOL_COUNT; i++) {
        if (mAlpha[i] != 0) {
            break;
        }
        done++;
    }
    if (done == WOOL_COUNT) {
        return FALSE;
    }
    return TRUE;
}

void SheepWool::Draw()
{
    for (int i = 0; i < WOOL_COUNT; i++) {
        mVisible[i] = gRenderer3D->DrawCulled(mParts[i]);
    }
}

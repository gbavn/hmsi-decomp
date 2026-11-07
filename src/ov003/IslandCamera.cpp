// Overlay 3, part 2: the island camera (perspective, look-at, shakes).
#include <nitro/fx/fx_trig.h>
#include <nitro/gx.h>
#include "ov003_island.h"

IslandCamera::IslandCamera()
{
    unkA4 = 0;
    mShaking = 0;
}

IslandCamera::~IslandCamera()
{
}

void IslandCamera::Setup()
{
    f32 v;

    v = FX_FX64C_TO_F32(FX_SinFx64c(0x4F9));
    mFovySin = FX_F32_TO_FX32(v);
    v = FX_FX64C_TO_F32(FX_CosFx64c(0x4F9));
    mFovyCos = FX_F32_TO_FX32(v);
    mAspect = 0x1555;
    mNear = 0x14000;
    mFar = 0x3E8000;
    mDist = 0x64000;
    mTarget.x = 0;
    mTarget.y = -0x64000;
    mTarget.z = 0;
    mAngle = 0;
    unk86 = 0;
    unk88 = 0;
    {
        fx32 cos = FX_CosIdx(mAngle);
        fx32 sin = FX_SinIdx(mAngle);

        mPos.x = cos * (mDist >> FX32_SHIFT);
        mPos.y = sin * (mDist >> FX32_SHIFT);
        mPos.z = cos * (mDist >> FX32_SHIFT);
    }
    mPos.x = 0;
    mPos.y = 0x64000;
    mPos.z = 0;
    mUp.x = 0;
    mUp.y = 0;
    mUp.z = -FX32_ONE;
}

void IslandCamera::Apply()
{
    G3_MtxMode(GX_MTXMODE_PROJECTION);
    if (mShaking) {
        fx32 ofs = FX_F32_TO_FX32(mShakeRandF / 10.0f);

        mTarget.x += FX_Mul(mShakeAmp + ofs, FX32_CONST(mShakeDir));
        mPos.x += FX_Mul(mShakeAmp + ofs, FX32_CONST(mShakeDir));
        if (--mShakeCounter < 0) {
            mShakeRand = func_020358d0(20) - 10;
            mShakeRandF = mShakeRand;
            mShakeCounter = mShakePeriod;
            mShakeDir *= -1;
        }
    }
    NNS_G3dGlbPerspectiveW(mFovySin, mFovyCos, mAspect, mNear, mFar, FX32_ONE);
    NNS_G3dGlbLookAt(&mPos, &mUp, &mTarget);
}

#ifdef NONMATCHING
void IslandCamera::SetPos(const VecFx32 *pos)
{
    VecFx32 v = *pos;

    mPos = v;
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm void IslandCamera::SetPos(const VecFx32 *pos) {
    ldmia r1, {r1, r2, r3}
    stmia r0, {r1, r2, r3}
    bx lr
}
#endif

#ifdef NONMATCHING
void IslandCamera::SetTarget(const VecFx32 *target)
{
    VecFx32 v = *target;

    mTarget = v;
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm void IslandCamera::SetTarget(const VecFx32 *target) {
    ldmia r1, {r1, r2, r3}
    str r1, [r0, #0xc]
    str r2, [r0, #0x10]
    str r3, [r0, #0x14]
    bx lr
}
#endif

void IslandCamera::StartShake(fx32 amp, int period)
{
    mShaking = 1;
    mShakeAmp = amp;
    mShakeCounter = period;
    mShakePeriod = period;
    mShakeDir = -1;
    mShakeRand = func_020358d0(20) - 10;
    mShakeRandF = mShakeRand;
}

void IslandCamera::StopShake()
{
    mShaking = 0;
    unkA4 = 0;
}

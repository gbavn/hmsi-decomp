// Overlay 3, part 3: the island models, animations and camera-joint tracking.
#include <nitro/fx/fx_trig.h>
#include <string.h>
#include "ov003_island.h"
#include "ov003_strings.h"


// .rodata (mwcc orders same-size constants in reverse order of declaration).
static const VecFx32 sIsland8Target = {-0xC29C3, 0x75B5C, 0x3FB5C};
static const VecFx32 sFollowOfs = {-0xE000, 0x17000, 0x14000}; // used by FollowCallback

static VecFx32 sUnusedIsland8Target = {-0xC29C3, 0x75B5C, 0x3FB5C};

static const char *sIslandPaths[9] = {
    gIslandPathStrings.sun,    gIslandPathStrings.rice,   gIslandPathStrings.fruit,
    gIslandPathStrings.sun2,   gIslandPathStrings.goddes, gIslandPathStrings.mushroom,
    gIslandPathStrings.vol,    gIslandPathStrings.wildA,  gIslandPathStrings.hinata,
};


#ifdef NONMATCHING
void IslandView::FollowCallbackIsland8(NNSG3dRS *rs)
{
    sFollowState.cam.prevPos.x = sFollowState.cam.pos.x;
    sFollowState.cam.prevPos.y = sFollowState.cam.pos.y;
    sFollowState.cam.prevPos.z = sFollowState.cam.pos.z;
    sFollowState.cam.pos.x = rs->pJntAnmResult->trans.x + FX_F32_TO_FX32(sFollowState.cam.ofsX);
    sFollowState.cam.pos.y = rs->pJntAnmResult->trans.y - FX_F32_TO_FX32(sFollowState.cam.ofsY);
    sFollowState.cam.pos.z = rs->pJntAnmResult->trans.z - FX_F32_TO_FX32(sFollowState.cam.ofsZ);
    sFollowState.jointRot = rs->pJntAnmResult->rot;
    sFollowState.cam.delta.x = FX_F32_TO_FX32(FX_FX32_TO_F32(sFollowState.cam.prevPos.x) - FX_FX32_TO_F32(sFollowState.cam.pos.x));
    sFollowState.cam.delta.y = FX_F32_TO_FX32(FX_FX32_TO_F32(sFollowState.cam.prevPos.y) - FX_FX32_TO_F32(sFollowState.cam.pos.y));
    sFollowState.cam.delta.z = FX_F32_TO_FX32(FX_FX32_TO_F32(sFollowState.cam.prevPos.z) - FX_FX32_TO_F32(sFollowState.cam.pos.z));
    sFollowState.cam.target.x -= sFollowState.cam.delta.x;
    sFollowState.cam.target.y -= sFollowState.cam.delta.y;
    sFollowState.cam.target.z -= sFollowState.cam.delta.z;
    sFollowState.cam.dirty = TRUE;
}
#else
// Linker names used by the asm below.
extern "C" {
void _fadd(void);
void _fdiv(void);
void _ffix(void);
void _fflt(void);
void _fgr(void);
void _fmul(void);
void _fsub(void);
}
// NONMATCHING: kept as assembly until the C version matches.
asm void IslandView::FollowCallbackIsland8(NNSG3dRS *rs) {
    stmdb sp!, {r4, r5, r6, lr}
    ldr r2, [pc, #0x41c]
    mov r4, r0
    ldr r0, [r2, #0x14]
    mov r1, #0x0
    str r0, [r2, #0x2c]
    ldr r0, [r2, #0x18]
    str r0, [r2, #0x30]
    ldr r0, [r2, #0x1c]
    str r0, [r2, #0x34]
    ldr r0, [r2, #0x8]
    bl _fgr
    ldr r0, [pc, #0x3f4]
    bls _021565bc
    ldr r1, [pc, #0x3e8]
    ldr r1, [r1, #0x8]
    bl _fmul
    mov r1, r0
    mov r0, #0x3f000000
    bl _fadd
    b _021565d0
_021565bc:
    ldr r1, [pc, #0x3cc]
    ldr r1, [r1, #0x8]
    bl _fmul
    mov r1, #0x3f000000
    bl _fsub
_021565d0:
    bl _ffix
    ldr r1, [r4, #0xb4]
    ldr r2, [pc, #0x3b0]
    ldr r3, [r1, #0x4c]
    mov r1, #0x0
    add r0, r3, r0
    str r0, [r2, #0x14]
    ldr r0, [r2, #0x10]
    bl _fgr
    ldr r0, [pc, #0x398]
    bls _02156618
    ldr r1, [pc, #0x38c]
    ldr r1, [r1, #0x10]
    bl _fmul
    mov r1, r0
    mov r0, #0x3f000000
    bl _fadd
    b _0215662c
_02156618:
    ldr r1, [pc, #0x370]
    ldr r1, [r1, #0x10]
    bl _fmul
    mov r1, #0x3f000000
    bl _fsub
_0215662c:
    bl _ffix
    ldr r1, [r4, #0xb4]
    ldr r2, [pc, #0x354]
    ldr r3, [r1, #0x50]
    mov r1, #0x0
    sub r0, r3, r0
    str r0, [r2, #0x18]
    ldr r0, [r2, #0x4]
    bl _fgr
    ldr r0, [pc, #0x33c]
    bls _02156674
    ldr r1, [pc, #0x330]
    ldr r1, [r1, #0x4]
    bl _fmul
    mov r1, r0
    mov r0, #0x3f000000
    bl _fadd
    b _02156688
_02156674:
    ldr r1, [pc, #0x314]
    ldr r1, [r1, #0x4]
    bl _fmul
    mov r1, #0x3f000000
    bl _fsub
_02156688:
    bl _ffix
    ldr r1, [r4, #0xb4]
    ldr ip, [pc, #0x2f8]
    ldr r1, [r1, #0x54]
    ldr r5, [pc, #0x2f8]
    sub r0, r1, r0
    str r0, [ip, #0x1c]
    ldr r0, [r4, #0xb4]
    ldr r4, [ip, #0x14]
    add lr, r0, #0x28
    ldmia lr!, {r0, r1, r2, r3}
    stmia r5!, {r0, r1, r2, r3}
    ldmia lr!, {r0, r1, r2, r3}
    stmia r5!, {r0, r1, r2, r3}
    ldr r0, [lr, #0x0]
    str r0, [r5, #0x0]
    ldr r6, [ip, #0x2c]
    mov r0, r6
    bl _fflt
    ldr r1, [pc, #0x2b8]
    bl _fdiv
    mov r5, r0
    mov r0, r4
    bl _fflt
    ldr r1, [pc, #0x2a4]
    bl _fdiv
    mov r1, r0
    mov r0, r5
    bl _fsub
    mov r1, #0x0
    bl _fgr
    mov r0, r6
    bls _02156754
    bl _fflt
    ldr r1, [pc, #0x27c]
    bl _fdiv
    mov r5, r0
    mov r0, r4
    bl _fflt
    ldr r1, [pc, #0x268]
    bl _fdiv
    mov r1, r0
    mov r0, r5
    bl _fsub
    mov r1, r0
    ldr r0, [pc, #0x250]
    bl _fmul
    mov r1, r0
    mov r0, #0x3f000000
    bl _fadd
    b _02156794
_02156754:
    bl _fflt
    ldr r1, [pc, #0x234]
    bl _fdiv
    mov r5, r0
    mov r0, r4
    bl _fflt
    ldr r1, [pc, #0x220]
    bl _fdiv
    mov r1, r0
    mov r0, r5
    bl _fsub
    mov r1, r0
    ldr r0, [pc, #0x208]
    bl _fmul
    mov r1, #0x3f000000
    bl _fsub
_02156794:
    bl _ffix
    ldr r1, [pc, #0x1f0]
    str r0, [r1, #0x38]
    ldr r6, [r1, #0x30]
    ldr r5, [r1, #0x18]
    mov r0, r6
    bl _fflt
    ldr r1, [pc, #0x1dc]
    bl _fdiv
    mov r4, r0
    mov r0, r5
    bl _fflt
    ldr r1, [pc, #0x1c8]
    bl _fdiv
    mov r1, r0
    mov r0, r4
    bl _fsub
    mov r1, #0x0
    bl _fgr
    mov r0, r6
    bls _02156830
    bl _fflt
    ldr r1, [pc, #0x1a0]
    bl _fdiv
    mov r4, r0
    mov r0, r5
    bl _fflt
    ldr r1, [pc, #0x18c]
    bl _fdiv
    mov r1, r0
    mov r0, r4
    bl _fsub
    mov r1, r0
    ldr r0, [pc, #0x174]
    bl _fmul
    mov r1, r0
    mov r0, #0x3f000000
    bl _fadd
    b _02156870
_02156830:
    bl _fflt
    ldr r1, [pc, #0x158]
    bl _fdiv
    mov r4, r0
    mov r0, r5
    bl _fflt
    ldr r1, [pc, #0x144]
    bl _fdiv
    mov r1, r0
    mov r0, r4
    bl _fsub
    mov r1, r0
    ldr r0, [pc, #0x12c]
    bl _fmul
    mov r1, #0x3f000000
    bl _fsub
_02156870:
    bl _ffix
    ldr r1, [pc, #0x114]
    str r0, [r1, #0x3c]
    ldr r6, [r1, #0x34]
    ldr r5, [r1, #0x1c]
    mov r0, r6
    bl _fflt
    ldr r1, [pc, #0x100]
    bl _fdiv
    mov r4, r0
    mov r0, r5
    bl _fflt
    ldr r1, [pc, #0xec]
    bl _fdiv
    mov r1, r0
    mov r0, r4
    bl _fsub
    mov r1, #0x0
    bl _fgr
    mov r0, r6
    bls _0215690c
    bl _fflt
    ldr r1, [pc, #0xc4]
    bl _fdiv
    mov r4, r0
    mov r0, r5
    bl _fflt
    ldr r1, [pc, #0xb0]
    bl _fdiv
    mov r1, r0
    mov r0, r4
    bl _fsub
    mov r1, r0
    ldr r0, [pc, #0x98]
    bl _fmul
    mov r1, r0
    mov r0, #0x3f000000
    bl _fadd
    b _0215694c
_0215690c:
    bl _fflt
    ldr r1, [pc, #0x7c]
    bl _fdiv
    mov r4, r0
    mov r0, r5
    bl _fflt
    ldr r1, [pc, #0x68]
    bl _fdiv
    mov r1, r0
    mov r0, r4
    bl _fsub
    mov r1, r0
    ldr r0, [pc, #0x50]
    bl _fmul
    mov r1, #0x3f000000
    bl _fsub
_0215694c:
    bl _ffix
    ldr r1, [pc, #0x38]
    mov r2, #0x1
    str r0, [r1, #0x40]
    ldr r4, [r1, #0x20]
    ldr r3, [r1, #0x38]
    sub r3, r4, r3
    str r3, [r1, #0x20]
    ldr r4, [r1, #0x24]
    ldr r3, [r1, #0x3c]
    sub r3, r4, r3
    str r3, [r1, #0x24]
    ldr r3, [r1, #0x28]
    sub r0, r3, r0
    str r0, [r1, #0x28]
    strb r2, [r1, #0x2]
    ldmia sp!, {r4, r5, r6, pc}
    dcd sFollowState
    dcd 0x45800000
    dcd sFollowState+0x44
}
#endif

#ifdef NONMATCHING
void IslandView::FollowCallback(NNSG3dRS *rs)
{
    fx32 frame;

    sFollowState.cam.pos.x = rs->pJntAnmResult->trans.x;
    sFollowState.cam.pos.y = rs->pJntAnmResult->trans.y;
    sFollowState.cam.pos.z = rs->pJntAnmResult->trans.z;
    sFollowState.jointRot = rs->pJntAnmResult->rot;
    frame = rs->pRenderObj->anmJnt->frame;
    if (frame > 0x493000) {
        sFollowState.cam.high = FALSE;
        sFollowState.cam.low = TRUE;
    } else if (frame > 0x3AC000) {
        sFollowState.cam.high = TRUE;
    } else {
        sFollowState.cam.high = FALSE;
    }
    {
        const VecFx32 ofs = {-0xE000, 0x17000, 0x14000};

        sFollowState.cam.dirty = TRUE;
        if (sFollowState.cam.high) {
            sFollowState.cam.target.x = sFollowState.cam.pos.x - ofs.x;
            sFollowState.cam.target.y = sFollowState.cam.pos.y - ofs.y;
            sFollowState.cam.target.z = sFollowState.cam.pos.z - ofs.z;
        } else if (sFollowState.cam.low) {
            sFollowState.cam.target.x = sFollowState.cam.pos.x;
            sFollowState.cam.target.y = -0x147800;
            sFollowState.cam.pos.y -= 0x64000;
            sFollowState.cam.target.z = sFollowState.cam.pos.z;
        } else {
            sFollowState.cam.target.x = sFollowState.cam.pos.x;
            sFollowState.cam.target.y = sFollowState.cam.pos.y - 0x64000;
            sFollowState.cam.target.z = sFollowState.cam.pos.z;
        }
    }
    sFollowState.cam.camera->SetPos(&sFollowState.cam.pos);
    sFollowState.cam.camera->SetTarget(&sFollowState.cam.target);
    sFollowState.cam.camera->Apply();
    sFollowState.cam.dirty = FALSE;
}
#else
// Linker names used by the asm below.
extern "C" {
void _ZN12IslandCamera5ApplyEv(void);
void _ZN12IslandCamera6SetPosEPK7VecFx32(void);
void _ZN12IslandCamera9SetTargetEPK7VecFx32(void);
}
// NONMATCHING: kept as assembly until the C version matches.
asm void IslandView::FollowCallback(NNSG3dRS *rs) {
    stmdb sp!, {r3, r4, r5, r6, lr}
    sub sp, sp, #0xc
    mov ip, r0
    ldr r0, [ip, #0xb4]
    ldr r4, [pc, #0x16c]
    ldr r0, [r0, #0x4c]
    ldr r5, [pc, #0x168]
    str r0, [r4, #0x14]
    ldr r0, [ip, #0xb4]
    ldr lr, [pc, #0x160]
    ldr r0, [r0, #0x50]
    str r0, [r4, #0x18]
    ldr r0, [ip, #0xb4]
    ldr r0, [r0, #0x54]
    str r0, [r4, #0x1c]
    ldr r0, [ip, #0xb4]
    add r6, r0, #0x28
    ldmia r6!, {r0, r1, r2, r3}
    stmia r5!, {r0, r1, r2, r3}
    ldmia r6!, {r0, r1, r2, r3}
    stmia r5!, {r0, r1, r2, r3}
    ldr r0, [r6, #0x0]
    str r0, [r5, #0x0]
    ldr r0, [ip, #0x4]
    ldr r0, [r0, #0x10]
    ldr r0, [r0, #0x0]
    cmp r0, lr
    ble _02156a20
    mov r0, #0x0
    strb r0, [r4, #0x0]
    mov r0, #0x1
    strb r0, [r4, #0x1]
    b _02156a34
_02156a20:
    cmp r0, #0x3ac000
    movgt r0, #0x1
    strgtb r0, [r4, #0x0]
    movle r0, #0x0
    strleb r0, [r4, #0x0]
_02156a34:
    ldr r0, [pc, #0xf0]
    add r3, sp, #0x0
    ldmia r0, {r0, r1, r2}
    stmia r3, {r0, r1, r2}
    ldr r0, [pc, #0xd4]
    mov r1, #0x1
    strb r1, [r0, #0x2]
    ldrb r1, [r0, #0x0]
    cmp r1, #0x0
    beq _02156a90
    ldr r2, [r0, #0x14]
    ldr r1, [sp, #0x0]
    sub r1, r2, r1
    str r1, [r0, #0x20]
    ldr r2, [r0, #0x18]
    ldr r1, [sp, #0x4]
    sub r1, r2, r1
    str r1, [r0, #0x24]
    ldr r2, [r0, #0x1c]
    ldr r1, [sp, #0x8]
    sub r1, r2, r1
    str r1, [r0, #0x28]
    b _02156ae0
_02156a90:
    ldrb r1, [r0, #0x1]
    cmp r1, #0x0
    beq _02156ac4
    ldr r2, [r0, #0x14]
    ldr r1, [pc, #0x88]
    str r2, [r0, #0x20]
    str r1, [r0, #0x24]
    ldr r1, [r0, #0x18]
    sub r1, r1, #0x64000
    str r1, [r0, #0x18]
    ldr r1, [r0, #0x1c]
    str r1, [r0, #0x28]
    b _02156ae0
_02156ac4:
    ldr r1, [r0, #0x14]
    str r1, [r0, #0x20]
    ldr r1, [r0, #0x18]
    sub r1, r1, #0x64000
    str r1, [r0, #0x24]
    ldr r1, [r0, #0x1c]
    str r1, [r0, #0x28]
_02156ae0:
    ldr r0, [pc, #0x38]
    ldr r1, [pc, #0x48]
    ldr r0, [r0, #0xc]
    bl _ZN12IslandCamera6SetPosEPK7VecFx32
    ldr r0, [pc, #0x28]
    ldr r1, [pc, #0x3c]
    ldr r0, [r0, #0xc]
    bl _ZN12IslandCamera9SetTargetEPK7VecFx32
    ldr r0, [pc, #0x18]
    ldr r0, [r0, #0xc]
    bl _ZN12IslandCamera5ApplyEv
    ldr r0, [pc, #0xc]
    mov r1, #0x0
    strb r1, [r0, #0x2]
    add sp, sp, #0xc
    ldmia sp!, {r3, r4, r5, r6, pc}
    dcd sFollowState
    dcd sFollowState+0x44
    dcd 0x493000
    dcd sFollowOfs
    dcd 0xffeb8800
    dcd sFollowState+0x14
    dcd sFollowState+0x20
}
#endif

IslandView::IslandView(IslandCamera *camera, IslandCamera *followCamera)
{
    char mdl[64];
    char tp[64];
    char ta[64];
    char ca[64];
    char ma[64];
    char va[64];
    int i;

    mFocusIdx = -1;
    unk128 = 0;
    mCamera = camera;
    mFrame = 0;
    sFollowState.cam.camera = followCamera;
    mIsland = func_0203366c(data_020f57c0, 0x316);
    mNumModels = 0;
    for (i = 0; i < 64; i++) {
        mModels[i] = NULL;
    }
    strncpy(mdl, sIslandPaths[mIsland], 64);
    strncat(mdl, gIslandViewStrings.nsbmd, 6);
    strncpy(tp, sIslandPaths[mIsland], 64);
    strncat(tp, gIslandViewStrings.nsbtp, 6);
    strncpy(ta, sIslandPaths[mIsland], 64);
    strncat(ta, gIslandViewStrings.nsbta, 6);
    strncpy(ca, sIslandPaths[mIsland], 64);
    strncat(ca, gIslandViewStrings.nsbca, 6);
    strncpy(ma, sIslandPaths[mIsland], 64);
    strncat(ma, gIslandViewStrings.nsbma, 6);
    strncpy(va, sIslandPaths[mIsland], 64);
    strncat(va, gIslandViewStrings.nsbva, 6);
    LoadResources(mdl, tp, ta, ca, ma, va);
    func_02035fc0(0xEF, 3);
    if (mIsland == 8) {
        const VecFx32 target = sIsland8Target;
        fx32 x;
        fx32 y;
        fx32 z;

        sFollowState.cam.target = target;
        sFollowState.cam.ofsX = 0.0f;
        sFollowState.cam.ofsY = 50.0f;
        sFollowState.cam.ofsZ = 50.0f;
        x = FX_F32_TO_FX32(sFollowState.cam.ofsX) + -0xCA11F;
        y = 0x9BFAE - FX_F32_TO_FX32(sFollowState.cam.ofsY);
        z = 0x5B571 - FX_F32_TO_FX32(sFollowState.cam.ofsZ);
        sFollowState.cam.pos.x = x;
        sFollowState.cam.pos.y = y;
        sFollowState.cam.pos.z = z;
    }
    sFollowState.cam.high = FALSE;
    sFollowState.cam.low = FALSE;
}

IslandView::~IslandView()
{
    int i;
    u32 j;

    for (i = 0; i < 6; i++) {
        if (mRes[i] != NULL) {
            if (i == 0) {
                NNS_G3dResDefaultRelease(mRes[i]);
            }
            delete[] (u8 *)mRes[i];
        }
    }
    for (j = 0; j < 64; j++) {
        if (mModels[j] == NULL) {
            break;
        }
        Model3D_Release(mModels[j]);
        delete mModels[j];
    }
}

BOOL IslandView::Update()
{
    if (mIsland == 8) {
        if (mFrame == 180) {
        } else if (mFrame == 300) {
            func_02036048(120);
            func_02035fc0(0xF1, 2);
            mCamera->StartShake(FX32_CONST(0.05), 1);
        } else if (mFrame == 360) {
            mCamera->StartShake(FX32_CONST(0.1), 1);
        } else if (mFrame == 450) {
            mCamera->StartShake(FX32_CONST(0.2), 1);
        } else if (mFrame == 510) {
            mCamera->StartShake(FX32_CONST(0.4), 1);
        } else if (mFrame == 570) {
            mCamera->StartShake(FX32_CONST(0.8), 1);
        } else if (mFrame == 630) {
            mCamera->StartShake(FX32_ONE, 1);
        } else if (mFrame == 1020) {
            func_02036048(8);
            mCamera->StartShake(FX32_CONST(0.8), 1);
        }
    } else {
        if (mFrame == 180) {
            func_02036048(30);
            func_02035fc0(0xF0, 1);
        } else if (mFrame == 360) {
            func_02036048(120);
            func_02035fc0(0xF1, 2);
            mCamera->StartShake(FX32_CONST(0.05), 4);
        } else if (mFrame == 450) {
            mCamera->StartShake(FX32_CONST(0.1), 4);
        } else if (mFrame == 510) {
            mCamera->StartShake(FX32_CONST(0.2), 4);
        } else if (mFrame == 570) {
            mCamera->StartShake(FX32_CONST(0.4), 4);
        } else if (mFrame == 630) {
            mCamera->StartShake(FX32_CONST(0.6), 4);
        } else if (mFrame == 900) {
            func_02036048(400);
            func_02035fc0(0xEF, 3);
            mCamera->StartShake(FX32_ONE, 4);
        } else if (mFrame == 1260) {
            mCamera->StartShake(FX32_ONE, 4);
        } else if (mFrame == 1290) {
            mCamera->StartShake(FX32_CONST(0.2), 4);
        } else if (mFrame == 1320) {
            mCamera->StartShake(FX32_CONST(0.1), 4);
        } else if (mFrame == 1350) {
            mCamera->StopShake();
            func_02036048(360);
        }
    }
    mFrame++;
    if (mIsland == 8) {
        return UpdateAnimsIsland8();
    }
    return UpdateAnims();
}

void IslandView::Draw()
{
    u32 i;

    for (i = 0; i < 64; i++) {
        if (mModels[i] == NULL) {
            return;
        }
        gRenderer3D->Draw(mModels[i]);
    }
}

#ifdef NONMATCHING
BOOL IslandView::UpdateAnimsIsland8()
{
    u32 i;
    u32 j;

    if (sFollowState.cam.dirty) {
        VecFx32 target;

        mCamera->SetPos(&sFollowState.cam.pos);
        target.x = sFollowState.cam.target.x + FX_F32_TO_FX32(sFollowState.cam.ofsX);
        target.y = sFollowState.cam.target.y - FX_F32_TO_FX32(sFollowState.cam.ofsY);
        target.z = sFollowState.cam.target.z - FX_F32_TO_FX32(sFollowState.cam.ofsZ);
        mCamera->SetTarget(&target);
        sFollowState.cam.dirty = FALSE;
    }
    for (i = 0; i < 64; i++) {
        if (mModels[i] == NULL) {
            break;
        }
        for (j = 0; j < mModels[i]->numAnims; j++) {
            if (mModels[i]->anims[j] != NULL) {
                NNSG3dAnmObj *anm;

                mModels[i]->anims[j]->anmObj->frame += FX32_ONE;
                anm = mModels[i]->anims[j]->anmObj;
                if (anm->frame >= NNS_G3dAnmObjGetNumFrame(anm)) {
                    if (i == mFocusIdx) {
                        func_02035ed4(120, 0);
                        return FALSE;
                    }
                    anm->frame = 0;
                }
            }
        }
    }
    return TRUE;
}
#else
// Linker names used by the asm below.
extern "C" {
void _ZN12IslandCamera6SetPosEPK7VecFx32(void);
void _ZN12IslandCamera9SetTargetEPK7VecFx32(void);
void _fadd(void);
void _ffix(void);
void _fgr(void);
void _fmul(void);
void _fsub(void);
}
// NONMATCHING: kept as assembly until the C version matches.
asm BOOL IslandView::UpdateAnimsIsland8() {
    stmdb sp!, {r4, r5, lr}
    sub sp, sp, #0xc
    ldr r1, [pc, #0x1fc]
    mov r4, r0
    ldrb r0, [r1, #0x2]
    cmp r0, #0x0
    beq _021573ec
    ldr r0, [r4, #0x12c]
    ldr r1, [pc, #0x1e8]
    bl _ZN12IslandCamera6SetPosEPK7VecFx32
    ldr r0, [pc, #0x1dc]
    mov r1, #0x0
    ldr r0, [r0, #0x8]
    bl _fgr
    ldr r0, [pc, #0x1d4]
    bls _021572fc
    ldr r1, [pc, #0x1c4]
    ldr r1, [r1, #0x8]
    bl _fmul
    mov r1, r0
    mov r0, #0x3f000000
    bl _fadd
    b _02157310
_021572fc:
    ldr r1, [pc, #0x1a8]
    ldr r1, [r1, #0x8]
    bl _fmul
    mov r1, #0x3f000000
    bl _fsub
_02157310:
    bl _ffix
    ldr r2, [pc, #0x190]
    mov r1, #0x0
    ldr r3, [r2, #0x20]
    add r0, r3, r0
    str r0, [sp, #0x0]
    ldr r0, [r2, #0x10]
    bl _fgr
    ldr r0, [pc, #0x17c]
    bls _02157354
    ldr r1, [pc, #0x16c]
    ldr r1, [r1, #0x10]
    bl _fmul
    mov r1, r0
    mov r0, #0x3f000000
    bl _fadd
    b _02157368
_02157354:
    ldr r1, [pc, #0x150]
    ldr r1, [r1, #0x10]
    bl _fmul
    mov r1, #0x3f000000
    bl _fsub
_02157368:
    bl _ffix
    ldr r2, [pc, #0x138]
    mov r1, #0x0
    ldr r3, [r2, #0x24]
    sub r0, r3, r0
    str r0, [sp, #0x4]
    ldr r0, [r2, #0x4]
    bl _fgr
    ldr r0, [pc, #0x124]
    bls _021573ac
    ldr r1, [pc, #0x114]
    ldr r1, [r1, #0x4]
    bl _fmul
    mov r1, r0
    mov r0, #0x3f000000
    bl _fadd
    b _021573c0
_021573ac:
    ldr r1, [pc, #0xf8]
    ldr r1, [r1, #0x4]
    bl _fmul
    mov r1, #0x3f000000
    bl _fsub
_021573c0:
    bl _ffix
    ldr r2, [pc, #0xe0]
    add r1, sp, #0x0
    ldr r2, [r2, #0x28]
    sub r0, r2, r0
    str r0, [sp, #0x8]
    ldr r0, [r4, #0x12c]
    bl _ZN12IslandCamera9SetTargetEPK7VecFx32
    ldr r0, [pc, #0xc4]
    mov r1, #0x0
    strb r1, [r0, #0x2]
_021573ec:
    mov r1, #0x0
_021573f0:
    add r0, r4, r1, lsl #0x2
    ldr r5, [r0, #0x20]
    cmp r5, #0x0
    beq _021574a0
    ldrb r3, [r5, #0xb2]
    mov r2, #0x0
    cmp r3, #0x0
    bls _02157494
    mov ip, r2
_02157414:
    add r3, r5, r2, lsl #0x2
    ldr r3, [r3, #0x60]
    cmp r3, #0x0
    beq _02157480
    ldr r5, [r3, #0x8]
    ldr r3, [r5, #0x0]
    add r3, r3, #0x1000
    str r3, [r5, #0x0]
    ldr r3, [r0, #0x20]
    add r3, r3, r2, lsl #0x2
    ldr r3, [r3, #0x60]
    ldr r5, [r3, #0x8]
    ldr r3, [r5, #0x8]
    ldr lr, [r5, #0x0]
    ldrh r3, [r3, #0x4]
    cmp lr, r3, lsl #0xc
    blt _02157480
    ldr r3, [r4, #0x124]
    cmp r1, r3
    bne _0215747c
    mov r0, #0x78
    mov r1, #0x0
    bl func_02035ed4
    add sp, sp, #0xc
    mov r0, #0x0
    ldmia sp!, {r4, r5, pc}
_0215747c:
    str ip, [r5, #0x0]
_02157480:
    ldr r5, [r0, #0x20]
    add r2, r2, #0x1
    ldrb r3, [r5, #0xb2]
    cmp r2, r3
    blo _02157414
_02157494:
    add r1, r1, #0x1
    cmp r1, #0x40
    blo _021573f0
_021574a0:
    mov r0, #0x1
    add sp, sp, #0xc
    ldmia sp!, {r4, r5, pc}
    dcd sFollowState
    dcd sFollowState+0x14
    dcd 0x45800000
}
#endif

BOOL IslandView::UpdateAnims()
{
    u32 i;
    u32 j;

    for (i = 0; i < 64; i++) {
        if (mModels[i] == NULL) {
            break;
        }
        for (j = 0; j < mModels[i]->numAnims; j++) {
            if (mModels[i]->anims[j] != NULL) {
                mModels[i]->anims[j]->anmObj->frame += FX32_ONE;
                if (mModels[i]->anims[j]->anmObj->frame == 0x494000 - FX32_ONE) {
                    mModels[i]->anims[j]->anmObj->frame = 0x494000;
                }
                if (mModels[i]->anims[j]->anmObj->frame >=
                        NNS_G3dAnmObjGetNumFrame(mModels[i]->anims[j]->anmObj) &&
                    i == mFocusIdx) {
                    func_02035ed4(120, 0);
                    return FALSE;
                }
            }
        }
    }
    return TRUE;
}

#ifdef NONMATCHING
BOOL IslandView::LoadResources(const char *mdl, const char *tp, const char *ta, const char *ca,
                               const char *ma, const char *va)
{
    int i;
    NNSG3dResMdlSet *set;
    u8 idx;
    Model3D *focus;

    for (i = 0; i < 6; i++) {
        mRes[i] = NULL;
    }
    mRes[0] = func_0202feb0(mdl);
    mRes[1] = func_0202feb0(tp);
    mRes[2] = func_0202feb0(ta);
    mRes[3] = func_0202feb0(ca);
    mRes[4] = func_0202feb0(ma);
    mRes[5] = func_0202feb0(va);
    set = NNS_G3dGetMdlSet((NNSG3dResFileHeader *)mRes[0]);
    mNumModels = set->dict.numEntry;
    for (idx = 0; idx < set->dict.numEntry; idx++) {
        Model3D *model = func_02030020(mRes[0], idx);
        const NNSG3dResName *name = NNS_G3dGetResNameByIdx(&set->dict, idx);
        void *anm;

        if (strstr((const char *)name, "camera") != NULL) {
            mFocusIdx = idx;
            focus = model;
        }
        if (mRes[1] != NULL && (anm = NNS_G3dGetAnmByName(mRes[1], name)) != NULL) {
            func_02030298(model, anm, 3);
            model->anims[model->numAnims - 1]->loop = 1;
        }
        if (mRes[2] != NULL && (anm = NNS_G3dGetAnmByName(mRes[2], name)) != NULL) {
            func_02030298(model, anm, 1);
            model->anims[model->numAnims - 1]->loop = 1;
        }
        if (mRes[3] != NULL && (anm = NNS_G3dGetAnmByName(mRes[3], name)) != NULL) {
            func_02030298(model, anm, 0);
            model->anims[model->numAnims - 1]->loop = 1;
        }
        if (mRes[4] != NULL && (anm = NNS_G3dGetAnmByName(mRes[4], name)) != NULL) {
            func_02030298(model, anm, 2);
            model->anims[model->numAnims - 1]->loop = 1;
        }
        if (mRes[5] != NULL && (anm = NNS_G3dGetAnmByName(mRes[5], name)) != NULL) {
            func_02030298(model, anm, 4);
            model->anims[model->numAnims - 1]->loop = 1;
        }
        mModels[idx] = model;
    }
    if (mFocusIdx != -1) {
        if (mIsland == 8) {
            NNS_G3dRenderObjSetCallBack(&focus->renderObj, FollowCallbackIsland8, NULL, NNS_G3D_SBC_NODEDESC,
                                        NNS_G3D_SBC_CALLBACK_TIMING_B);
        } else {
            NNS_G3dRenderObjSetCallBack(&focus->renderObj, FollowCallback, NULL, NNS_G3D_SBC_NODEDESC,
                                        NNS_G3D_SBC_CALLBACK_TIMING_B);
        }
    }
    return TRUE;
}
#else
// Linker names used by the asm below.
extern "C" {
void _ZN10IslandView14FollowCallbackEP9NNSG3dRS_(void);
void _ZN10IslandView21FollowCallbackIsland8EP9NNSG3dRS_(void);
}
// NONMATCHING: kept as assembly until the C version matches.
asm BOOL IslandView::LoadResources(const char *mdl, const char *tp, const char *ta, const char *ca,
                               const char *ma, const char *va) {
    stmdb sp!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, lr}
    mov r4, r3
    mov r3, #0x0
    mov r6, r2
    mov r10, r0
    mov r2, r3
_021575a8:
    add r0, r10, r3, lsl #0x2
    add r3, r3, #0x1
    str r2, [r0, #0x8]
    cmp r3, #0x6
    blt _021575a8
    mov r0, r1
    bl func_0202feb0
    str r0, [r10, #0x8]
    mov r0, r6
    bl func_0202feb0
    str r0, [r10, #0xc]
    mov r0, r4
    bl func_0202feb0
    str r0, [r10, #0x10]
    ldr r0, [sp, #0x28]
    bl func_0202feb0
    str r0, [r10, #0x14]
    ldr r0, [sp, #0x2c]
    bl func_0202feb0
    str r0, [r10, #0x18]
    ldr r0, [sp, #0x30]
    bl func_0202feb0
    str r0, [r10, #0x1c]
    ldr r0, [r10, #0x8]
    bl NNS_G3dGetMdlSet
    mov r6, r0
    ldrb r0, [r6, #0x9]
    mov r7, #0x0
    str r0, [r10, #0x120]
    ldrb r0, [r6, #0x9]
    cmp r0, #0x0
    bls _021577cc
    add r4, r6, #0x8
    mov r11, #0x1
_02157630:
    ldr r0, [r10, #0x8]
    mov r1, r7
    bl func_02030020
    mov r8, r0
    cmp r4, #0x0
    beq _0215766c
    ldrb r0, [r6, #0x9]
    cmp r7, r0
    bhs _0215766c
    ldrh r0, [r6, #0xe]
    add r1, r4, r0
    ldrh r0, [r1, #0x2]
    add r0, r1, r0
    add r9, r0, r7, lsl #0x4
    b _02157670
_0215766c:
    mov r9, #0x0
_02157670:
    ldr r1, [pc, #0x1ac]
    mov r0, r9
    bl strstr
    cmp r0, #0x0
    strne r7, [r10, #0x124]
    ldr r0, [r10, #0xc]
    movne r5, r8
    cmp r0, #0x0
    beq _021576c0
    mov r1, r9
    bl NNS_G3dGetAnmByName
    movs r1, r0
    beq _021576c0
    mov r0, r8
    mov r2, #0x3
    bl func_02030298
    ldrb r0, [r8, #0xb2]
    add r0, r8, r0, lsl #0x2
    ldr r0, [r0, #0x5c]
    strb r11, [r0, #0xd]
_021576c0:
    ldr r0, [r10, #0x10]
    cmp r0, #0x0
    beq _021576fc
    mov r1, r9
    bl NNS_G3dGetAnmByName
    movs r1, r0
    beq _021576fc
    mov r0, r8
    mov r2, #0x1
    bl func_02030298
    ldrb r0, [r8, #0xb2]
    add r0, r8, r0, lsl #0x2
    ldr r1, [r0, #0x5c]
    mov r0, #0x1
    strb r0, [r1, #0xd]
_021576fc:
    ldr r0, [r10, #0x14]
    cmp r0, #0x0
    beq _02157738
    mov r1, r9
    bl NNS_G3dGetAnmByName
    movs r1, r0
    beq _02157738
    mov r0, r8
    mov r2, #0x0
    bl func_02030298
    ldrb r0, [r8, #0xb2]
    add r0, r8, r0, lsl #0x2
    ldr r1, [r0, #0x5c]
    mov r0, #0x1
    strb r0, [r1, #0xd]
_02157738:
    ldr r0, [r10, #0x18]
    cmp r0, #0x0
    beq _02157774
    mov r1, r9
    bl NNS_G3dGetAnmByName
    movs r1, r0
    beq _02157774
    mov r0, r8
    mov r2, #0x2
    bl func_02030298
    ldrb r0, [r8, #0xb2]
    add r0, r8, r0, lsl #0x2
    ldr r1, [r0, #0x5c]
    mov r0, #0x1
    strb r0, [r1, #0xd]
_02157774:
    ldr r0, [r10, #0x1c]
    cmp r0, #0x0
    beq _021577b0
    mov r1, r9
    bl NNS_G3dGetAnmByName
    movs r1, r0
    beq _021577b0
    mov r0, r8
    mov r2, #0x4
    bl func_02030298
    ldrb r0, [r8, #0xb2]
    add r0, r8, r0, lsl #0x2
    ldr r1, [r0, #0x5c]
    mov r0, #0x1
    strb r0, [r1, #0xd]
_021577b0:
    add r0, r10, r7, lsl #0x2
    str r8, [r0, #0x20]
    add r0, r7, #0x1
    and r7, r0, #0xff
    ldrb r0, [r6, #0x9]
    cmp r7, r0
    blo _02157630
_021577cc:
    ldr r1, [r10, #0x124]
    mvn r0, #0x0
    cmp r1, r0
    beq _0215781c
    ldr r0, [r10, #0x4]
    mov r4, #0x2
    cmp r0, #0x8
    mov r2, #0x0
    bne _02157808
    ldr r1, [pc, #0x30]
    mov r0, r5
    mov r3, #0x6
    str r4, [sp, #0x0]
    bl NNS_G3dRenderObjSetCallBack
    b _0215781c
_02157808:
    ldr r1, [pc, #0x1c]
    mov r0, r5
    mov r3, #0x6
    str r4, [sp, #0x0]
    bl NNS_G3dRenderObjSetCallBack
_0215781c:
    mov r0, #0x1
    ldmia sp!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, pc}
    dcd gIslandViewStrings+0x30
    dcd _ZN10IslandView21FollowCallbackIsland8EP9NNSG3dRS_
    dcd _ZN10IslandView14FollowCallbackEP9NNSG3dRS_
}
#endif

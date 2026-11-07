// Overlay 5, part 9: the title screen's 3D camera.
#include <nnsys/g3d.h>
#include "ov005_title.h"

static const VecFx32 sUp = {0, 1, 0};
static const VecFx32 sDrawPos = {0, 0, 0x8D6BE};
static const VecFx32 sDrawUp = {0, 1, 0};

TitleCamera::TitleCamera()
{
    NNS_G3dGlbSetViewPort(0, 0, 255, 191);
    G3_MtxMode(GX_MTXMODE_PROJECTION);
    NNS_G3dGlbOrthoW(0x12000, -0x12000, -0x18000, 0x18000, -0x400000, 0x400000, FX32_ONE);
    SetFocus(0, 0);
}

BOOL TitleCamera::Update()
{
    VecFx32 target = mTarget;

    target.z -= 0x4800;
    NNS_G3dGlbLookAt(&mPos, &sUp, &target);
    return TRUE;
}

#ifdef NONMATCHING
void TitleCamera::Draw()
{
    G3_ViewPort(0, 0, 255, 191);
    VecFx32 pos = sDrawPos;
    VecFx32 up = sDrawUp;
    VecFx32 target = {0, 0, 0};
    MtxFx43 camera;

    pos.z = mTarget.z + 0x8D6BE;
    target.z = mTarget.z;
    MTX_LookAt(&pos, &up, &target, &camera);
    G3_OrthoW(0, 0xC0000, 0, 0x100000, -0x400000, 0x400000, 0x400000, NULL);
    G3_MultMtx43(&camera);
    G3_StoreMtx(0);
    G3_MtxMode(GX_MTXMODE_POSITION);
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm void TitleCamera::Draw() {
    stmdb sp!, {r4, lr}
    sub sp, sp, #0x68
    ldr ip, [pc, #0xc0]
    ldr r2, [pc, #0xc0]
    ldr r1, [pc, #0xc0]
    str ip, [r2, #0x0]
    mov ip, r0
    add r4, sp, #0x5c
    ldmia r1, {r0, r1, r2}
    ldr r3, [pc, #0xb0]
    stmia r4, {r0, r1, r2}
    ldmia r3, {r0, r1, r2}
    add lr, sp, #0x50
    stmia lr, {r0, r1, r2}
    add r2, sp, #0x44
    mov r0, #0x0
    str r0, [r2, #0x0]
    str r0, [r2, #0x4]
    str r0, [r2, #0x8]
    ldr r1, [ip, #0x28]
    ldr r0, [pc, #0x88]
    add r3, sp, #0x14
    add r0, r1, r0
    str r0, [sp, #0x64]
    ldr ip, [ip, #0x28]
    mov r0, r4
    mov r1, lr
    str ip, [sp, #0x4c]
    bl MTX_LookAt
    mov r1, #0x400000
    rsb r1, r1, #0x0
    str r1, [sp, #0x0]
    mov r1, #0x400000
    str r1, [sp, #0x4]
    mov r0, #0x0
    str r1, [sp, #0x8]
    mov r1, #0x1
    str r1, [sp, #0xc]
    mov r2, r0
    mov r1, #0xc0000
    mov r3, #0x100000
    str r0, [sp, #0x10]
    bl G3i_OrthoW_
    add r0, sp, #0x14
    bl G3_MultMtx43
    ldr r1, [pc, #0x28]
    mov r0, #0x0
    str r0, [r1, #0x0]
    mov r0, #0x1
    str r0, [r1, #-0xc]
    add sp, sp, #0x68
    ldmia sp!, {r4, pc}
    dcd 0xbfff0000
    dcd 0x4000580
    dcd sDrawPos
    dcd sDrawUp
    dcd 0x8d6be
    dcd 0x400044c
}
#endif

void TitleCamera::SetFocus(fx32 x, fx32 z)
{
    mPos.x = x;
    mPos.y = 0x64000;
    mPos.z = z + 0x64000;
    mTarget.x = x;
    mTarget.y = 0;
    mTarget.z = z;
}

#ifdef NONMATCHING
TitleCamera::~TitleCamera()
{
}
#else
// The original has the deleting destructor in TitleMenu.cpp's code, so the complete destructor is
// asm here and the vtable is spelled out (mwcc emits no vtable while the destructor is undefined).
extern "C" {
void _ZN4TaskD2Ev(void);
void _ZN4Task5Func4Ev(void);
void _ZN11TitleCameraD1Ev(void);
void func_ov005_02157888(void);   // TitleCamera's deleting destructor
void _ZN11TitleCamera6UpdateEv(void);
void _ZN11TitleCamera4DrawEv(void);

void *_ZTV11TitleCamera[7] = {
    NULL, NULL, (void *)_ZN11TitleCameraD1Ev, (void *)func_ov005_02157888, (void *)_ZN11TitleCamera6UpdateEv,
    (void *)_ZN11TitleCamera4DrawEv, (void *)_ZN4Task5Func4Ev,
};
}
extern "C" asm void _ZN11TitleCameraD1Ev(void) {
    stmdb sp!, {r4, lr}
    mov r4, r0
    bl _ZN4TaskD2Ev
    mov r0, r4
    ldmia sp!, {r4, pc}
}
#endif

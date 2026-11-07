// The 2D graphics manager (ARM9 0x02029198..0x02029380).
#include "Gfx2D.h"

extern "C" void func_0202bf8c(NNSG2dOamManagerInstance *oam);

BOOL Gfx2D::RegisterTransfer(NNS_GFD_DST_TYPE type, u32 dstAddr, void *src, u32 size)
{
    return NNS_GfdRegisterNewVramTransferTask(type, dstAddr, src, size);
}

Gfx2D::Gfx2D()
{
    u32 size;

    NNS_GfdInitVramTransferManager(mTasks, 400);
    NNS_G2dInitCellTransferStateManager(mCellStates, 400, RegisterTransfer);
    size = NNS_GfdGetLnkTexVramManagerWorkSize(0x80);
    mTexWork = new u8[size];
    NNS_GfdInitLnkTexVramManager(0x60000, 0, mTexWork, size, TRUE);
    size = NNS_GfdGetLnkPlttVramManagerWorkSize(0x200);
    mPlttWork = new u8[size];
    NNS_GfdInitLnkPlttVramManager(0x18000, mPlttWork, size, TRUE);
    NNS_G2dInitOamManagerModule();
    NNS_G2dGetNewManagerInstance(&mOam, 0, 64, NNS_G2D_OAMTYPE_MAIN);
    func_0202bf8c(&mOam);
}

void Gfx2D::InitCellAnim(NNSG2dCellAnimation *anim, const NNSG2dAnimSequence *seq, const NNSG2dCellDataBank *bank,
                         u32 handle, u32 dst3D, u32 dst2DMain, u32 dst2DSub, const void *ncgr, const void *ncbr,
                         u32 size)
{
    NNS_G2dInitCellAnimationVramTransfered(anim, seq, bank, handle, dst3D, dst2DMain, dst2DSub, ncgr, ncbr, size);
}

u32 Gfx2D::GetNewTransferHandle()
{
    return NNS_G2dGetNewCellTransferStateHandle();
}

void Gfx2D::FreeTransferHandle(u32 handle)
{
    NNS_G2dFreeCellTransferStateHandle(handle);
}

void Gfx2D::Flush()
{
    NNS_GfdDoVramTransfer();
    NNS_G2dApplyOamManagerToHW(&mOam);
    NNS_G2dResetOamManagerBuffer(&mOam);
}

Gfx2DBase::Gfx2DBase()
{
    gGfx2D = (Gfx2D *)this;
}

#ifdef NONMATCHING
// In the original both destructors are inline; their out-of-line copies are spread over the files
// that use them (Gfx2D's complete destructor is at 0x02027d44).
Gfx2DBase::~Gfx2DBase()
{
    gGfx2D = NULL;
}

Gfx2D::~Gfx2D()
{
}
#else
extern "C" {
void _ZdlPv(void);
void _ZN5Gfx2DD1Ev(void);
void _ZN5Gfx2DD0Ev(void);
void _ZN9Gfx2DBaseD1Ev(void);
void _ZN9Gfx2DBaseD0Ev(void);

// mwcc emits no vtables while the destructors are undefined, so they are spelled out.
void *_ZTV5Gfx2D[4] = {NULL, NULL, (void *)_ZN5Gfx2DD1Ev, (void *)_ZN5Gfx2DD0Ev};
void *_ZTV9Gfx2DBase[4] = {NULL, NULL, (void *)_ZN9Gfx2DBaseD1Ev, (void *)_ZN9Gfx2DBaseD0Ev};
}
extern "C" asm void _ZN5Gfx2DD0Ev(void) {
    stmdb sp!, {r4, lr}
    ldr r1, =gGfx2D
    mov r2, #0x0
    mov r4, r0
    str r2, [r1, #0x0]
    bl _ZdlPv
    mov r0, r4
    ldmia sp!, {r4, pc}
}
extern "C" asm void _ZN9Gfx2DBaseD1Ev(void) {
    ldr r1, =gGfx2D
    mov r2, #0x0
    str r2, [r1, #0x0]
    bx lr
}
extern "C" asm void _ZN9Gfx2DBaseD0Ev(void) {
    stmdb sp!, {r4, lr}
    ldr r1, =gGfx2D
    mov r2, #0x0
    mov r4, r0
    str r2, [r1, #0x0]
    bl _ZdlPv
    mov r0, r4
    ldmia sp!, {r4, pc}
}
#endif

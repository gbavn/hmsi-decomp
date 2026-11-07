#ifndef GFX2D_H
#define GFX2D_H

// The 2D graphics manager (ARM9 0x02029198..): VRAM transfer tasks, cell transfer states, the linked
// texture/palette VRAM managers and the main OAM manager. Names are ours.
#include <nnsys/g2d.h>
#include <nnsys/gfd.h>

// Registers itself as gGfx2D (0x020ec858).
class Gfx2DBase {
public:
    Gfx2DBase();
    virtual ~Gfx2DBase();
};

class Gfx2D : public Gfx2DBase {
public:
    Gfx2D();
    virtual ~Gfx2D();

    static BOOL RegisterTransfer(NNS_GFD_DST_TYPE type, u32 dstAddr, void *src, u32 size);
    void InitCellAnim(NNSG2dCellAnimation *anim, const NNSG2dAnimSequence *seq, const NNSG2dCellDataBank *bank,
                      u32 handle, u32 dst3D, u32 dst2DMain, u32 dst2DSub, const void *ncgr, const void *ncbr,
                      u32 size);
    u32 GetNewTransferHandle();
    void FreeTransferHandle(u32 handle);
    void Flush();

    NNSGfdVramTransferTask mTasks[400];
    NNSG2dCellTransferState mCellStates[400];
    NNSG2dOamManagerInstance mOam;
    void *mTexWork;
    void *mPlttWork;
};

extern Gfx2D *gGfx2D;

#endif

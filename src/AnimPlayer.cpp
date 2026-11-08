// A small frame animation player (ARM9 0x02036404..0x020365f8): plays the frame lists of an animation
// file (header at file+4 with an offset table, then two data blocks). Names are ours.
#include <nitro.h>

extern "C" {

// One sequence: mode (what happens at the end), loop frame, frame count, then {length, cell} per frame.
typedef struct AnimSeq {
    u8 mode;
    u8 loopFrame;
    u16 numFrames;
    struct {
        u16 length;
        u16 cell;
    } frames[1];
} AnimSeq;

typedef struct AnimPlayer {
    u8 *block1;
    u8 *head;
    u8 *block2;
    AnimSeq *seq;
    u8 flags;               // bit 0: playing
    u8 unk11;
    u8 unk12[6];
    int mode;               // 0 = stop at the end, 1 = loop
    u16 seqNo;
    u16 frame;
    u16 cell;
    u8 unk22[2];
    fx32 frameLength;
    fx32 time;
    u8 unk2c[4];
    BOOL unk30;
    u8 dirty;
    u8 unk35[3];
} AnimPlayer;

AnimSeq *func_020364b0(AnimPlayer *self, int i);
void func_020364d8(AnimPlayer *self, int frame);
BOOL func_020365ac(AnimPlayer *self);
void func_020365c0(AnimPlayer *self);
void func_020365d4(AnimPlayer *self);
int func_020365e4(AnimPlayer *self);
int func_020365f0(AnimPlayer *self);

void func_02036404(AnimPlayer *self, u8 *file)
{
    u8 *head;
    u8 *block1;
    u32 ofs;
    u32 ofs2;

    MIi_CpuClear32(0, self, sizeof(AnimPlayer));
    head = file + 4;
    ofs = *(u32 *)head;
    ofs2 = *(u32 *)(head + ofs);
    block1 = head + ofs;
    self->block1 = block1;
    self->head = head;
    self->block2 = block1 + ofs2;
    self->unk11 = 1;
    self->mode = 5;
    self->unk30 = (BOOL)(*(u32 *)self->block2 > 4);
    self->dirty = 1;
}

void func_0203646c(AnimPlayer *self, int seqNo)
{
    self->seq = func_020364b0(self, seqNo);
    self->mode = func_020365e4(self);
    self->seqNo = seqNo;
    self->time = 0;
    func_020364d8(self, 0);
    self->flags |= 1;
}

AnimSeq *func_020364b0(AnimPlayer *self, int i)
{
    return (AnimSeq *)(self->head + ((u16 *)(self->head + 4))[i]);
}

u8 *func_020364c4(AnimPlayer *self, int i)
{
    return self->block1 + ((u16 *)(self->block1 + 4))[i];
}

void func_020364d8(AnimPlayer *self, int frame)
{
    self->frame = frame;
    self->cell = self->seq->frames[frame].cell;
    self->frameLength = self->seq->frames[frame].length << FX32_SHIFT;
    self->dirty = 1;
}

// Advances the animation by `dt`.
void func_0203650c(AnimPlayer *self, fx32 dt)
{
    u16 frame;

    if (func_020365ac(self)) {
        return;
    }
    if (dt < 0) {
        return;
    }
    self->time += dt;
    if (self->time < self->frameLength) {
        return;
    }
    frame = self->frame;
    self->time -= self->frameLength;
    if (frame + 1 < self->seq->numFrames) {
        func_020364d8(self, frame + 1);
        return;
    }
    switch (func_020365f0(self)) {
    case 1:
        func_020365c0(self);
        break;
    case 0:
        func_020365d4(self);
        break;
    case 2:
    case 3:
        break;
    }
}

BOOL func_020365ac(AnimPlayer *self)
{
    return !(self->flags & 1);
}

void func_020365c0(AnimPlayer *self)
{
    func_020364d8(self, self->seq->loopFrame);
}

void func_020365d4(AnimPlayer *self)
{
    self->flags &= ~1;
}

int func_020365e4(AnimPlayer *self)
{
    return self->seq->mode;
}

int func_020365f0(AnimPlayer *self)
{
    return self->mode;
}

}

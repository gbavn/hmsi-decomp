// Objects lying on the field (ARM9 0x0203772c..0x02038558): crops, stones, lava... drawn with one or two
// 2D sprites placed by the item table. Names are ours.
//
// The class derives from a field object base (constructor/destructor in overlay 2) that derives from
// Task. Vtable at 0x020bcaf4 (in the asm gap): [dtor, dtor+delete, Update, Draw, Task::Func4, SetPos,
// SetPosXYZ, GetPos, GetPos2, SetUnk11, IsNear, ov002 0x021c9990]. As in PackFile.cpp the methods are
// extern "C" functions and the class below is only a "view" with declared virtuals to make virtual calls.
#include <nitro.h>
#include "Task.h"

// One entry of the item definitions (12 bytes).
struct ItemDef {
    s8 kind;
    s8 flags;
    u8 unk2[2];
    u32 unk4_0 : 20;
    u32 stage : 3;      // growth stage?
    u32 unk4_23 : 9;
    u32 unk8;
};

struct ItemModel {
    u8 unk0[8];
    int depth;
};

struct ItemOffset {
    s16 x;
    s16 y;
};

struct ItemTable {
    u8 unk0[0x24];
    ItemDef *defs;
    int count;
    u8 unk2c[0x3c - 0x2c];
    ItemModel *models;
    ItemOffset *offsets;

    s8 GetKind(int id)
    {
        if (id >= 0 && id < count) {
            return defs[id].kind;
        }
        return -1;
    }
    int GetStage(int id)
    {
        if (id >= 0 && id < count) {
            return defs[id].stage;
        }
        return 0;
    }
    s8 GetFlags(int id)
    {
        if (id >= 0 && id < count) {
            return defs[id].flags;
        }
        return 0;
    }
    ItemModel *GetModel(int id)
    {
        if (id >= 0 && id < count) {
            return &models[id];
        }
        return NULL;
    }
    ItemOffset *GetOffset(int id)
    {
        if (id >= 0 && id < count) {
            return &offsets[id];
        }
        return NULL;
    }
};

// The sprite object of 0x02029xxx.
struct GSprite {
    u8 unk0[6];
    u16 height;
    u8 unk8[0x84 - 8];
    s16 x;
    s16 y;
};

class GroundObject {
public:
    virtual GroundObject *Dtor();
    virtual GroundObject *DtorDelete();
    virtual BOOL Update();
    virtual void Draw();
    virtual void Func4();
    virtual void SetPos(const VecFx32 *pos);
    virtual void SetPosXYZ(fx32 x, fx32 y, fx32 z);
    virtual VecFx32 *GetPos();
    virtual VecFx32 *GetPos2();
    virtual void SetUnk11(u8 v);
    virtual BOOL IsNear(const VecFx32 *pos);

    TaskManager *mManager;
    Task *mPrev;
    Task *mNext;
    u8 mFlag;
    u8 unk11;
    fx32 radius;
    VecFx32 pos;
    int unk24;
    u16 itemId;
    u8 dead;
    GSprite *sprites[2];
};

extern "C" {
extern ItemTable *data_0214f6ec;
extern void *data_020bcaf4[];
extern void *data_020ec870;
extern char data_020bc95c[];   // "rom/pics/GROUND_OBJECT/GREEN_PEPPER_GROWTH3.NCBR"
extern char data_020bc990[];   // "....NCLR"
extern char data_020bc9c4[];   // "rom/pics/GROUND_OBJECT/ORE_STONE.NCBR"
extern char data_020bc9ec[];
extern char data_020bca14[];   // "rom/pics/GROUND_OBJECT/LAVA0.NCBR"
extern char data_020bca38[];
extern char data_020bca5c[];   // LAVA1
extern char data_020bca80[];
extern char data_020bcaa4[];   // LAVA2
extern char data_020bcac8[];

void func_ov002_021c9910(GroundObject *self);
void func_ov002_021c997c(GroundObject *self);

void func_0202938c(GSprite *s, int depth);
void func_020293c0(GSprite *s, int flags);
void func_020293d0(GSprite *s, int flags);
GSprite *func_02029b88(const char *ncbr, const char *nclr, VecFx32 pos, int unk);
void func_02029e24(GSprite *s);
void func_0202a014(void *renderer, GSprite *s);

GSprite *func_02039648(VecFx32 *pos);
GSprite *func_020396a0(VecFx32 *pos);
GSprite *func_020396f8(int kind, s8 stage, VecFx32 *pos, int unk);
GSprite *func_0203999c(VecFx32 *pos);
GSprite *func_020399e8(VecFx32 *pos);
GSprite *func_02039a34(VecFx32 *pos);
int func_02039a80(int kind);
int func_02039ae4(int kind, s8 stage);
BOOL func_02039b24(int kind, s8 stage);
BOOL func_02039ff4(int kind);
void func_0203a100(int kind, s8 stage, s16 *x, s16 *y, u8 index);
BOOL func_0203a460(int kind);
BOOL func_0203a47c(int kind);
BOOL func_0203a484(int kind);
BOOL func_0203a4a0(int kind);
BOOL func_0203a4bc(int kind);
BOOL func_0203a4f4(int kind);
BOOL func_0203a534(int kind);
BOOL func_0203a550(int kind);

void func_02037e60(GroundObject *self);
BOOL func_02037ecc(GroundObject *self);
void func_02038050(GroundObject *self, int kind, s8 stage);
void func_02038258(GroundObject *self, int kind, s8 stage);
void func_020382a8(GroundObject *self, int kind);
void func_020382f4(GroundObject *self);
void func_02038360(GroundObject *self);

GroundObject *func_0203772c(GroundObject *self, fx32 x, fx32 y, fx32 z, int itemId, int unk24)
{
    func_ov002_021c9910(self);
    *(void ***)self = data_020bcaf4;
    self->unk24 = unk24;
    self->itemId = itemId;
    self->dead = 0;
    self->SetPosXYZ(x, y, z);
    for (int i = 0; i < 2; i++) {
        self->sprites[i] = NULL;
    }
    s8 kind = data_0214f6ec->GetKind(self->itemId);
    s8 stage = data_0214f6ec->GetStage(self->itemId);
    if (func_0203a534(kind)) {
        func_020382f4(self);
    } else if (func_0203a550(kind)) {
        func_02038360(self);
    } else if (func_0203a4bc(kind)) {
        func_020382a8(self, kind);
    } else if (func_0203a4a0(kind)) {
        func_02038258(self, kind, stage);
    } else if (stage == 0) {
        if (kind == 0x15) {
            self->sprites[0] = func_020396a0(self->GetPos());
        } else if (!func_0203a47c(kind)) {
            self->sprites[0] = func_02039648(self->GetPos());
        }
    } else if (stage > func_02039a80(kind)) {
        if (kind == 0x15) {
            self->sprites[0] = func_020399e8(self->GetPos());
        } else {
            self->sprites[0] = func_0203999c(self->GetPos());
        }
    } else {
        func_02038050(self, kind, stage);
    }
    return self;
}

VecFx32 *func_0203794c(GroundObject *self)
{
    return &self->pos;
}

void func_02037954(GroundObject *self, fx32 x, fx32 y, fx32 z)
{
    self->pos.x = x;
    self->pos.y = y;
    self->pos.z = z;
}

GroundObject *func_02037964(GroundObject *self)
{
    *(void ***)self = data_020bcaf4;
    func_02037e60(self);
    func_ov002_021c997c(self);
    return self;
}

GroundObject *func_0203798c(GroundObject *self)
{
    *(void ***)self = data_020bcaf4;
    func_02037e60(self);
    func_ov002_021c997c(self);
    Task::operator delete(self);
    return self;
}

BOOL func_020379bc(GroundObject *self)
{
    return self->dead == 0;
}

#ifdef NONMATCHING
void func_020379d0(GroundObject *self)
{
    int kind;
    int i;
    if (self->dead) {
        return;
    }
    kind = data_0214f6ec->GetKind(self->itemId);
    if ((u32)(kind - 0x1e) <= 2 && self->unk24 == 4) {
        return;
    }
    for (i = 0; i < 2; i++) {
        if (self->sprites[i] == NULL) {
            continue;
        }
        if (data_0214f6ec->GetOffset(self->itemId)->x < -0x60) {
            continue;
        }
        if (data_0214f6ec->GetOffset(self->itemId)->x > 0x140) {
            continue;
        }
        if (data_0214f6ec->GetOffset(self->itemId)->y < -0x60) {
            continue;
        }
        if (data_0214f6ec->GetOffset(self->itemId)->y > 0x100) {
            continue;
        }
        self->sprites[i]->x = data_0214f6ec->GetOffset(self->itemId)->x;
        self->sprites[i]->y = data_0214f6ec->GetOffset(self->itemId)->y;
        if (data_0214f6ec->GetFlags(self->itemId) & 0x10) {
            self->sprites[i]->y += 8;
            self->sprites[i]->y -= self->sprites[i]->height >> 1;
        } else if (!func_0203a4a0(kind)) {
            if (!func_0203a4bc(kind) && !func_0203a534(kind) && !func_0203a550(kind)) {
                self->sprites[i]->y += 8;
                self->sprites[i]->y -= self->sprites[i]->height >> 1;
            } else if (func_0203a4f4(data_0214f6ec->GetKind(self->itemId))) {
                self->sprites[i]->x += 12;
            }
        }
        int depth = data_0214f6ec->GetModel(self->itemId)->depth - 0x2000;
        if (data_0214f6ec->GetStage(self->itemId) == 0) {
            depth -= 0x5000;
        } else if (func_0203a460(data_0214f6ec->GetKind(self->itemId))) {
            depth -= 0x1000;
        }
        func_0202938c(self->sprites[i], depth);
        if (i != 0) {
            func_0202938c(self->sprites[i], depth + 0x1000);
        }
        if (kind == 0x14) {
            func_0202938c(self->sprites[i], depth + 0x1800);
        }
        func_020293c0(self->sprites[i], 0x200);
        func_020293c0(self->sprites[i], 0x1000);
        func_0203a100(data_0214f6ec->GetKind(self->itemId), data_0214f6ec->GetStage(self->itemId),
                      &self->sprites[i]->x, &self->sprites[i]->y, i);
        func_0202a014(data_020ec870, self->sprites[i]);
    }
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm void func_020379d0(GroundObject *self) {
    stmdb sp!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, lr}
    mov r10, r0
    ldrb r0, [r10, #0x2a]
    cmp r0, #0x0
    ldmneia sp!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, pc}
    ldrh r2, [r10, #0x28]
    ldr r0, [pc, #0x468]
    cmp r2, #0x0
    ldr r1, [r0, #0x0]
    blt _02037a18
    ldr r0, [r1, #0x28]
    cmp r2, r0
    bge _02037a18
    mov r0, #0xc
    mul r0, r2, r0
    ldr r1, [r1, #0x24]
    ldrsb r9, [r1, r0]
    b _02037a1c
_02037a18:
    mvn r9, #0x0
_02037a1c:
    sub r0, r9, #0x1e
    cmp r0, #0x2
    bhi _02037a34
    ldr r0, [r10, #0x24]
    cmp r0, #0x4
    ldmeqia sp!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, pc}
_02037a34:
    mvn r6, #0x0
    sub r11, r6, #0x5f
    ldr r5, [pc, #0x414]
    mov r8, #0x0
    mov r4, r11
_02037a48:
    add r0, r10, r8, lsl #0x2
    ldr r2, [r0, #0x2c]
    cmp r2, #0x0
    beq _02037e48
    ldrh r0, [r10, #0x28]
    ldr r1, [r5, #0x0]
    cmp r0, #0x0
    blt _02037a7c
    ldr r3, [r1, #0x28]
    cmp r0, r3
    ldrlt r3, [r1, #0x40]
    addlt r3, r3, r0, lsl #0x2
    blt _02037a80
_02037a7c:
    mov r3, #0x0
_02037a80:
    ldrsh r3, [r3, #0x0]
    cmp r3, r4
    blt _02037e48
    cmp r0, #0x0
    blt _02037aa8
    ldr r3, [r1, #0x28]
    cmp r0, r3
    ldrlt r3, [r1, #0x40]
    addlt r3, r3, r0, lsl #0x2
    blt _02037aac
_02037aa8:
    mov r3, #0x0
_02037aac:
    ldrsh r3, [r3, #0x0]
    cmp r3, #0x140
    bgt _02037e48
    cmp r0, #0x0
    blt _02037ad4
    ldr r3, [r1, #0x28]
    cmp r0, r3
    ldrlt r3, [r1, #0x40]
    addlt r3, r3, r0, lsl #0x2
    blt _02037ad8
_02037ad4:
    mov r3, #0x0
_02037ad8:
    ldrsh r3, [r3, #0x2]
    cmp r3, r11
    blt _02037e48
    cmp r0, #0x0
    blt _02037b00
    ldr r3, [r1, #0x28]
    cmp r0, r3
    ldrlt r3, [r1, #0x40]
    addlt r3, r3, r0, lsl #0x2
    blt _02037b04
_02037b00:
    mov r3, #0x0
_02037b04:
    ldrsh r3, [r3, #0x2]
    cmp r3, #0x100
    bgt _02037e48
    cmp r0, #0x0
    blt _02037b2c
    ldr r3, [r1, #0x28]
    cmp r0, r3
    ldrlt r1, [r1, #0x40]
    addlt r0, r1, r0, lsl #0x2
    blt _02037b30
_02037b2c:
    mov r0, #0x0
_02037b30:
    ldrsh r0, [r0, #0x0]
    strh r0, [r2, #0x84]
    ldrh r2, [r10, #0x28]
    ldr r1, [r5, #0x0]
    cmp r2, #0x0
    blt _02037b5c
    ldr r0, [r1, #0x28]
    cmp r2, r0
    ldrlt r0, [r1, #0x40]
    addlt r0, r0, r2, lsl #0x2
    blt _02037b60
_02037b5c:
    mov r0, #0x0
_02037b60:
    ldrsh r1, [r0, #0x2]
    add r0, r10, r8, lsl #0x2
    ldr r0, [r0, #0x2c]
    strh r1, [r0, #0x86]
    ldrh r2, [r10, #0x28]
    ldr r1, [r5, #0x0]
    cmp r2, #0x0
    blt _02037ba0
    ldr r0, [r1, #0x28]
    cmp r2, r0
    bge _02037ba0
    ldr r1, [r1, #0x24]
    mov r0, #0xc
    mla r0, r2, r0, r1
    ldrsb r0, [r0, #0x1]
    b _02037ba4
_02037ba0:
    mov r0, #0x0
_02037ba4:
    tst r0, #0x10
    beq _02037bd8
    add r2, r10, r8, lsl #0x2
    ldr r1, [r2, #0x2c]
    ldrsh r0, [r1, #0x86]
    add r0, r0, #0x8
    strh r0, [r1, #0x86]
    ldr r2, [r2, #0x2c]
    ldrsh r1, [r2, #0x86]
    ldrh r0, [r2, #0x6]
    sub r0, r1, r0, asr #0x1
    strh r0, [r2, #0x86]
    b _02037c98
_02037bd8:
    mov r0, r9
    bl func_0203a4a0
    cmp r0, #0x0
    bne _02037c98
    mov r0, r9
    bl func_0203a4bc
    cmp r0, #0x0
    bne _02037c44
    mov r0, r9
    bl func_0203a534
    cmp r0, #0x0
    bne _02037c44
    mov r0, r9
    bl func_0203a550
    cmp r0, #0x0
    bne _02037c44
    add r2, r10, r8, lsl #0x2
    ldr r1, [r2, #0x2c]
    ldrsh r0, [r1, #0x86]
    add r0, r0, #0x8
    strh r0, [r1, #0x86]
    ldr r2, [r2, #0x2c]
    ldrsh r1, [r2, #0x86]
    ldrh r0, [r2, #0x6]
    sub r0, r1, r0, asr #0x1
    strh r0, [r2, #0x86]
    b _02037c98
_02037c44:
    ldrh r2, [r10, #0x28]
    ldr r1, [r5, #0x0]
    cmp r2, #0x0
    blt _02037c74
    ldr r0, [r1, #0x28]
    cmp r2, r0
    bge _02037c74
    mov r0, #0xc
    mul r0, r2, r0
    ldr r1, [r1, #0x24]
    ldrsb r0, [r1, r0]
    b _02037c78
_02037c74:
    mov r0, r6
_02037c78:
    bl func_0203a4f4
    cmp r0, #0x0
    beq _02037c98
    add r0, r10, r8, lsl #0x2
    ldr r1, [r0, #0x2c]
    ldrsh r0, [r1, #0x84]
    add r0, r0, #0xc
    strh r0, [r1, #0x84]
_02037c98:
    ldrh r0, [r10, #0x28]
    ldr r1, [r5, #0x0]
    cmp r0, #0x0
    blt _02037cc4
    ldr r2, [r1, #0x28]
    cmp r0, r2
    bge _02037cc4
    ldr r3, [r1, #0x3c]
    mov r2, #0xc
    mla r2, r0, r2, r3
    b _02037cc8
_02037cc4:
    mov r2, #0x0
_02037cc8:
    ldr r2, [r2, #0x8]
    cmp r0, #0x0
    sub r7, r2, #0x2000
    blt _02037d00
    ldr r2, [r1, #0x28]
    cmp r0, r2
    bge _02037d00
    ldr r3, [r1, #0x24]
    mov r2, #0xc
    mla r2, r0, r2, r3
    ldr r2, [r2, #0x4]
    mov r2, r2, lsl #0x9
    mov r2, r2, lsr #0x1d
    b _02037d04
_02037d00:
    mov r2, #0x0
_02037d04:
    cmp r2, #0x0
    subeq r7, r7, #0x5000
    beq _02037d48
    cmp r0, #0x0
    blt _02037d38
    ldr r2, [r1, #0x28]
    cmp r0, r2
    bge _02037d38
    ldr r2, [r1, #0x24]
    mov r1, #0xc
    mul r1, r0, r1
    ldrsb r0, [r2, r1]
    b _02037d3c
_02037d38:
    mov r0, r6
_02037d3c:
    bl func_0203a460
    cmp r0, #0x0
    subne r7, r7, #0x1000
_02037d48:
    add r0, r10, r8, lsl #0x2
    ldr r0, [r0, #0x2c]
    mov r1, r7
    bl func_0202938c
    cmp r8, #0x0
    beq _02037d70
    add r0, r10, r8, lsl #0x2
    ldr r0, [r0, #0x2c]
    add r1, r7, #0x1000
    bl func_0202938c
_02037d70:
    cmp r9, #0x14
    bne _02037d88
    add r1, r7, #0x1800
    add r0, r10, r8, lsl #0x2
    ldr r0, [r0, #0x2c]
    bl func_0202938c
_02037d88:
    add r0, r10, r8, lsl #0x2
    ldr r0, [r0, #0x2c]
    mov r1, #0x200
    bl func_020293c0
    add r0, r10, r8, lsl #0x2
    ldr r0, [r0, #0x2c]
    mov r1, #0x1000
    bl func_020293c0
    ldrh r1, [r10, #0x28]
    ldr r2, [r5, #0x0]
    cmp r1, #0x0
    blt _02037dd8
    ldr r0, [r2, #0x28]
    cmp r1, r0
    bge _02037dd8
    mov r0, #0xc
    mul r0, r1, r0
    ldr r3, [r2, #0x24]
    ldrsb r0, [r3, r0]
    b _02037ddc
_02037dd8:
    mov r0, r6
_02037ddc:
    cmp r1, #0x0
    blt _02037e0c
    ldr r3, [r2, #0x28]
    cmp r1, r3
    bge _02037e0c
    ldr r3, [r2, #0x24]
    mov r2, #0xc
    mla r2, r1, r2, r3
    ldr r1, [r2, #0x4]
    mov r1, r1, lsl #0x9
    mov r1, r1, lsr #0x1d
    b _02037e10
_02037e0c:
    mov r1, #0x0
_02037e10:
    add r2, r10, r8, lsl #0x2
    ldr r3, [r2, #0x2c]
    mov r1, r1, lsl #0x18
    and r2, r8, #0xff
    str r2, [sp, #0x0]
    add r2, r3, #0x84
    mov r1, r1, asr #0x18
    add r3, r3, #0x86
    bl func_0203a100
    ldr r0, [pc, #0x20]
    add r1, r10, r8, lsl #0x2
    ldr r0, [r0, #0x0]
    ldr r1, [r1, #0x2c]
    bl func_0202a014
_02037e48:
    add r8, r8, #0x1
    cmp r8, #0x2
    blt _02037a48
    ldmia sp!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, pc}
    dcd data_0214f6ec
    dcd data_020ec870
}
#endif

void func_02037e60(GroundObject *self)
{
    for (int i = 0; i < 2; i++) {
        if (self->sprites[i] != NULL) {
            func_02029e24(self->sprites[i]);
            self->sprites[i] = NULL;
        }
    }
}

BOOL func_02037e9c(GroundObject *self)
{
    if (func_02037ecc(self)) {
        return FALSE;
    }
    self->dead = 1;
    return TRUE;
}

BOOL func_02037ebc(GroundObject *self)
{
    self->dead = 1;
    return TRUE;
}

BOOL func_02037ecc(GroundObject *self)
{
    return func_02039ff4(data_0214f6ec->GetKind(self->itemId));
}

void func_02037f14(GroundObject *self)
{
    s8 kind = data_0214f6ec->GetKind(self->itemId);
    if (func_0203a460(kind)) {
        return;
    }
    if (kind == 0x14) {
        func_02029e24(self->sprites[0]);
        self->sprites[0] = NULL;
        GSprite *s = func_02039a34(self->GetPos());
        self->sprites[0] = s;
        if (s == NULL) {
            return;
        }
        func_020293d0(s, 0x100);
    } else if (kind == 0xe) {
        func_02029e24(self->sprites[1]);
        self->sprites[1] = NULL;
        GSprite *s = func_02029b88(data_020bc95c, data_020bc990, *self->GetPos(), 0);
        self->sprites[1] = s;
        if (s == NULL) {
            return;
        }
        func_020293d0(s, 0x100);
    } else if (self->sprites[1] != NULL) {
        func_02029e24(self->sprites[1]);
        self->sprites[1] = NULL;
    }
}

void func_02038050(GroundObject *self, int kind, s8 stage)
{
    if (stage > func_02039a80(kind)) {
        return;
    }
    for (int i = 0; i < 2; i++) {
        if (i >= func_02039ae4(kind, stage)) {
            continue;
        }
        self->sprites[i] = NULL;
        if (i == 0 && func_02039b24(kind, stage)) {
            if (func_0203a484(kind)) {
                self->sprites[i] = func_020396f8(kind, stage, self->GetPos(), i != 0);
            } else if (func_0203a460(kind)) {
                self->sprites[i] = func_020396f8(kind, 3, self->GetPos(), 0);
            } else if (kind == 0xe) {
                if (stage == 3) {
                    self->sprites[i] = func_020396f8(kind, stage - 1, self->GetPos(), 0);
                } else if (stage == 4) {
                    self->sprites[i] = func_020396f8(kind, stage - 2, self->GetPos(), 0);
                }
            } else {
                self->sprites[i] = func_020396f8(kind, stage - 1, self->GetPos(), 0);
            }
        } else {
            self->sprites[i] = func_020396f8(kind, stage, self->GetPos(), i != 0);
        }
        if (self->sprites[i] != NULL) {
            func_020293d0(self->sprites[i], 0x100);
        }
    }
}

void func_02038258(GroundObject *self, int kind, s8 stage)
{
    self->sprites[0] = NULL;
    GSprite *s = func_020396f8(kind, stage, self->GetPos(), 0);
    self->sprites[0] = s;
    if (s == NULL) {
        return;
    }
    func_020293d0(s, 0x100);
}

void func_020382a8(GroundObject *self, int kind)
{
    self->sprites[0] = NULL;
    GSprite *s = func_020396f8(kind, 0, self->GetPos(), 0);
    self->sprites[0] = s;
    if (s == NULL) {
        return;
    }
    func_020293d0(s, 0x100);
}

void func_020382f4(GroundObject *self)
{
    self->sprites[0] = NULL;
    GSprite *s = func_02029b88(data_020bc9c4, data_020bc9ec, *self->GetPos(), 0);
    self->sprites[0] = s;
    if (s == NULL) {
        return;
    }
    func_020293d0(s, 0x100);
}

static inline void LoadSprite(GroundObject *self, const char *ncbr, const char *nclr)
{
    if (self->sprites[0] != NULL) {
        func_02029e24(self->sprites[0]);
        ::operator delete(self->sprites[0]);
    }
    self->sprites[0] = NULL;
    GSprite *s = func_02029b88(ncbr, nclr, *self->GetPos(), 0);
    self->sprites[0] = s;
    if (s == NULL) {
        return;
    }
    func_020293d0(s, 0x100);
}

void func_02038360(GroundObject *self)
{
    LoadSprite(self, data_020bca14, data_020bca38);
}

void func_020383e8(GroundObject *self)
{
    LoadSprite(self, data_020bca5c, data_020bca80);
}

void func_02038470(GroundObject *self)
{
    LoadSprite(self, data_020bcaa4, data_020bcac8);
}

void func_020384f8(GroundObject *self, const VecFx32 *pos)
{
    self->pos.x = pos->x;
    self->pos.y = pos->y;
    self->pos.z = pos->z;
}

VecFx32 *func_02038514(GroundObject *self)
{
    return &self->pos;
}

void func_0203851c(GroundObject *self, u8 v)
{
    self->unk11 = v;
}

BOOL func_02038524(GroundObject *self, const VecFx32 *pos)
{
    return VEC_Distance(self->GetPos(), pos) <= self->radius;
}

}

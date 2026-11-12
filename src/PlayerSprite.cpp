// The player's sprites (ARM9 0x0203b07c..0x0203c04c). Names are ours.
//
// The pictures of the player are .xbb archives, one per set: rom/pics/char/boy/boy1.xbb (always loaded,
// 0x11 sprites), boy_axe.xbb, boy_fishing_rod.xbb... (one tool at a time, loaded in the background), and
// the same for the girl. Each sprite of a set has 3 files in the archive (LZ characters, cells,
// animations) and one palette per set. The number of sprites of each set is in data_020b2a4c; sprite i
// of all sets loaded is sprites[i] (the tool set starts at `base`).
//
// C++ class with a vtable at 0x020bda40 (in the asm gap): [~PlayerSprite, ~PlayerSprite + delete].
#include <nitro.h>
#include <nnsys/g2d.h>
#include <nnsys/gfd.h>
#include "game_file.h"
#include "Gfx2D.h"
#include "Sprite2D.h"

void *Ov009_GetWork();

extern "C" {

typedef struct ArchiveView {
    void *vtable;
    void *data;
} ArchiveView;

typedef struct PlayerSprite {
    void *vtable;
    GFile mainFile;             // 0x04
    u8 *mainData;               // 0x50
    GFile toolFile;             // 0x54
    u8 *toolData;               // 0xa0
    Sprite2D *cur;              // 0xa4
    u8 unkA8[8];                // 0xa8
    Sprite2D *sprites[0x38];    // 0xb0
    int biggest[0x10];          // 0x190 sprite of each set with the most characters
    int unk1D0;
    int unk1D4;
    int set;                    // 0x1d8 tool set (0 = none)
    int base;                   // 0x1dc first sprite of the tool set
    int count;                  // 0x1e0 sprites of the tool set
    int curNo;                  // 0x1e4
    int state;                  // 0x1e8 loading the tool set: 0..3, 4 = idle
    u8 unk1EC[8];
    OSTick tick;                // 0x1f4 time taken by the last load
} PlayerSprite;

extern void *data_020bda40[];       // vtable
extern void *_ZTV11GFileObject[];
extern char *data_020bd9b8[16];     // boy pictures (boy1.xbb, boy_axe.xbb...)
extern char *data_020bd9f8[16];     // girl pictures
extern int data_020b2a4c[];         // number of sprites of each set (index set + 1)
extern void *data_020f57c0;         // the game data

#define SET_GFILE_VT(f) (*(void ***)(f) = &_ZTV11GFileObject[2])

ArchiveView *_ZN7ArchiveC1EPv(ArchiveView *self, void *data);
void *func_02036fb0(ArchiveView *self, int i);
u32 func_02036ff8(ArchiveView *self, int i);
void *func_02037058(ArchiveView *self, int i);
int func_0203366c(void *self, int id);
void func_01ff8fa0(const VecFx32 *pos, int *x, int *y);
void func_02064104(void *self, int a, int b);
void func_02064124(void *self);
void func_020641f8(void *self, const VecFx32 *pos);
void func_020642d8(void *self);
void func_020642f8(void *self);
void func_0202cff4(const NNSG2dCellDataBank **bank, void *ncer, u32 *a, const char *key, void *b, u8 *loaded);
void func_0202d150(const NNSG2dAnimBankData **bank, void *nanr, const char *key, void *a, u8 *loaded);
void func_0202e22c(void *proxy, void *nclr, void *b, const char *key);
BOOL func_0202d370(void *a);
BOOL func_0202d3d8(void *a, u8 loaded);
BOOL func_0202d40c(void *a, u8 loaded);

int func_0203b744(PlayerSprite *self, int seq);
void func_0203b788(PlayerSprite *self);
void func_0203b890(PlayerSprite *self);
void func_0203b99c(PlayerSprite *self);
BOOL func_0203b9c8(PlayerSprite *self);
BOOL func_0203b9d8(PlayerSprite *self);
BOOL func_0203ba2c(PlayerSprite *self);
BOOL func_0203ba58(PlayerSprite *self);
int func_0203bab8(PlayerSprite *self, int set);
int func_0203bacc(PlayerSprite *self);
void func_0203bb00(PlayerSprite *self, int i, Sprite2D *s);
Sprite2D *func_0203bb14(PlayerSprite *self, int i);
int func_0203bb28(PlayerSprite *self, int set);
void func_0203bb34(PlayerSprite *self, ArchiveView *arc);
void func_0203bba4(PlayerSprite *self, ArchiveView *arc);
void func_0203bc90(PlayerSprite *self, ArchiveView *arc);
void func_0203bd1c(PlayerSprite *self, ArchiveView *arc);
void func_0203bdc0(PlayerSprite *self, ArchiveView *arc, int i);
void func_0203be7c(PlayerSprite *self);
void *func_0203bf28(PlayerSprite *self, ArchiveView *arc, int i, int type);
u32 func_0203bf80(PlayerSprite *self, ArchiveView *arc, int i, int type);
int func_0203bfd8(PlayerSprite *self, ArchiveView *arc, int i, int type);

PlayerSprite *func_0203b07c(PlayerSprite *self)
{
    int i;
    *(void ***)self = data_020bda40;
    SET_GFILE_VT(&self->mainFile);
    GFile_Init(&self->mainFile);
    SET_GFILE_VT(&self->toolFile);
    GFile_Init(&self->toolFile);
    func_02064104(self->unkA8, 1, -4);
    self->cur = NULL;
    for (i = 0; i < 0x38; i++) {
        self->sprites[i] = NULL;
        self->biggest[i] = 0;
    }
    for (i = 0; i < 0x10; i++) {
        self->biggest[i] = 0;
    }
    self->unk1D0 = -1;
    self->unk1D4 = 0;
    self->set = 0;
    self->base = -1;
    self->count = 0;
    self->toolData = NULL;
    self->state = 4;
    self->mainData = (u8 *)Ov009_GetWork() + 0x10000;
    // boy1.xbb or girl1.xbb (data_020bd9b8[16] = data_020bd9f8[0])
    GFile_Open(&self->mainFile, func_0203366c(data_020f57c0, 0) == 0 ? data_020bd9b8[0] : data_020bd9b8[16]);
    u32 len = GFile_GetLength(&self->mainFile);
    GFile_Read(&self->mainFile, len, self->mainData);
    DC_FlushRange(self->mainData, len);
    ArchiveView arc;
    _ZN7ArchiveC1EPv(&arc, self->mainData);
    func_0203bb34(self, &arc);
    self->cur = self->sprites[14];
    MI_UncompressLZ8(func_0203bf28(self, &arc, 14, 0), Ov009_GetWork());
    DC_FlushRange(Ov009_GetWork(), 0x10000);
    NNS_G2dInitImageProxy((NNSG2dImageProxy *)self->cur->unk58);
    NNS_G2dGetUnpackedCharacterData(Ov009_GetWork(), (NNSG2dCharacterData **)&self->cur->unk54);
    NNS_G2dLoadImageVramTransfer((NNSG2dCharacterData *)self->cur->unk54, NNS_GfdGetTexKeyAddr(self->cur->unk7C),
                                 NNS_G2D_VRAM_TYPE_3DMAIN, (NNSG2dImageProxy *)self->cur->unk58);
    self->cur->ncgr = Ov009_GetWork();
    if (self->cur->transferHandle == (u32)-1) {
        self->cur->transferHandle = gGfx2D->GetNewTransferHandle();
    }
    Gfx2D *gfx;
    Sprite2D *s;
    NNSG2dCharacterData *chr;
    s = self->cur;
    gfx = gGfx2D;
    chr = (NNSG2dCharacterData *)s->unk54;
    gfx->InitCellAnim(self->cur->cellAnim, NNS_G2dGetAnimSequenceByIdx(s->animBank, 0), s->cellBank,
                      s->transferHandle, NNS_GfdGetTexKeyAddr(s->unk7C), NNS_G2D_VRAM_ADDR_NONE, NNS_G2D_VRAM_ADDR_NONE, NULL, chr->pRawData,
                         chr->szByte);
    self->tick = 0;
    return self;
}

PlayerSprite *func_0203b2e0(PlayerSprite *self)
{
    *(void ***)self = data_020bda40;
    func_0203b890(self);
    func_0203b788(self);
    func_02064124(self->unkA8);
    SET_GFILE_VT(&self->toolFile);
    GFile_Close(&self->toolFile);
    SET_GFILE_VT(&self->mainFile);
    GFile_Close(&self->mainFile);
    return self;
}

PlayerSprite *func_0203b334(PlayerSprite *self)
{
    *(void ***)self = data_020bda40;
    func_0203b890(self);
    func_0203b788(self);
    func_02064124(self->unkA8);
    SET_GFILE_VT(&self->toolFile);
    GFile_Close(&self->toolFile);
    SET_GFILE_VT(&self->mainFile);
    GFile_Close(&self->mainFile);
    operator delete(self);
    return self;
}

PlayerSprite *func_0203b390(PlayerSprite *self)
{
    *(void ***)self = data_020bda40;
    func_0203b890(self);
    func_0203b788(self);
    func_02064124(self->unkA8);
    SET_GFILE_VT(&self->toolFile);
    GFile_Close(&self->toolFile);
    SET_GFILE_VT(&self->mainFile);
    GFile_Close(&self->mainFile);
    return self;
}

void func_0203b3e4(PlayerSprite *self, int front)
{
    if (front != 0 || self->curNo == 0xd) {
        for (int i = 0; i < 0x38; i++) {
            Sprite2D *s = self->sprites[i];
            if (s != NULL) {
                s->priority += 0x5000;
            }
        }
        func_020642f8(self->unkA8);
    } else {
        func_020642d8(self->unkA8);
    }
}

void func_0203b43c(PlayerSprite *self, const VecFx32 *pos)
{
    int x;
    int y;
    func_01ff8fa0(pos, &x, &y);
    for (int i = 0; i < 0x38; i++) {
        if (self->sprites[i] != NULL) {
            self->sprites[i]->x = x << FX32_SHIFT;
            self->sprites[i]->y = y << FX32_SHIFT;
            self->sprites[i]->z = pos->z;
            self->sprites[i]->priority = pos->z;
        }
    }
    func_020641f8(self->unkA8, pos);
}

void func_0203b4c4(PlayerSprite *self)
{
    switch (self->state) {
    case 0:
        func_0203b9c8(self);
        break;
    case 1:
        func_0203b9d8(self);
        break;
    case 2:
        func_0203ba2c(self);
        break;
    case 3:
        func_0203ba58(self);
        break;
    case 4:
        break;
    }
    func_0203b99c(self);
}

// Starts loading tool set `set`.
int func_0203b518(PlayerSprite *self, int set)
{
    if (self->state != 4) {
        return -1;
    }
    if (set >= 0x10) {
        return -2;
    }
    if (self->set == set) {
        return -3;
    }
    func_0203b890(self);
    self->set = set;
    self->state = 0;
    return 1;
}

// Shows sprite `no` playing animation `seq`.
#ifdef NONMATCHING
int func_0203b568(PlayerSprite *self, int no, int seq)
{
    if (self->state != 4) {
        return -1;
    }
    if (self->set == 0) {
        if (no >= data_020b2a4c[self->set + 1]) {
            return -2;
        }
    } else if (no >= self->base + self->count) {
        return -2;
    }
    if (self->sprites[no] == NULL) {
        return -3;
    }
    int mainCount = data_020b2a4c[1];
    ArchiveView arc;
    _ZN7ArchiveC1EPv(&arc, no < mainCount ? self->mainData : self->toolData);
    self->cur = self->sprites[no];
    MI_UncompressLZ8(func_0203bf28(self, &arc, no < mainCount ? no : no - self->base, 0), Ov009_GetWork());
    DC_FlushRange(Ov009_GetWork(), 0x10000);
    NNS_G2dInitImageProxy((NNSG2dImageProxy *)self->cur->unk58);
    NNS_G2dGetUnpackedCharacterData(Ov009_GetWork(), (NNSG2dCharacterData **)&self->cur->unk54);
    NNS_G2dLoadImageVramTransfer((NNSG2dCharacterData *)self->cur->unk54, NNS_GfdGetTexKeyAddr(self->cur->unk7C),
                                 NNS_G2D_VRAM_TYPE_3DMAIN, (NNSG2dImageProxy *)self->cur->unk58);
    self->cur->ncgr = Ov009_GetWork();
    Gfx2D *gfx;
    Sprite2D *s;
    NNSG2dCharacterData *chr;
    s = self->cur;
    gfx = gGfx2D;
    chr = (NNSG2dCharacterData *)s->unk54;
    gfx->InitCellAnim(self->cur->cellAnim, NNS_G2dGetAnimSequenceByIdx(s->animBank, 0), s->cellBank,
                      s->transferHandle, NNS_GfdGetTexKeyAddr(s->unk7C), NNS_G2D_VRAM_ADDR_NONE, NNS_G2D_VRAM_ADDR_NONE, NULL, chr->pRawData,
                         chr->szByte);
    func_0203b744(self, seq);
    self->cur->cellAnim->animCtrl.bActive = TRUE;
    self->curNo = no;
    return 1;
}
#else
// Linker names used by the asm below.
extern "C" {
void _Z13Ov009_GetWorkv(void);
void _ZN5Gfx2D12InitCellAnimEP19NNSG2dCellAnimationPK22NNSG2dAnimSequenceDataPK18NNSG2dCellDataBankmmmmPKvS9_m(void);
}
// NONMATCHING: kept as assembly until the C version matches.
asm int func_0203b568(PlayerSprite *self, int no, int seq) {
    stmdb sp!, {r4, r5, r6, r7, r8, r9, lr}
    sub sp, sp, #0x24
    mov r8, r0
    ldr r0, [r8, #0x1e8]
    mov r7, r1
    cmp r0, #0x4
    mov r6, r2
    addne sp, sp, #0x24
    mvnne r0, #0x0
    ldmneia sp!, {r4, r5, r6, r7, r8, r9, pc}
    ldr r1, [r8, #0x1d8]
    cmp r1, #0x0
    bne _0203b5bc
    ldr r0, [pc, #0x194]
    add r1, r1, #0x1
    ldr r0, [r0, r1, lsl #0x2]
    cmp r7, r0
    blt _0203b5d8
    add sp, sp, #0x24
    mvn r0, #0x1
    ldmia sp!, {r4, r5, r6, r7, r8, r9, pc}
_0203b5bc:
    ldr r1, [r8, #0x1dc]
    ldr r0, [r8, #0x1e0]
    add r0, r1, r0
    cmp r7, r0
    addge sp, sp, #0x24
    mvnge r0, #0x1
    ldmgeia sp!, {r4, r5, r6, r7, r8, r9, pc}
_0203b5d8:
    add r0, r8, r7, lsl #0x2
    ldr r0, [r0, #0xb0]
    cmp r0, #0x0
    addeq sp, sp, #0x24
    mvneq r0, #0x2
    ldmeqia sp!, {r4, r5, r6, r7, r8, r9, pc}
    ldr r0, [pc, #0x144]
    ldr r4, [r0, #0x4]
    add r0, sp, #0x1c
    cmp r7, r4
    ldrlt r1, [r8, #0x50]
    ldrge r1, [r8, #0xa0]
    bl _ZN7ArchiveC1EPv
    add r0, r8, r7, lsl #0x2
    ldr r0, [r0, #0xb0]
    cmp r7, r4
    str r0, [r8, #0xa4]
    ldrge r0, [r8, #0x1dc]
    movlt r2, r7
    subge r2, r7, r0
    add r1, sp, #0x1c
    mov r0, r8
    mov r3, #0x0
    bl func_0203bf28
    mov r4, r0
    bl _Z13Ov009_GetWorkv
    mov r1, r0
    mov r0, r4
    bl MI_UncompressLZ8
    bl _Z13Ov009_GetWorkv
    mov r1, #0x10000
    bl DC_FlushRange
    ldr r0, [r8, #0xa4]
    add r0, r0, #0x58
    bl NNS_G2dInitImageProxy
    bl _Z13Ov009_GetWorkv
    ldr r1, [r8, #0xa4]
    add r1, r1, #0x54
    bl NNS_G2dGetUnpackedCharacterData
    ldr r3, [r8, #0xa4]
    mov r2, #0x0
    ldr r1, [r3, #0x7c]
    ldr r0, [r3, #0x54]
    mov r1, r1, lsl #0x10
    add r3, r3, #0x58
    mov r1, r1, lsr #0xd
    bl NNS_G2dLoadImageVramTransfer
    bl _Z13Ov009_GetWorkv
    ldr r2, [r8, #0xa4]
    ldr r1, [pc, #0x9c]
    str r0, [r2, #0x24]
    ldr r4, [r8, #0xa4]
    ldr r9, [r1, #0x0]
    ldr r0, [r4, #0x4c]
    mov r1, #0x0
    ldr r5, [r4, #0x54]
    bl NNS_G2dGetAnimSequenceByIdx
    ldr r1, [r4, #0x40]
    mov r2, r0
    str r1, [sp, #0x0]
    ldr ip, [r4, #0x7c]
    mvn r3, #0x0
    mov ip, ip, lsl #0x10
    mov ip, ip, lsr #0xd
    str ip, [sp, #0x4]
    str r3, [sp, #0x8]
    mov r1, #0x0
    str r3, [sp, #0xc]
    str r1, [sp, #0x10]
    ldr r1, [r5, #0x14]
    mov r0, r9
    str r1, [sp, #0x14]
    ldr r1, [r5, #0x10]
    str r1, [sp, #0x18]
    ldr r1, [r8, #0xa4]
    ldr r3, [r4, #0x48]
    ldr r1, [r1, #0x50]
    bl _ZN5Gfx2D12InitCellAnimEP19NNSG2dCellAnimationPK22NNSG2dAnimSequenceDataPK18NNSG2dCellDataBankmmmmPKvS9_m
    mov r1, r6
    mov r0, r8
    bl func_0203b744
    ldr r1, [r8, #0xa4]
    mov r0, #0x1
    ldr r1, [r1, #0x50]
    str r0, [r1, #0xc]
    str r7, [r8, #0x1e4]
    add sp, sp, #0x24
    ldmia sp!, {r4, r5, r6, r7, r8, r9, pc}
    dcd data_020b2a4c
    dcd data_020b2a4c
    dcd gGfx2D
}
#endif

int func_0203b744(PlayerSprite *self, int seq)
{
    if (seq >= self->cur->animBank->numSequences) {
        return -1;
    }
    NNS_G2dSetCellAnimationSequence(self->cur->cellAnim, NNS_G2dGetAnimSequenceByIdx(self->cur->animBank, seq));
    return 1;
}

// Frees the main set.
void func_0203b788(PlayerSprite *self)
{
    delete[] self->mainData;
    int n = data_020b2a4c[1];
    for (int i = 0; i < n; i++) {
        if (self->sprites[i]->transferHandle != (u32)-1) {
            gGfx2D->FreeTransferHandle(self->sprites[i]->transferHandle);
            self->sprites[i]->transferHandle = -1;
        }
        if (self->sprites[i]->cellAnim != NULL) {
            delete self->sprites[i]->cellAnim;
        }
        if (i == func_0203bb28(self, 0)) {
            NNS_GfdFreeTexVram(self->sprites[i]->unk7C);
        }
        func_0202d370(self->sprites[i]->unk94);
        func_0202d3d8(self->sprites[i]->unk3C, self->sprites[i]->ncerLoaded);
        func_0202d40c((void *)self->sprites[i]->unk44, self->sprites[i]->nanrLoaded);
        delete self->sprites[i];
        self->sprites[i] = NULL;
    }
}

// Frees the tool set.
void func_0203b890(PlayerSprite *self)
{
    if (self->set == 0) {
        return;
    }
    delete[] self->toolData;
    for (int i = 0; i < self->count; i++) {
        Sprite2D *s = func_0203bb14(self, i);
        if (s->transferHandle != (u32)-1) {
            gGfx2D->FreeTransferHandle(s->transferHandle);
            s->transferHandle = -1;
        }
        if (s->cellAnim != NULL) {
            delete s->cellAnim;
        }
        if (i == func_0203bb28(self, self->set)) {
            NNS_GfdFreeTexVram(s->unk7C);
        }
        func_0202d370(s->unk94);
        func_0202d3d8(s->unk3C, s->ncerLoaded);
        func_0202d40c((void *)s->unk44, s->nanrLoaded);
        delete s;
        func_0203bb00(self, i, NULL);
    }
    self->set = 0;
}

BOOL func_0203b980(PlayerSprite *self)
{
    return self->cur->cellAnim->animCtrl.bActive != 0;
}

void func_0203b99c(PlayerSprite *self)
{
    func_0202b1d0(self->cur, 0x40);
    func_0202bc20(gRenderer2D, self->cur);
}

BOOL func_0203b9c8(PlayerSprite *self)
{
    self->state = 1;
    return TRUE;
}

BOOL func_0203b9d8(PlayerSprite *self)
{
    self->toolData = (u8 *)GFile_LoadAsync(
        &self->toolFile, func_0203366c(data_020f57c0, 0) == 0 ? data_020bd9b8[self->set] : data_020bd9f8[self->set]);
    self->state = 2;
    return TRUE;
}

BOOL func_0203ba2c(PlayerSprite *self)
{
    if (!GFile_IsBusy(&self->toolFile)) {
        self->state = 3;
        return TRUE;
    }
    return FALSE;
}

BOOL func_0203ba58(PlayerSprite *self)
{
    ArchiveView arc;
    _ZN7ArchiveC1EPv(&arc, self->toolData);
    self->tick = OS_GetTick();
    func_0203bb34(self, &arc);
    self->tick = OS_GetTick() - self->tick;
    self->state = 4;
    return TRUE;
}

int func_0203bab8(PlayerSprite *self, int set)
{
    return data_020b2a4c[set + 1];
}

int func_0203bacc(PlayerSprite *self)
{
    int sum = 0;
    for (int i = 0; i <= self->set; i++) {
        sum += data_020b2a4c[i];
    }
    return sum;
}

void func_0203bb00(PlayerSprite *self, int i, Sprite2D *s)
{
    self->sprites[self->base + i] = s;
}

Sprite2D *func_0203bb14(PlayerSprite *self, int i)
{
    return self->sprites[self->base + i];
}

int func_0203bb28(PlayerSprite *self, int set)
{
    return self->biggest[set];
}

// Sets up the sprites of the current set from an archive.
void func_0203bb34(PlayerSprite *self, ArchiveView *arc)
{
    int n = func_0203bab8(self, self->set);
    self->base = func_0203bacc(self);
    func_0203bba4(self, arc);
    func_0203bc90(self, arc);
    func_0203bd1c(self, arc);
    func_0203bdc0(self, arc, 0);
    func_0203be7c(self);
    if (self->base != 0) {
        self->count = n;
    }
}

void func_0203bba4(PlayerSprite *self, ArchiveView *arc)
{
    int n = func_0203bab8(self, self->set);
    int big = 0;
    Sprite2D *bigSprite = NULL;
    for (int i = 0; i < n; i++) {
        Sprite2D *s = new Sprite2D;
        void *ncer = func_0203bf28(self, arc, i, 1);
        func_0202cff4(&s->cellBank, ncer, &s->unk38, (const char *)func_02037058(arc, func_0203bfd8(self, arc, i, 1)),
                      &s->unk3C, &s->ncerLoaded);
        s->ncer = ncer;
        if (bigSprite == NULL || bigSprite->unk38 < s->unk38) {
            bigSprite = s;
            big = i;
        }
        self->sprites[self->base + i] = s;
    }
    self->biggest[self->set] = big;
}

// All the sprites of the set share the VRAM of the biggest one.
void func_0203bc90(PlayerSprite *self, ArchiveView *arc)
{
    int n = func_0203bab8(self, self->set);
    int big = self->biggest[self->set];
    Sprite2D *s = self->sprites[self->base + big];
    s->unk7C = NNS_GfdAllocTexVram(s->unk38, FALSE, 0);
    for (int i = 0; i < n; i++) {
        if (i != big) {
            self->sprites[self->base + i]->unk7C = s->unk7C;
        }
    }
}

void func_0203bd1c(PlayerSprite *self, ArchiveView *arc)
{
    int n = func_0203bab8(self, self->set);
    for (int i = 0; i < n; i++) {
        Sprite2D *s = self->sprites[self->base + i];
        void *nanr = func_0203bf28(self, arc, i, 2);
        func_0202d150(&s->animBank, nanr, (const char *)func_02037058(arc, func_0203bfd8(self, arc, i, 2)),
                      &s->unk44, &s->nanrLoaded);
        s->nanr = nanr;
    }
}

void func_0203bdc0(PlayerSprite *self, ArchiveView *arc, int no)
{
    int n = func_0203bab8(self, self->set);
    for (int i = 0; i < n; i++) {
        Sprite2D *s = self->sprites[self->base + i];
        void *nclr = func_0203bf28(self, arc, no, -1);
        DC_FlushRange(nclr, func_0203bf80(self, arc, no, -1));
        func_0202e22c(s->unk80, nclr, &s->unk94, (const char *)func_02037058(arc, func_0203bfd8(self, arc, no, -1)));
        s->nclr = nclr;
    }
}

void func_0203be7c(PlayerSprite *self)
{
    int n = func_0203bab8(self, self->set);
    for (int i = 0; i < n; i++) {
        Sprite2D *s = self->sprites[self->base + i];
        s->transferHandle = gGfx2D->GetNewTransferHandle();
        s->cellAnim = NULL;
        s->cellAnim = new NNSG2dCellAnimation;
        s->z = 0;
        s->priority = 0;
        s->numCells = 0;
        s->rotation = 0;
        s->scaleX = FX32_ONE;
        s->scaleY = FX32_ONE;
        s->x = 0x80000;
        s->y = 0x60000;
        s->alpha = 31;
        func_0202b1e4(s, 2);
    }
}

// File `type` (0 characters, 1 cells, 2 animations, -1 palette of the set) of sprite i.
void *func_0203bf28(PlayerSprite *self, ArchiveView *arc, int i, int type)
{
    int idx = 0;
    switch (type) {
    case 0:
    case 1:
    case 2:
        idx = type + i * 3;
        break;
    case -1:
        idx = i + func_0203bab8(self, self->set) * 3;
        break;
    }
    return func_02036fb0(arc, idx);
}

u32 func_0203bf80(PlayerSprite *self, ArchiveView *arc, int i, int type)
{
    int idx = 0;
    switch (type) {
    case 0:
    case 1:
    case 2:
        idx = type + i * 3;
        break;
    case -1:
        idx = i + func_0203bab8(self, self->set) * 3;
        break;
    }
    return func_02036ff8(arc, idx);
}

int func_0203bfd8(PlayerSprite *self, ArchiveView *arc, int i, int type)
{
    int idx = 0;
    switch (type) {
    case 0:
    case 1:
    case 2:
        idx = type + i * 3;
        break;
    case -1:
        idx = i + func_0203bab8(self, self->set) * 3;
        break;
    }
    return idx;
}

void func_0203c028(PlayerSprite *self, BOOL on)
{
    if (on) {
        func_0202b1d0(self->cur, 4);
    } else {
        func_0202b1f0(self->cur, 4);
    }
}

}

// The 2D sprites and the 2D renderer (ARM9 0x0202aec0..0x0202bed0). Names are ours.
// Functions called by not-yet-decompiled code keep their dsd names (declared in Sprite2D.h).
#include <nnsys/g2d.h>
#include <nnsys/g2d/load/g2d_NAN_load.h>
#include <nnsys/g3d.h>
#include "Sprite2D.h"
#include "Gfx2D.h"
#include "game_file.h"

extern "C" {
// Resource loaders (ARM9 0x0202c050..).
void *func_0202c050(void *proxy, void *chr, const char *path, u32 *vram, void *a, u8 *loaded, u32 size, BOOL keep);
void *func_0202c454(void *proxy, const char *path, void *b, BOOL keep);
void *func_0202c72c(const NNSG2dCellDataBank **bank, const char *path, u32 *a, void *b, u8 *loaded, BOOL keep);
void *func_0202c898(const NNSG2dAnimBankData **bank, const char *path, void *a, u8 *loaded, BOOL keep);
void func_0202cff4(const NNSG2dCellDataBank **bank, void *ncer, u32 *a, const char *key, void *b, u8 *loaded);
void func_0202d150(const NNSG2dAnimBankData **bank, void *nanr, const char *key, void *a, u8 *loaded);
void func_0202e00c(void *proxy, void *chr, void *ncbr, u32 *vram, const char *key, void *a, u8 *loaded);
void func_0202e22c(void *proxy, void *nclr, void *b, const char *key);
BOOL func_0202d284(void *a, u8 loaded, u32 vram);
BOOL func_0202d370(void *a);
BOOL func_0202d3d8(void *a, u8 loaded);
BOOL func_0202d40c(void *a, u8 loaded);
void *func_02036fb0(void *arc, const char *name);   // file data in an archive
void *func_02037058(void *arc, const char *name);   // file key/size in an archive
const char *func_0204449c(int slot, const char *name);
BOOL func_01ff8fa0(const VecFx32 *pos, int *x, int *y);   // 3D position to screen (ITCM)
void func_01ff8ab8(void *renderer, Sprite2D *sprite);       // draw one sprite (ITCM)
}

// Out-of-line copies of inline functions that the original emits here: GFile's complete destructor and
// the destructors of a resource holder class (vtable 0x020bc384); kept as asm under their linker names.
extern "C" {
void _Z11GFile_CloseP5GFile(void);
void _ZdlPv(void);
void func_0202aec0(void);
void func_0202afe0(void);
void func_0202aee0(void);
void func_0202af0c(void);

// GFile's vtable (its key function is never defined, see game_file.h) and the resource holder's.
void *_ZTV11GFileObject[4] = {NULL, NULL, (void *)func_0202aec0, (void *)func_0202afe0};
void *_ZTV9SpriteRes[4] = {NULL, NULL, (void *)func_0202aee0, (void *)func_0202af0c};
}
extern "C" asm void func_0202aec0(void) {
    stmdb sp!, {r4, lr}
    ldr r1, [pc, #0x10]
    mov r4, r0
    str r1, [r4, #0x0]
    bl _Z11GFile_CloseP5GFile
    mov r0, r4
    ldmia sp!, {r4, pc}
    dcd _ZTV11GFileObject+8
}
extern "C" asm void func_0202aee0(void) {
    stmdb sp!, {r4, lr}
    ldr r1, [pc, #0x1c]
    mov r4, r0
    str r1, [r4, #0x0]
    ldr r0, [r4, #0x4]
    bl _ZdlPv
    ldr r0, [r4, #0x24]
    bl _ZdlPv
    mov r0, r4
    ldmia sp!, {r4, pc}
    dcd _ZTV9SpriteRes+8
}
extern "C" asm void func_0202af0c(void) {
    stmdb sp!, {r4, lr}
    ldr r1, [pc, #0x24]
    mov r4, r0
    str r1, [r4, #0x0]
    ldr r0, [r4, #0x4]
    bl _ZdlPv
    ldr r0, [r4, #0x24]
    bl _ZdlPv
    mov r0, r4
    bl _ZdlPv
    mov r0, r4
    ldmia sp!, {r4, pc}
    dcd _ZTV9SpriteRes+8
}
extern "C" asm void func_0202af40(void) {
    str r1, [r0, #0x10]
    bx lr
}

// Index of the highest set bit, mapped through a table (texture size enums).
extern "C" int func_0202af48(u32 size)
{
    const int sizes[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    u8 i = 0;

    for (;;) {
        size >>= 1;
        if (size == 0) {
            break;
        }
        i++;
    }
    return sizes[i];
}

extern "C" int func_0202af94(u32 size)
{
    const int sizes[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    u8 i = 0;

    for (;;) {
        size >>= 1;
        if (size == 0) {
            break;
        }
        i++;
    }
    return sizes[i];
}

extern "C" asm void func_0202afe0(void) {
    stmdb sp!, {r4, lr}
    ldr r1, [pc, #0x18]
    mov r4, r0
    str r1, [r4, #0x0]
    bl _Z11GFile_CloseP5GFile
    mov r0, r4
    bl _ZdlPv
    mov r0, r4
    ldmia sp!, {r4, pc}
    dcd _ZTV11GFileObject+8
}

extern "C" void func_0202b008(fx32 *a, fx32 *b, fx32 *c, fx32 *d, BOOL decA, BOOL decB)
{
    if (decA) {
        *a -= FX32_ONE;
        *c -= FX32_ONE;
    }
    if (decB) {
        *b -= FX32_ONE;
        *d -= FX32_ONE;
    }
}

extern "C" void func_0202b054(Sprite2D *sprite)
{
    sprite->cellAnim->animCtrl.bActive = TRUE;
}

extern "C" void func_0202b064(Sprite2D *sprite, u16 seq)
{
    sprite->animSeq = seq;
    NNS_G2dSetCellAnimationSequence(sprite->cellAnim, NNS_G2dGetAnimSequenceByIdx(sprite->animBank, seq));
}

extern "C" void func_0202b088(Sprite2D *sprite, fx32 speed)
{
    sprite->cellAnim->animCtrl.speed = speed;
}

extern "C" BOOL func_0202b094(Sprite2D *sprite)
{
    NNS_G2dGetAnimCtrlCurrentFrame(&sprite->cellAnim->animCtrl);
    return sprite->cellAnim->animCtrl.bActive != 0;
}

extern "C" BOOL func_0202b0bc(Sprite2D *sprite, int seq)
{
    return sprite->animBank->pSequenceArrayHead[seq].playMode == 2;
}

extern "C" u16 func_0202b0dc(Sprite2D *sprite)
{
    return NNS_G2dGetAnimCtrlCurrentFrame(&sprite->cellAnim->animCtrl);
}

extern "C" void func_0202b0ec(Sprite2D *sprite, int priority)
{
    sprite->priority = priority << FX32_SHIFT;
}

extern "C" void func_0202b0f8(Sprite2D *sprite, fx32 priority)
{
    sprite->priority = priority;
}

extern "C" void func_0202b100(Sprite2D *sprite, int x, int y)
{
    sprite->x = x << FX32_SHIFT;
    sprite->y = y << FX32_SHIFT;
}

extern "C" void func_0202b114(Sprite2D *sprite, int x)
{
    sprite->x = x << FX32_SHIFT;
}

extern "C" void func_0202b120(Sprite2D *sprite, int y)
{
    sprite->y = y << FX32_SHIFT;
}

extern "C" int func_0202b12c(Sprite2D *sprite)
{
    return sprite->x >> FX32_SHIFT;
}

extern "C" int func_0202b138(Sprite2D *sprite)
{
    return sprite->y >> FX32_SHIFT;
}

extern "C" void func_0202b144(Sprite2D *sprite, int dy)
{
    sprite->y += dy << FX32_SHIFT;
}

extern "C" BOOL func_0202b154(Sprite2D *sprite, const VecFx32 *pos)
{
    int x;
    int y;
    BOOL ret = func_01ff8fa0(pos, &x, &y);

    sprite->x = x << FX32_SHIFT;
    sprite->y = y << FX32_SHIFT;
    sprite->z = pos->z;
    sprite->priority = pos->z;
    return ret;
}

extern "C" void func_0202b1a4(Sprite2D *sprite, fx32 x, fx32 y, fx32 z)
{
    VecFx32 pos;

    pos.x = x;
    pos.y = y;
    pos.z = z;
    sprite->z = z;
    sprite->priority = z;
    func_0202b154(sprite, &pos);
}

extern "C" void func_0202b1d0(Sprite2D *sprite, u16 flags)
{
    sprite->flags |= flags;
}

extern "C" void func_0202b1e4(Sprite2D *sprite, u16 flags)
{
    sprite->flags = flags;
}

extern "C" void func_0202b1f0(Sprite2D *sprite, u16 flags)
{
    sprite->flags &= ~flags;
}

extern "C" Sprite2D *func_0202b208(void *arc, const char *ncbr, const char *nclr, const char *ncer, const char *nanr,
                                   int x, int y, int priority, int seq)
{
    Sprite2D *sprite = func_0202b41c(func_02036fb0(arc, ncbr), func_02036fb0(arc, nclr), func_02036fb0(arc, ncer),
                                     func_02036fb0(arc, nanr), (const char *)func_02037058(arc, ncbr),
                                     (const char *)func_02037058(arc, nclr), (const char *)func_02037058(arc, ncer),
                                     (const char *)func_02037058(arc, nanr), x, y);

    if (seq != -1) {
        func_0202b064(sprite, seq);
    }
    if (priority != -1) {
        func_0202b0ec(sprite, priority);
    }
    return sprite;
}

extern "C" Sprite2D *func_0202b31c(void *ncbr, void *nclr, void *ncer, const char *ncbrKey, const char *nclrKey,
                                   const char *ncerKey, s16 x, s16 y)
{
    Sprite2D *sprite = new Sprite2D;

    func_0202cff4(&sprite->cellBank, ncer, &sprite->unk38, ncerKey, &sprite->unk3C, &sprite->ncerLoaded);
    func_0202e00c(sprite->unk58, &sprite->unk54, ncbr, &sprite->unk7C, ncbrKey, &sprite->unk34, &sprite->ncgrLoaded);
    func_0202e22c(sprite->unk80, nclr, &sprite->unk94, nclrKey);
    sprite->ncer = ncer;
    sprite->ncgr = ncbr;
    sprite->nclr = nclr;
    sprite->transferHandle = -1;
    sprite->cellAnim = NULL;
    sprite->rotation = 0;
    sprite->scaleX = FX32_ONE;
    sprite->scaleY = FX32_ONE;
    sprite->x = x << FX32_SHIFT;
    sprite->y = y << FX32_SHIFT;
    sprite->alpha = 31;
    sprite->flags = 8;
    sprite->z = 0;
    sprite->priority = 0;
    sprite->numCells = sprite->cellBank->pCellDataArrayHead->numOAMAttrs;
    return sprite;
}

extern "C" Sprite2D *func_0202b41c(void *ncbr, void *nclr, void *ncer, void *nanr, const char *ncbrKey,
                                   const char *nclrKey, const char *ncerKey, const char *nanrKey, s16 x, s16 y)
{
    Sprite2D *sprite = new Sprite2D;

    func_0202cff4(&sprite->cellBank, ncer, &sprite->unk38, ncerKey, &sprite->unk3C, &sprite->ncerLoaded);
    func_0202e00c(sprite->unk58, &sprite->unk54, ncbr, &sprite->unk7C, ncbrKey, &sprite->unk34, &sprite->ncgrLoaded);
    func_0202e22c(sprite->unk80, nclr, &sprite->unk94, nclrKey);
    func_0202d150(&sprite->animBank, nanr, nanrKey, &sprite->unk44, &sprite->nanrLoaded);
    sprite->ncer = ncer;
    sprite->ncgr = ncbr;
    sprite->nclr = nclr;
    sprite->nanr = nanr;
    sprite->transferHandle = -1;
    sprite->cellAnim = NULL;
    sprite->cellAnim = new NNSG2dCellAnimation;
    NNS_G2dInitCellAnimation(sprite->cellAnim, NNS_G2dGetAnimSequenceByIdx(sprite->animBank, 0), sprite->cellBank);
    sprite->rotation = 0;
    sprite->scaleX = FX32_ONE;
    sprite->scaleY = FX32_ONE;
    sprite->x = x << FX32_SHIFT;
    sprite->y = y << FX32_SHIFT;
    sprite->alpha = 31;
    sprite->flags = 8;
    sprite->z = 0;
    sprite->priority = 0;
    sprite->numCells = sprite->cellBank->pCellDataArrayHead->numOAMAttrs;
    return sprite;
}

#ifdef NONMATCHING
extern "C" Sprite2D *func_0202b56c(const char *ncbr, const char *nclr, const char *ncer, const char *nanr, s16 x,
                                   s16 y)
{
    Sprite2D *sprite = new Sprite2D;
    NNSG2dCharacterData *chr;

    sprite->ncer = func_0202c72c(&sprite->cellBank, ncer, &sprite->unk38, &sprite->unk3C, &sprite->ncerLoaded, FALSE);
    sprite->ncgr = func_0202c050(sprite->unk58, &sprite->unk54, ncbr, &sprite->unk7C, &sprite->unk34,
                                 &sprite->ncgrLoaded, sprite->unk38, FALSE);
    sprite->nclr = func_0202c454(sprite->unk80, nclr, &sprite->unk94, FALSE);
    sprite->nanr = func_0202c898(&sprite->animBank, nanr, &sprite->unk44, &sprite->nanrLoaded, FALSE);
    sprite->transferHandle = gGfx2D->GetNewTransferHandle();
    sprite->cellAnim = NULL;
    sprite->cellAnim = new NNSG2dCellAnimation;
    chr = (NNSG2dCharacterData *)sprite->unk54;
    gGfx2D->InitCellAnim(sprite->cellAnim, NNS_G2dGetAnimSequenceByIdx(sprite->animBank, 0), sprite->cellBank,
                         sprite->transferHandle, (u16)sprite->unk7C << 3, (u32)-1, (u32)-1, NULL, chr->pRawData,
                         chr->szByte);
    sprite->z = 0;
    sprite->priority = 0;
    sprite->numCells = sprite->cellBank->pCellDataArrayHead->numOAMAttrs;
    sprite->rotation = 0;
    sprite->scaleX = FX32_ONE;
    sprite->scaleY = FX32_ONE;
    sprite->x = x << FX32_SHIFT;
    sprite->y = y << FX32_SHIFT;
    sprite->alpha = 31;
    sprite->flags = 0;
    return sprite;
}
#else
// Linker names used by the asm below.
extern "C" {
void _ZN5Gfx2D12InitCellAnimEP19NNSG2dCellAnimationPK22NNSG2dAnimSequenceDataPK18NNSG2dCellDataBankmmmmPKvS9_m(void);
void _ZN5Gfx2D20GetNewTransferHandleEv(void);
void _Znwm(void);
}
// NONMATCHING: kept as assembly until the C version matches.
extern "C" asm Sprite2D *func_0202b56c(const char *ncbr, const char *nclr, const char *ncer, const char *nanr, s16 x,
                                   s16 y) {
    stmdb sp!, {r3, r4, r5, r6, r7, r8, lr}
    sub sp, sp, #0x1c
    mov r8, r0
    mov r0, #0x17c
    mov r6, r1
    mov r7, r2
    mov r5, r3
    bl _Znwm
    mov r4, r0
    add r0, r4, #0x22
    str r0, [sp, #0x0]
    mov r0, #0x0
    str r0, [sp, #0x4]
    mov r1, r7
    add r0, r4, #0x48
    add r2, r4, #0x38
    add r3, r4, #0x3c
    bl func_0202c72c
    mov r2, r8
    str r0, [r4, #0x2c]
    add r0, r4, #0x34
    str r0, [sp, #0x0]
    add r0, r4, #0x21
    str r0, [sp, #0x4]
    ldr r1, [r4, #0x38]
    mov r0, #0x0
    str r1, [sp, #0x8]
    str r0, [sp, #0xc]
    add r0, r4, #0x58
    add r1, r4, #0x54
    add r3, r4, #0x7c
    bl func_0202c050
    mov r1, r6
    str r0, [r4, #0x24]
    add r0, r4, #0x80
    add r2, r4, #0x94
    mov r3, #0x0
    bl func_0202c454
    mov r1, r5
    str r0, [r4, #0x28]
    mov r0, #0x0
    str r0, [sp, #0x0]
    add r0, r4, #0x4c
    add r2, r4, #0x44
    add r3, r4, #0x20
    bl func_0202c898
    str r0, [r4, #0x30]
    ldr r0, [pc, #0xe4]
    ldr r0, [r0, #0x0]
    bl _ZN5Gfx2D20GetNewTransferHandleEv
    str r0, [r4, #0x40]
    mov r0, #0x0
    str r0, [r4, #0x50]
    mov r0, #0x58
    bl _Znwm
    str r0, [r4, #0x50]
    ldr r0, [pc, #0xc0]
    ldr r5, [r4, #0x54]
    ldr r6, [r0, #0x0]
    ldr r0, [r4, #0x4c]
    mov r1, #0x0
    bl NNS_G2dGetAnimSequenceByIdx
    mov r2, r0
    mov r0, r6
    ldr r1, [r4, #0x40]
    mvn r3, #0x0
    str r1, [sp, #0x0]
    ldr r6, [r4, #0x7c]
    mov r1, #0x0
    mov r6, r6, lsl #0x10
    mov r6, r6, lsr #0xd
    str r6, [sp, #0x4]
    str r3, [sp, #0x8]
    str r3, [sp, #0xc]
    str r1, [sp, #0x10]
    ldr r1, [r5, #0x14]
    str r1, [sp, #0x14]
    ldr r1, [r5, #0x10]
    str r1, [sp, #0x18]
    ldr r1, [r4, #0x50]
    ldr r3, [r4, #0x48]
    bl _ZN5Gfx2D12InitCellAnimEP19NNSG2dCellAnimationPK22NNSG2dAnimSequenceDataPK18NNSG2dCellDataBankmmmmPKvS9_m
    mov r5, #0x0
    str r5, [r4, #0x14]
    str r5, [r4, #0x18]
    ldr r0, [r4, #0x48]
    ldrsh r1, [sp, #0x38]
    ldr r2, [r0, #0x4]
    ldrsh r0, [sp, #0x3c]
    ldrh r3, [r2, #0x0]
    mov r2, #0x1000
    mov r1, r1, lsl #0xc
    strh r3, [r4, #0x0]
    strh r5, [r4, #0x2]
    str r2, [r4, #0x4]
    str r2, [r4, #0x8]
    str r1, [r4, #0xc]
    mov r0, r0, lsl #0xc
    str r0, [r4, #0x10]
    mov r0, #0x1f
    strb r0, [r4, #0x1e]
    add r1, r4, #0x100
    mov r0, r4
    strh r5, [r1, #0x78]
    add sp, sp, #0x1c
    ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
    dcd gGfx2D
}
#endif

extern "C" Sprite2D *func_0202b718(const char *ncbr, const char *nclr, const char *ncer, int x, s16 y)
{
    Sprite2D *sprite = new Sprite2D;

    sprite->ncer = func_0202c72c(&sprite->cellBank, ncer, &sprite->unk38, &sprite->unk3C, &sprite->ncerLoaded, FALSE);
    sprite->ncgr = func_0202c050(sprite->unk58, &sprite->unk54, ncbr, &sprite->unk7C, &sprite->unk34,
                                 &sprite->ncgrLoaded, sprite->unk38, FALSE);
    sprite->nclr = func_0202c454(sprite->unk80, nclr, &sprite->unk94, FALSE);
    sprite->nanr = NULL;
    sprite->transferHandle = -1;
    sprite->cellAnim = NULL;
    sprite->rotation = 0;
    sprite->scaleX = FX32_ONE;
    sprite->scaleY = FX32_ONE;
    sprite->x = x << FX32_SHIFT;
    sprite->y = y << FX32_SHIFT;
    sprite->alpha = 31;
    sprite->flags = 8;
    sprite->z = 0;
    sprite->priority = 0;
    sprite->numCells = sprite->cellBank->pCellDataArrayHead->numOAMAttrs;
    return sprite;
}

extern "C" Sprite2D *func_0202b824(const char *ncbr, const char *nclr, const char *ncer, const char *nanr, s16 x,
                                   s16 y, BOOL keep)
{
    Sprite2D *sprite = new Sprite2D;

    sprite->ncer = func_0202c72c(&sprite->cellBank, ncer, &sprite->unk38, &sprite->unk3C, &sprite->ncerLoaded, keep);
    sprite->ncgr = func_0202c050(sprite->unk58, &sprite->unk54, ncbr, &sprite->unk7C, &sprite->unk34,
                                 &sprite->ncgrLoaded, sprite->unk38, keep);
    sprite->nclr = func_0202c454(sprite->unk80, nclr, &sprite->unk94, keep);
    sprite->nanr = func_0202c898(&sprite->animBank, nanr, &sprite->unk44, &sprite->nanrLoaded, keep);
    sprite->transferHandle = -1;
    sprite->cellAnim = NULL;
    sprite->cellAnim = new NNSG2dCellAnimation;
    NNS_G2dInitCellAnimation(sprite->cellAnim, NNS_G2dGetAnimSequenceByIdx(sprite->animBank, 0), sprite->cellBank);
    sprite->rotation = 0;
    sprite->scaleX = FX32_ONE;
    sprite->scaleY = FX32_ONE;
    sprite->x = x << FX32_SHIFT;
    sprite->y = y << FX32_SHIFT;
    sprite->alpha = 31;
    sprite->flags = 8;
    sprite->z = 0;
    sprite->priority = 0;
    sprite->numCells = sprite->cellBank->pCellDataArrayHead->numOAMAttrs;
    return sprite;
}

extern "C" Sprite2D *func_0202b970(const char *ncbr, const char *nclr, const char *ncer, const char *nanr, int x,
                                   int y, u8 keep)
{
    const char *ncbrPath = func_0204449c(0, ncbr);
    const char *nclrPath = func_0204449c(1, nclr);
    const char *ncerPath = func_0204449c(2, ncer);
    const char *nanrPath = func_0204449c(3, nanr);

    return func_0202b824(ncbrPath, nclrPath, ncerPath, nanrPath, x, y, keep);
}

extern "C" void func_0202ba04(Sprite2D *sprite, BOOL keepData)
{
    if (sprite == NULL) {
        return;
    }
    if (sprite->transferHandle != (u32)-1) {
        gGfx2D->FreeTransferHandle(sprite->transferHandle);
        sprite->transferHandle = -1;
    }
    if (func_0202d3d8(sprite->unk3C, sprite->ncerLoaded) && !keepData) {
        delete sprite->ncer;
    }
    if (sprite->cellAnim != NULL) {
        delete sprite->cellAnim;
    }
    if (sprite->nanr != NULL && func_0202d40c(sprite->unk44, sprite->nanrLoaded) && !keepData) {
        delete sprite->nanr;
    }
    if (func_0202d284(sprite->unk34, sprite->ncgrLoaded, sprite->unk7C) && !keepData && sprite->ncgr != NULL) {
        delete sprite->ncgr;
    }
    if (func_0202d370(sprite->unk94) && !keepData && sprite->nclr != NULL) {
        delete sprite->nclr;
    }
    delete sprite;
}

extern "C" void func_0202bb04(Sprite2D **sprites, int count, BOOL keepData)
{
    int i;

    if (sprites == NULL) {
        return;
    }
    for (i = 0; i < count; i++) {
        func_0202ba04(*sprites, keepData);
        *sprites++ = NULL;
    }
}

extern "C" {
void _ZN9SpriteVecC1Ev(void);
void func_02023818(void);
void func_0202bf10(void);
void func_0202ac98(void);
void func_0202adb0(void);
}

Renderer2D::Renderer2D()
    : unk4(0), unk8(0)
{
    gSpriteDepthStep = 0x5000;
}

Renderer2D::~Renderer2D()
{
}

extern "C" asm void func_0202bbd4(Renderer2D *renderer) {
    stmdb sp!, {r3, lr}
    ldr r1, [r0, #0x1c]
    cmp r1, #0x100
    ldmhsia sp!, {r3, pc}
    add r0, r0, #0x14
    mov r1, #0x100
    bl func_0202ac98
    ldmia sp!, {r3, pc}
}
extern "C" asm void func_0202bbf4(Renderer2D *renderer) {
    stmdb sp!, {r3}
    sub sp, sp, #0x4
    ldr r2, [r0, #0x18]
    mov r1, #0x0
    strb r1, [sp, #0x0]
    ldr r1, [r0, #0x18]
    sub r1, r1, r2
    str r1, [r0, #0x18]
    add sp, sp, #0x4
    ldmia sp!, {r3}
    bx lr
}
extern "C" asm void func_0202bc20(Renderer2D *renderer, Sprite2D *sprite) {
    stmdb sp!, {r0, r1, r2, r3}
    stmdb sp!, {r3, r4, lr}
    sub sp, sp, #0x4
    ldr r2, [sp, #0x14]
    mov r4, r0
    add r0, r2, #0x100
    ldrh r0, [r0, #0x78]
    tst r0, #0x100
    beq _0202bca4
    mov r0, #0x20000
    ldr r1, [r2, #0xc]
    rsb r0, r0, #0x0
    cmp r1, r0
    addlt sp, sp, #0x4
    ldmltia sp!, {r3, r4, lr}
    addlt sp, sp, #0x10
    bxlt lr
    cmp r1, #0x120000
    addgt sp, sp, #0x4
    ldmgtia sp!, {r3, r4, lr}
    addgt sp, sp, #0x10
    bxgt lr
    ldr r1, [r2, #0x10]
    cmp r1, r0
    addlt sp, sp, #0x4
    ldmltia sp!, {r3, r4, lr}
    addlt sp, sp, #0x10
    bxlt lr
    cmp r1, #0xf0000
    addgt sp, sp, #0x4
    ldmgtia sp!, {r3, r4, lr}
    addgt sp, sp, #0x10
    bxgt lr
_0202bca4:
    ldr r3, [sp, #0x14]
    add r0, r3, #0x98
    add r1, r3, #0x130
    add r2, r3, #0x58
    add r3, r3, #0x80
    bl func_0202bf10
    mov r2, #0x0
    strb r2, [sp, #0x1]
    ldr r1, [r4, #0x18]
    ldr r0, [r4, #0x1c]
    cmp r1, r0
    bhs _0202bcfc
    add r0, r1, #0x1
    str r0, [r4, #0x18]
    ldr r2, [sp, #0x14]
    ldr r1, [r4, #0x14]
    sub r0, r0, #0x1
    str r2, [r1, r0, lsl #0x2]
    add sp, sp, #0x4
    ldmia sp!, {r3, r4, lr}
    add sp, sp, #0x10
    bx lr
_0202bcfc:
    strb r2, [sp, #0x0]
    sub r1, sp, #0x4
    and r0, r2, #0xff
    strb r0, [r1, #0x0]
    ldr r2, [r1, #0x0]
    add r1, sp, #0x14
    add r0, r4, #0x14
    bl func_02023818
    add sp, sp, #0x4
    ldmia sp!, {r3, r4, lr}
    add sp, sp, #0x10
    bx lr
}

extern "C" void func_0202bd2c(Renderer2D *renderer, Sprite2D **sprites, int count)
{
    int i;

    for (i = 0; i < count; i++) {
        if (*sprites != NULL) {
            func_0202bc20(renderer, *sprites);
        }
        sprites++;
    }
}

#ifdef NONMATCHING
extern "C" void func_0202bd70(Renderer2D *renderer)
{
    u32 i;

    for (i = 0; i < renderer->mSprites.Size(); i++) {
        Sprite2D *sprite = renderer->mSprites[i];
        if (sprite->cellAnim != NULL && !(sprite->flags & 4)) {
            NNS_G2dTickCellAnimation(sprite->cellAnim, 0x2800);
        }
    }
}
#else
// NONMATCHING: kept as assembly until the C version matches.
extern "C" asm void func_0202bd70(Renderer2D *renderer) {
    stmdb sp!, {r3, r4, r5, r6, r7, lr}
    mov r6, r0
    ldr r0, [r6, #0x18]
    mov r4, #0x0
    cmp r0, #0x0
    ldmlsia sp!, {r3, r4, r5, r6, r7, pc}
    mov r5, r4
    mov r7, #0x2800
_0202bd90:
    ldr r0, [r6, #0x14]
    ldr r1, [r0, r5]
    ldr r0, [r1, #0x50]
    cmp r0, #0x0
    beq _0202bdbc
    add r1, r1, #0x100
    ldrh r1, [r1, #0x78]
    tst r1, #0x4
    bne _0202bdbc
    mov r1, r7
    bl NNS_G2dTickCellAnimation
_0202bdbc:
    ldr r0, [r6, #0x18]
    add r4, r4, #0x1
    cmp r4, r0
    add r5, r5, #0x4
    blo _0202bd90
    ldmia sp!, {r3, r4, r5, r6, r7, pc}
}
#endif

#ifdef NONMATCHING
extern "C" void func_0202bdd4(Renderer2D *renderer)
{
    u32 i;

    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    G3_PushMtx();
    for (i = 0; i < renderer->mSprites.Size(); i++) {
        func_01ff8ab8(renderer, renderer->mSprites[i]);
    }
    G3_PopMtx(1);
    NNS_G3dGeFlushBuffer();
    NNS_G2dUpdateCellTransferStateManager();
}
#else
// NONMATCHING: kept as assembly until the C version matches.
extern "C" asm void func_0202bdd4(Renderer2D *renderer) {
    stmdb sp!, {r4, r5, r6, lr}
    ldr r1, [pc, #0x5c]
    mov r2, #0x2
    str r2, [r1, #0x0]
    mov r4, #0x0
    mov r6, r0
    str r4, [r1, #0x4]
    ldr r0, [r6, #0x18]
    cmp r0, #0x0
    bls _0202be24
    mov r5, r4
_0202be00:
    ldr r1, [r6, #0x14]
    mov r0, r6
    ldr r1, [r1, r5]
    bl func_01ff8ab8
    ldr r0, [r6, #0x18]
    add r4, r4, #0x1
    cmp r4, r0
    add r5, r5, #0x4
    blo _0202be00
_0202be24:
    ldr r0, [pc, #0x14]
    mov r1, #0x1
    str r1, [r0, #0x0]
    bl NNS_G3dGeFlushBuffer
    bl NNS_G2dUpdateCellTransferStateManager
    ldmia sp!, {r4, r5, r6, pc}
    dcd 0x4000440
    dcd 0x4000448
}
#endif

extern "C" asm void func_0202be44(void) {
    ldr r1, [r1, #0x8]
    str r1, [r0, #0xc]
    strb r2, [r0, #0x10]
    bx lr
}

Renderer2DBase::Renderer2DBase()
{
    gRenderer2D = (Renderer2D *)this;
}

#ifdef NONMATCHING
// Inline in the original; the out-of-line copies are in the order D2, (SpriteVec's constructor), D1, D0.
Renderer2DBase::~Renderer2DBase()
{
    gRenderer2D = NULL;
}
#else
// mwcc emits no vtable while the destructor is undefined; it is spelled out in Renderer2DBaseVtable.cpp.
extern "C" asm void _ZN14Renderer2DBaseD2Ev(void) {
    ldr r1, [pc, #0x8]
    mov r2, #0x0
    str r2, [r1, #0x0]
    bx lr
    dcd gRenderer2D
}
extern "C" asm void _ZN9SpriteVecC1Ev(void) {
    stmdb sp!, {r4, lr}
    mov r4, r0
    bl func_0202adb0
    mov r0, r4
    ldmia sp!, {r4, pc}
}
extern "C" asm void _ZN14Renderer2DBaseD1Ev(void) {
    ldr r1, [pc, #0x8]
    mov r2, #0x0
    str r2, [r1, #0x0]
    bx lr
    dcd gRenderer2D
}
extern "C" asm void _ZN14Renderer2DBaseD0Ev(void) {
    stmdb sp!, {r4, lr}
    ldr r1, [pc, #0x14]
    mov r2, #0x0
    mov r4, r0
    str r2, [r1, #0x0]
    bl _ZdlPv
    mov r0, r4
    ldmia sp!, {r4, pc}
    dcd gRenderer2D
}
#endif

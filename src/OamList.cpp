// Sprite lists for the OAM (ARM9 0x020365f8..0x02036d70). Names are ours.
//
// Animated cells (AnimPlayer, see AnimPlayer.cpp) are queued with a position and a priority into one of
// 3 lists sorted by priority (with a "middle" pointer to speed up the insertion), then written as OAM
// entries into a buffer. In mode 1 the characters of each frame are copied to OBJ VRAM through the
// transfer queue (VramQueue.cpp).
//
// The variables are file statics in the original (in the asm gap, .bss 0x0214d900): mwcc addresses them
// relative to the start of the file's .bss, reproduced with the view `gOam` on the first one.
#include <nitro.h>

extern "C" {

// Same as in AnimPlayer.cpp.
typedef struct AnimPlayer {
    u8 *block1;
    u8 *head;
    u8 *block2;             // characters: data at block2 + ((u16 *)block2)[2]
    void *seq;
    u8 flags;
    u8 unk11;               // bit 0: visible, bits 1-2: OBJ mode
    u8 palette;
    u8 priority;
    u8 unk14;               // bits 0-4: affine parameter, bits 6-7: affine flags
    u8 unk15[3];
    int mode;
    u16 seqNo;
    u16 frame;
    u16 cell;
    u8 unk22[2];
    fx32 frameLength;
    fx32 time;
    u32 charOffset;         // in OBJ VRAM
    BOOL unk30;             // 0: characters already in VRAM, 1: copy the characters of each frame
    u8 dirty;
    u8 unk35[3];
} AnimPlayer;

typedef struct OamCell {
    u16 attr0;
    u16 attr1;
    u16 attr2;
} OamCell;

typedef struct OamNode {
    u32 unk0;
    struct OamNode *next;
    s16 x;
    s16 y;
    AnimPlayer *anim;
    u16 prio;
} OamNode;

typedef struct OamList {
    OamNode head;
    OamNode *mid;
    u16 nFront;             // nodes between head and mid
    u16 nBack;              // nodes after mid
} OamList;

typedef struct OamLists {
    u16 count;
    OamNode nodes[0x7f];
    OamList lists[3];
} OamLists;

typedef struct OamVars {
    GXOamAttr mainOam[1];   // 0x000
    u8 unk8[0x18];
    GXOamAttr subOam[0x60]; // 0x020
    OamLists s;             // 0x320
    fx32 minX;              // 0xd64
    fx32 maxX;
    fx32 minY;
    fx32 maxY;
    fx32 viewMinX;          // 0xd74
    fx32 viewMaxX;
    fx32 viewMinY;
    fx32 viewMaxY;
    int scrollX;            // 0xd84
    int scrollY;
    GXOamAttr *out;         // 0xd8c
    int remaining;          // 0xd90
    u32 vram;               // 0xd94
    int screen;             // 0xd98
    s16 shift;              // 0xd9c: OBJ character name -> byte offset
    u32 sizeMain;           // 0xda0
    u32 sizeSub;            // 0xda4
} OamVars;

extern GXOamAttr data_0214d900[1];   // data_0214d900
extern GXOamAttr data_0214d920[0x60];   // data_0214d920
extern OamLists data_0214dc20;      // gOam.s
// Section-relative literals of the asm functions (gOam + 0x300, + 0xd00, nodes, lists).
extern u8 data_0214dc00[];
extern u8 data_0214e600[];
extern u8 data_0214dc24[];
extern u8 data_0214e610[];
extern u16 data_020b2870[];   // size of an OBJ in 32-byte units by [shape, color mode][size]

#define gOam (*(OamVars *)data_0214d900)

u8 *func_020364c4(AnimPlayer *self, int i);
void func_02036d88(const void *src, u32 dst, u32 size);
int func_02036ee4(int screen);

typedef struct ObjCharHeader {
    u8 unk0[4];
    u16 dataOffset;
    u16 size;
} ObjCharHeader;

u32 func_020365f8(u8 *file, int screen, u32 offset, AnimPlayer *anim)
{
    ObjCharHeader *head = (ObjCharHeader *)(file + 4);
    u32 size = head->size;
    u32 ofs = head->dataOffset;
    if (screen == 1) {
        GX_LoadOBJ((u8 *)head + ofs, offset, size);
    } else {
        GXS_LoadOBJ((u8 *)head + ofs, offset, size);
    }
    if (anim != NULL) {
        anim->charOffset = offset;
    }
    return size;
}

void func_02036644(void)
{
    for (int i = 0; i < 3; i++) {
        data_0214dc20.lists[i].head.prio = 0;
        data_0214dc20.lists[i].head.next = NULL;
        data_0214dc20.lists[i].mid = &data_0214dc20.lists[i].head;
        data_0214dc20.lists[i].nFront = 0;
        data_0214dc20.lists[i].nBack = 0;
    }
    gOam.s.count = 0;
}

void func_02036698(int minX, int minY, int maxX, int maxY)
{
    gOam.minX = minX << FX32_SHIFT;
    gOam.maxX = maxX << FX32_SHIFT;
    gOam.minY = minY << FX32_SHIFT;
    gOam.maxY = maxY << FX32_SHIFT;
}

void func_020366c4(GXOamAttr *out, int count, int screen)
{
    gOam.out = out;
    gOam.remaining = count;
    gOam.vram = screen == 1 ? HW_OBJ_VRAM : HW_DB_OBJ_VRAM;
    gOam.screen = screen;
    gOam.shift = func_02036ee4(screen);
}

void func_02036708(int scrollX, int scrollY)
{
    func_02036644();
    gOam.scrollX = scrollX;
    gOam.scrollY = scrollY;
    gOam.viewMinX = gOam.minX + (scrollX << FX32_SHIFT);
    gOam.viewMaxX = gOam.maxX + (scrollX << FX32_SHIFT);
    gOam.viewMinY = gOam.minY + (scrollY << FX32_SHIFT);
    gOam.viewMaxY = gOam.maxY + (scrollY << FX32_SHIFT);
}

#ifdef NONMATCHING
void func_0203675c(int listNo, u32 prio, fx32 x, fx32 y, AnimPlayer *anim)
{
    OamList *list = &data_0214dc20.lists[listNo];
    if (gOam.s.count >= 0x7f) {
        return;
    }
    if (!(anim->unk11 & 1)) {
        return;
    }
    if (!(x > gOam.viewMinX && gOam.viewMaxX > x && y > gOam.viewMinY && gOam.viewMaxY > y)) {
        return;
    }
    OamNode *node = &data_0214dc20.nodes[gOam.s.count];
    node->x = x >> FX32_SHIFT;
    node->y = y >> FX32_SHIFT;
    node->anim = anim;
    node->prio = prio;
    OamNode *prev = list->mid;
    if (prio > prev->prio) {
        list->nFront++;
        prev = &list->head;
    } else {
        list->nBack++;
    }
    OamNode *cur;
    for (cur = prev->next; cur != NULL; cur = cur->next) {
        if (cur->prio < prio) {
            break;
        }
        prev = cur;
    }
    prev->next = node;
    node->next = cur;
    gOam.s.count++;
    int diff = list->nFront - list->nBack;
    if (diff <= 5 && -diff <= 5) {
        return;
    }
    int total = list->nFront + list->nBack;
    list->nFront = total >> 1;
    list->nBack = total - list->nFront;
    OamNode *mid = list->head.next;
    for (int i = 0; i < list->nFront; i++) {
        mid = mid->next;
    }
    list->mid = mid;
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm void func_0203675c(int listNo, u32 prio, fx32 x, fx32 y, AnimPlayer *anim) {
    stmdb sp!, {r3, r4, r5, lr}
    ldr ip, [pc, #0x13c]
    ldr r5, [pc, #0x13c]
    ldrh ip, [ip, #0x20]
    mov r4, #0x1c
    mla lr, r0, r4, r5
    cmp ip, #0x7f
    ldmhsia sp!, {r3, r4, r5, pc}
    ldr r0, [sp, #0x10]
    ldrb r4, [r0, #0x11]
    tst r4, #0x1
    ldmeqia sp!, {r3, r4, r5, pc}
    ldr r4, [pc, #0x118]
    ldr r5, [r4, #0xd74]
    cmp r2, r5
    ldrgt r5, [r4, #0xd78]
    cmpgt r5, r2
    ldrgt r5, [r4, #0xd7c]
    cmpgt r3, r5
    ldrgt r4, [r4, #0xd80]
    cmpgt r4, r3
    ldmleia sp!, {r3, r4, r5, pc}
    ldr r5, [pc, #0xf4]
    mov r4, #0x14
    mla r4, ip, r4, r5
    mov r2, r2, asr #0xc
    strh r2, [r4, #0x8]
    mov r2, r3, asr #0xc
    strh r2, [r4, #0xa]
    str r0, [r4, #0xc]
    strh r1, [r4, #0x10]
    ldr r2, [lr, #0x14]
    ldrh r0, [r2, #0x10]
    cmp r1, r0
    ldrlsh r0, [lr, #0x1a]
    addls r0, r0, #0x1
    strlsh r0, [lr, #0x1a]
    bls _02036804
    ldrh r0, [lr, #0x18]
    mov r2, lr
    add r0, r0, #0x1
    strh r0, [lr, #0x18]
_02036804:
    ldr r3, [r2, #0x4]
    cmp r3, #0x0
    beq _0203682c
_02036810:
    ldrh r0, [r3, #0x10]
    cmp r0, r1
    blo _0203682c
    mov r2, r3
    ldr r3, [r3, #0x4]
    cmp r3, #0x0
    bne _02036810
_0203682c:
    str r4, [r2, #0x4]
    ldr r0, [pc, #0x6c]
    str r3, [r4, #0x4]
    ldrh r1, [r0, #0x20]
    add r1, r1, #0x1
    strh r1, [r0, #0x20]
    ldrh r1, [lr, #0x1a]
    ldrh r2, [lr, #0x18]
    sub r0, r2, r1
    cmp r0, #0x5
    rsble r0, r0, #0x0
    cmple r0, #0x5
    ldmleia sp!, {r3, r4, r5, pc}
    add r1, r2, r1
    mov r0, r1, asr #0x1
    strh r0, [lr, #0x18]
    ldrh r0, [lr, #0x18]
    mov r2, #0x0
    sub r0, r1, r0
    strh r0, [lr, #0x1a]
    ldrh r0, [lr, #0x18]
    ldr r1, [lr, #0x4]
    cmp r0, #0x0
    ble _0203689c
_0203688c:
    add r2, r2, #0x1
    cmp r2, r0
    ldr r1, [r1, #0x4]
    blt _0203688c
_0203689c:
    str r1, [lr, #0x14]
    ldmia sp!, {r3, r4, r5, pc}
    dcd data_0214dc00
    dcd data_0214e610
    dcd data_0214d900
    dcd data_0214dc24
}
#endif

GXOamAttr *func_020369c8(GXOamAttr *dst, int x, int y, AnimPlayer *anim);

void func_020368b4(void)
{
    OamNode *node;
    int i;
    for (i = 0; i < 3; i++) {
        for (node = data_0214dc20.lists[i].head.next; node != NULL && gOam.remaining > 0; node = node->next) {
            gOam.out = func_020369c8(gOam.out, node->x, node->y, node->anim);
        }
    }
}

void func_02036918(u32 screens)
{
    if (screens & 1) {
        DC_FlushRange(data_0214d900, sizeof(data_0214d900));
        GX_LoadOAM(data_0214d900, 0, sizeof(data_0214d900));
    }
    if (screens & 2) {
        DC_FlushRange(data_0214d920, sizeof(data_0214d920));
        GXS_LoadOAM(data_0214d920, 0, sizeof(data_0214d920));
    }
}

void func_02036974(u32 dmaNo)
{
    DC_InvalidateRange(data_0214d900, sizeof(data_0214d900));
    MI_DmaFill32(dmaNo, data_0214d900, 0xc0, sizeof(data_0214d900));
    DC_InvalidateRange(data_0214d920, sizeof(data_0214d920));
    MI_DmaFill32(dmaNo, data_0214d920, 0xc0, sizeof(data_0214d920));
}

#ifdef NONMATCHING
GXOamAttr *func_020369c8(GXOamAttr *dst, int x, int y, AnimPlayer *anim)
{
    int px = x - gOam.scrollX;
    int py = y - gOam.scrollY;
    u16 *cell = (u16 *)func_020364c4(anim, anim->cell);
    OamCell *src = (OamCell *)(cell + 1);
    int n = cell[0];
    if (gOam.remaining < n) {
        return dst;
    }
    u32 attr0 = ((anim->unk14 & 0xc0) << 2) + ((anim->unk11 & 6) << 9);
    u32 attr2 = (anim->palette << 12) + (anim->priority << 10) + (anim->charOffset >> gOam.shift);
    u32 name = anim->charOffset >> gOam.shift;
    u32 affine = anim->unk14 << 27;
    int i;
    switch (anim->unk30) {
    case 0:
        for (i = 0; i < n; i++) {
            u32 a0 = src->attr0;
            dst->attr0 = attr0 + ((a0 & 0xff00) + ((py + a0) & 0xff));
            u32 a1 = src->attr1;
            dst->attr1 = (a1 & 0xfe00) + ((px + a1) & 0x1ff) + (affine >> 18);
            dst->attr2 = src->attr2 + attr2;
            src++;
            dst++;
        }
        break;
    case 1:
        int shift = gOam.shift - 5;
        u32 round = (1 << shift) - 1;
        u8 *chars = anim->block2 + ((u16 *)anim->block2)[2];
        for (i = 0; i < n; i++) {
            u32 a0 = src->attr0;
            dst->attr0 = attr0 + ((a0 & 0xff00) + ((py + a0) & 0xff));
            u32 a1 = src->attr1;
            dst->attr1 = (a1 & 0xfe00) + ((px + a1) & 0x1ff) + (affine >> 18);
            u16 size = data_020b2870[(a0 >> 13) * 4 + (a1 >> 14)];
            u32 a2 = src->attr2;
            u32 charNo = a2 & 0x3ff;
            dst->attr2 = (a2 & ~0x3ff) + name + (attr2 & ~0x3ff);
            dst++;
            if (anim->dirty) {
                func_02036d88(chars + (charNo << 5), gOam.vram + (name << gOam.shift), size << 5);
            }
            src++;
            name += (size + round) >> shift;
        }
        anim->dirty = 0;
        break;
    }
    gOam.remaining -= n;
    return dst;
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm GXOamAttr *func_020369c8(GXOamAttr *dst, int x, int y, AnimPlayer *anim) {
    stmdb sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    sub sp, sp, #0x24
    ldr r4, [pc, #0x28c]
    str r3, [sp, #0x0]
    ldr r5, [r4, #0xd84]
    ldr r3, [r4, #0xd88]
    sub r9, r1, r5
    ldr r1, [sp, #0x0]
    mov r10, r0
    ldrh r1, [r1, #0x20]
    ldr r0, [sp, #0x0]
    sub r11, r2, r3
    bl func_020364c4
    mov r1, r4
    ldrh r5, [r0, #0x0]
    ldr r1, [r1, #0xd90]
    add r4, r0, #0x2
    cmp r1, r5
    addlt sp, sp, #0x24
    movlt r0, r10
    ldmltia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, pc}
    ldr r0, [sp, #0x0]
    ldrb r1, [r0, #0x11]
    ldrb r6, [r0, #0x13]
    ldrb r3, [r0, #0x14]
    and r2, r1, #0x6
    ldr r1, [sp, #0x0]
    ldr r0, [r0, #0x30]
    ldrb r7, [r1, #0x12]
    mov r6, r6, lsl #0xa
    ldr r1, [pc, #0x220]
    add r7, r6, r7, lsl #0xc
    ldr r6, [sp, #0x0]
    ldrsh r1, [r1, #0x9c]
    ldr r6, [r6, #0x2c]
    mov r8, r2, lsl #0x9
    add r2, r7, r6, lsr r1
    mov r7, r6, lsr r1
    and r6, r3, #0xc0
    mov r3, r3, lsl #0x1b
    cmp r0, #0x0
    add r8, r8, r6, lsl #0x2
    str r3, [sp, #0x18]
    beq _02036a84
    cmp r0, #0x1
    beq _02036af4
    b _02036c48
_02036a84:
    cmp r5, #0x0
    mov r0, #0x0
    ble _02036c48
    ldr r3, [pc, #0x1d4]
_02036a94:
    ldrh r6, [r4, #0x0]
    add r0, r0, #0x1
    cmp r0, r5
    add r1, r11, r6
    and r6, r6, #0xff00
    and r1, r1, #0xff
    add r1, r6, r1
    add r1, r8, r1
    strh r1, [r10, #0x0]
    ldrh r6, [r4, #0x2]
    add r1, r9, r6
    and r6, r6, #0xfe00
    and r1, r1, r3
    add r6, r6, r1
    ldr r1, [sp, #0x18]
    add r1, r6, r1, lsr #0x12
    strh r1, [r10, #0x2]
    ldrh r1, [r4, #0x4]
    add r4, r4, #0x6
    add r1, r1, r2
    strh r1, [r10, #0x4]
    add r10, r10, #0x8
    blt _02036a94
    b _02036c48
_02036af4:
    ldr r0, [sp, #0x0]
    sub r1, r1, #0x5
    ldr r0, [r0, #0x8]
    mov r3, #0x1
    str r1, [sp, #0x14]
    mov r1, r3, lsl r1
    ldrh r3, [r0, #0x4]
    sub r1, r1, #0x1
    str r1, [sp, #0x8]
    mov r1, #0x0
    add r0, r0, r3
    cmp r5, #0x0
    str r1, [sp, #0x4]
    str r0, [sp, #0x10]
    ble _02036c3c
    mov r0, r1
    sub r0, r0, #0x400
    and r0, r2, r0
    str r0, [sp, #0xc]
    mov r0, #0x400
    str r0, [sp, #0x1c]
    ldr r0, [pc, #0x11c]
    add r0, r0, #0x200
    str r0, [sp, #0x20]
    ldr r0, [sp, #0x1c]
    rsb r0, r0, #0x0
    str r0, [sp, #0x1c]
_02036b60:
    ldrh r2, [r4, #0x0]
    add r0, r11, r2
    and r1, r2, #0xff00
    and r0, r0, #0xff
    add r0, r1, r0
    add r0, r8, r0
    strh r0, [r10, #0x0]
    ldrh r0, [r4, #0x2]
    mov r1, r2, lsr #0xd
    ldr r2, [pc, #0xe0]
    add r3, r9, r0
    and r6, r0, #0xfe00
    and r2, r3, r2
    add r3, r6, r2
    ldr r2, [sp, #0x18]
    mov r0, r0, lsr #0xe
    add r0, r0, r1, lsl #0x2
    add r2, r3, r2, lsr #0x12
    mov r1, r0, lsl #0x1
    ldr r0, [pc, #0xbc]
    strh r2, [r10, #0x2]
    ldrh r6, [r0, r1]
    ldrh r2, [r4, #0x4]
    ldr r1, [sp, #0x1c]
    ldr r0, [sp, #0x20]
    and r1, r2, r1
    and r0, r2, r0
    add r2, r1, r7
    ldr r1, [sp, #0xc]
    add r1, r2, r1
    strh r1, [r10, #0x4]
    ldr r1, [sp, #0x0]
    add r10, r10, #0x8
    ldrb r1, [r1, #0x34]
    cmp r1, #0x0
    beq _02036c14
    ldr r1, [sp, #0x10]
    mov r2, r6, lsl #0x5
    add r0, r1, r0, lsl #0x5
    ldr r1, [pc, #0x60]
    ldr r3, [r1, #0xd94]
    ldr r1, [pc, #0x5c]
    ldrsh r1, [r1, #0x9c]
    add r1, r3, r7, lsl r1
    bl func_02036d88
_02036c14:
    ldr r0, [sp, #0x8]
    add r4, r4, #0x6
    add r1, r6, r0
    ldr r0, [sp, #0x14]
    add r7, r7, r1, lsr r0
    ldr r0, [sp, #0x4]
    add r0, r0, #0x1
    str r0, [sp, #0x4]
    cmp r0, r5
    blt _02036b60
_02036c3c:
    ldr r0, [sp, #0x0]
    mov r1, #0x0
    strb r1, [r0, #0x34]
_02036c48:
    ldr r1, [pc, #0x14]
    mov r0, r10
    ldr r2, [r1, #0xd90]
    sub r2, r2, r5
    str r2, [r1, #0xd90]
    add sp, sp, #0x24
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, pc}
    dcd data_0214d900
    dcd data_0214e600
    dcd 0x1ff
    dcd data_020b2870
}
#endif

static inline u32 GetCharAlign(GXOBJVRamModeChar mode)
{
    u32 mask = 0x1f;
    switch (mode) {
    case GX_OBJVRAMMODE_CHAR_1D_64K:
        mask = 0x3f;
        break;
    case GX_OBJVRAMMODE_CHAR_1D_128K:
        mask = 0x7f;
        break;
    case GX_OBJVRAMMODE_CHAR_1D_256K:
        mask = 0xff;
        break;
    }
    return mask;
}

void func_02036c74(u32 screens, u32 size)
{
    if (screens & 1) {
        u32 mask = GetCharAlign((GXOBJVRamModeChar)(reg_GX_DISPCNT & (REG_GX_DISPCNT_EXOBJ_CH_MASK |
                                                                      REG_GX_DISPCNT_OBJMAP_CH_MASK)));
        size = (size + mask) & ~mask;
        gOam.sizeMain = size;
    }
    if (screens & 2) {
        u32 mask = GetCharAlign((GXOBJVRamModeChar)(reg_GXS_DB_DISPCNT & (REG_GX_DISPCNT_EXOBJ_CH_MASK |
                                                                          REG_GX_DISPCNT_OBJMAP_CH_MASK)));
        size = (size + mask) & ~mask;
        gOam.sizeSub = size;
    }
}

u32 func_02036d48(u32 screens)
{
    u32 size = 0;
    if (screens & 1) {
        size = gOam.sizeMain;
    }
    if (screens & 2) {
        size = gOam.sizeSub;
    }
    return size;
}

}

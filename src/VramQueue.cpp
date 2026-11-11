// Queues of VRAM loads done later (ARM9 0x02036d70..0x02036f80). Names are ours.
//
// The variables are file statics in the original (in the asm gap, .bss 0x0214e6ac): the init function
// addresses them through the view `gVramQueue` (relative to the start of the .bss), the others use one
// symbol per variable.
#include <nitro.h>

extern "C" {

typedef struct VramCopy {
    const void *src;
    u32 dst;
    u32 size;
    int unk;
} VramCopy;

// Character data for a sub screen BG, freed (delete[]) once loaded.
typedef struct BgCharLoad {
    void *src;
    u16 offset;
    u32 size;
    int bg;
} BgCharLoad;

typedef struct VramQueueVars {
    VramCopy copies[1];
    BgCharLoad loads[0x100];
    int count;
    int unk;
} VramQueueVars;

extern VramCopy data_0214e6ac[1];
extern BgCharLoad data_0214e6bc[0x100];
extern int data_0214f6bc;           // count

#define gVramQueue (*(VramQueueVars *)data_0214e6ac)

void GXS_LoadBG0Char(const void *pSrc, u32 offset, u32 szByte);
void GXS_LoadBG1Char(const void *pSrc, u32 offset, u32 szByte);
void GXS_LoadBG2Char(const void *pSrc, u32 offset, u32 szByte);
void GXS_LoadBG3Char(const void *pSrc, u32 offset, u32 szByte);

void func_02036d98(const void *src, u32 dst, u32 size, int unk);
void func_02036de4(void *src, u16 offset, u32 size, int bg);

void func_02036d70(int unk)
{
    gVramQueue.unk = unk;
    gVramQueue.count = 0;
}

void func_02036d88(const void *src, u32 dst, u32 size)
{
    func_02036d98(src, dst, size, 0);
}

#ifdef NONMATCHING
void func_02036d98(const void *src, u32 dst, u32 size, int unk)
{
    if (data_0214f6bc >= 1) {
        return;
    }
    VramCopy *copy = &data_0214e6ac[data_0214f6bc];
    copy->src = src;
    copy->dst = dst;
    copy->size = size;
    copy->unk = unk;
    data_0214f6bc++;
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm void func_02036d98(const void *src, u32 dst, u32 size, int unk) {
    stmdb sp!, {r4, lr}
    ldr r4, [pc, #0x28]
    ldr lr, [r4, #0x0]
    cmp lr, #0x1
    ldmgeia sp!, {r4, pc}
    ldr ip, [pc, #0x1c]
    str r0, [ip, lr, lsl #0x4]
    add r0, ip, lr, lsl #0x4
    stmib r0, {r1, r2, r3}
    ldr r0, [r4, #0x0]
    add r0, r0, #0x1
    str r0, [r4, #0x0]
    ldmia sp!, {r4, pc}
    dcd data_0214f6bc
    dcd data_0214e6ac
}
#endif

void func_02036dd4(void *src, u16 offset, u32 size)
{
    func_02036de4(src, offset, size, 0);
}

#ifdef NONMATCHING
void func_02036de4(void *src, u16 offset, u32 size, int bg)
{
    if (data_0214f6bc >= 0x100) {
        return;
    }
    BgCharLoad *load = &data_0214e6bc[data_0214f6bc];
    load->src = src;
    load->offset = offset;
    load->size = size;
    load->bg = bg;
    data_0214f6bc++;
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm void func_02036de4(void *src, u16 offset, u32 size, int bg) {
    stmdb sp!, {r4, lr}
    ldr r4, [pc, #0x30]
    ldr lr, [r4, #0x0]
    cmp lr, #0x100
    ldmgeia sp!, {r4, pc}
    ldr ip, [pc, #0x24]
    str r0, [ip, lr, lsl #0x4]
    add r0, ip, lr, lsl #0x4
    strh r1, [r0, #0x4]
    str r2, [r0, #0x8]
    str r3, [r0, #0xc]
    ldr r0, [r4, #0x0]
    add r0, r0, #0x1
    str r0, [r4, #0x0]
    ldmia sp!, {r4, pc}
    dcd data_0214f6bc
    dcd data_0214e6bc
}
#endif

#ifdef NONMATCHING
void func_02036e28(void)
{
    BgCharLoad *load = data_0214e6bc;
    if (data_0214f6bc <= 0) {
        return;
    }
    do {
        switch (load->bg) {
        case 0:
            GXS_LoadBG0Char(load->src, load->offset, load->size);
            break;
        case 1:
            GXS_LoadBG1Char(load->src, load->offset, load->size);
            break;
        case 2:
            GXS_LoadBG2Char(load->src, load->offset, load->size);
            break;
        case 3:
            GXS_LoadBG3Char(load->src, load->offset, load->size);
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            break;
        }
        delete[] (u8 *)(load++)->src;
    } while (--data_0214f6bc > 0);
}
#else
// Linker names used by the asm below.
extern "C" {
void _ZdaPv(void);
}
// NONMATCHING: kept as assembly until the C version matches.
asm void func_02036e28(void) {
    stmdb sp!, {r3, r4, r5, lr}
    ldr r5, [pc, #0xa8]
    ldr r4, [pc, #0xa8]
    ldr r0, [r5, #0x0]
    cmp r0, #0x0
    ldmleia sp!, {r3, r4, r5, pc}
_02036e40:
    ldr r0, [r4, #0xc]
    cmp r0, #0x7
    addls pc, pc, r0, lsl #0x2
    b _02036ebc
_02036e50:
    b _02036e70
    b _02036e84
    b _02036e98
    b _02036eac
    b _02036ebc
    b _02036ebc
    b _02036ebc
    b _02036ebc
_02036e70:
    ldrh r1, [r4, #0x4]
    ldr r0, [r4, #0x0]
    ldr r2, [r4, #0x8]
    bl GXS_LoadBG0Char
    b _02036ebc
_02036e84:
    ldrh r1, [r4, #0x4]
    ldr r0, [r4, #0x0]
    ldr r2, [r4, #0x8]
    bl GXS_LoadBG1Char
    b _02036ebc
_02036e98:
    ldrh r1, [r4, #0x4]
    ldr r0, [r4, #0x0]
    ldr r2, [r4, #0x8]
    bl GXS_LoadBG2Char
    b _02036ebc
_02036eac:
    ldrh r1, [r4, #0x4]
    ldr r0, [r4, #0x0]
    ldr r2, [r4, #0x8]
    bl GXS_LoadBG3Char
_02036ebc:
    ldr r0, [r4], #0x10
    bl _ZdaPv
    ldr r0, [r5, #0x0]
    sub r0, r0, #0x1
    str r0, [r5, #0x0]
    cmp r0, #0x0
    bgt _02036e40
    ldmia sp!, {r3, r4, r5, pc}
    dcd data_0214f6bc
    dcd data_0214e6bc
}
#endif

// OBJ character name -> byte offset shift (5 = 32 bytes) for the OBJ VRAM mode of a screen.
int func_02036ee4(int screen)
{
    GXOBJVRamModeChar mode;
    if (screen == 1) {
        mode = (GXOBJVRamModeChar)(reg_GX_DISPCNT & (REG_GX_DISPCNT_EXOBJ_CH_MASK | REG_GX_DISPCNT_OBJMAP_CH_MASK));
    } else {
        mode = (GXOBJVRamModeChar)(reg_GXS_DB_DISPCNT &
                                   (REG_GX_DISPCNT_EXOBJ_CH_MASK | REG_GX_DISPCNT_OBJMAP_CH_MASK));
    }
    switch (mode) {
    default:
    case GX_OBJVRAMMODE_CHAR_2D:
        return 5;
    case GX_OBJVRAMMODE_CHAR_1D_32K:
        return 5;
    case GX_OBJVRAMMODE_CHAR_1D_64K:
        return 6;
    case GX_OBJVRAMMODE_CHAR_1D_128K:
        return 7;
    case GX_OBJVRAMMODE_CHAR_1D_256K:
        return 8;
    }
}

}

// Crop data and the pictures of the field objects (ARM9 0x02039208..0x0203ab28). Names are ours.
//
// rom/crop/cropData.bin holds one entry per crop (marker byte 0x99, entry[3] = number of stages, then
// 6-byte stages); the entry of one crop is selected with func_0203aa60. The pictures of crops (one
// archive per season), trees, rocks... are .xbb archives indexed by kind and stage. Kinds: 0-14 crops,
// 15-19 fruit trees, 0x14-0x19 "others", 0x1a-0x2f breakable objects, 0x30-0x34 tree saplings,
// 0x35-0x49 ..., 0x4a ... (see the predicates func_0203a3c8..func_0203a560).
//
// The variables are file statics in the original (in the asm gap, .bss 0x0214f6f4): one block `gCrop`
// and the stage pointers of the selected crop (`sStages`, its own symbol).
#include <nitro.h>
#include "game_file.h"

extern "C" {

typedef struct CropVars {
    u8 *seasonPics;         // 0x00 crops of the season (NCBR archive)
    s8 *entryEnd;           // 0x04 after the stages of the selected crop
    u32 fileSize;           // 0x08
    s8 *entry;              // 0x0c the selected crop
    u8 unk10[4];
    u8 *seasonPals;         // 0x14 crops of the season (NCLR archive)
    u8 unk18[4];
    int cropId;             // 0x1c selected crop (-1: none)
    u8 *othersPals;         // 0x20
    int season;             // 0x24 0-3, 4 = none
    u8 *treePals;           // 0x28
    u8 *treePics;           // 0x2c
    u8 *breakPals;          // 0x30
    u8 *othersPics;         // 0x34
    u8 *breakPics;          // 0x38
    u8 *file;               // 0x3c cropData.bin
} CropVars;

typedef struct ArchiveView {
    void *vtable;
    void *data;
} ArchiveView;

typedef struct GameDay {
    u32 year;
    s8 season;
    s8 day;
    s8 weekday;
} GameDay;

typedef struct GSprite GSprite;

typedef struct S8x10 {
    s8 v[10];
} S8x10;

typedef struct U8x10 {
    u8 v[10];
} U8x10;

typedef struct S32x10 {
    int v[10];
} S32x10;

typedef struct S32x75 {
    int v[75];
} S32x75;

extern CropVars data_0214f6f4;
extern u8 *data_0214f734[];         // sStages

#define gCrop data_0214f6f4
#define sStages data_0214f734

extern char data_020bcfb8[];        // "rom/crop/cropData.bin"
extern char data_020bcfd0[];        // othersNCBR.xbb
extern char data_020bcff8[];        // othersNCLR.xbb
extern char data_020bd020[];        // treeNCBR.xbb
extern char data_020bd044[];        // treeNCLR.xbb
extern char data_020bd068[];        // breakNCBR.xbb
extern char data_020bd090[];        // breakNCLR.xbb
extern char *data_020bccd0[];       // springNCBR.xbb... per season
extern char *data_020bcce0[];       // springNCLR.xbb... per season
extern char *data_020bcdb0[];       // BUD.NCBR, SNOW_BUD.NCBR, FRUIT_TREE_1.NCBR...
extern char *data_020bcdd0[];       // BUD.NCLR...
extern char data_020bd358[];        // SEED.NCBR
extern char data_020bd37c[];
extern char data_020bd3a0[];        // RICE_SEED.NCBR
extern char data_020bd3c8[];
extern char data_020bd3f0[];        // CROPS_DEAD.NCBR
extern char data_020bd418[];
extern char data_020bd440[];        // RICE_DEAD.NCBR
extern char data_020bd468[];
extern char data_020bd490[];        // GRASS_GROWTH1.NCBR
extern char data_020bd4bc[];
extern const S8x10 data_020b28bc;
extern const S8x10 data_020b28c6;
extern const S32x10 data_020b28d0;
extern const U8x10 data_020b28f8;
extern const S32x75 data_020b2904;

void func_020293c0(GSprite *s, int flags);
void func_020293d0(GSprite *s, int flags);
GSprite *func_02029b88(const char *ncbr, const char *nclr, VecFx32 pos, int unk);
GSprite *func_02029c98(void *ncbr, void *nclr, VecFx32 pos, void *ncbrExtra, void *nclrExtra, int a, int b);
void func_02035524(GameDay *out);
ArchiveView *_ZN7ArchiveC1EPv(ArchiveView *self, void *data);
void *func_02036fb0(ArchiveView *self, int i);
u32 func_02036ff8(ArchiveView *self, int i);
void *func_02037058(ArchiveView *self, int i);

int func_02039a80(int kind);
BOOL func_02039fbc(int kind, int stage);
BOOL func_0203a3c8(int kind);
BOOL func_0203a3e4(int kind);
BOOL func_0203a414(int kind);
BOOL func_0203a460(int kind);
BOOL func_0203a47c(int kind);
BOOL func_0203a484(int kind);
BOOL func_0203a4a0(int kind);
BOOL func_0203a4bc(int kind);
int func_0203a5e8(int kind);
BOOL func_0203aa60(int kind);

}

extern "C" {

void func_02039208(void)
{
    GFileObject file;
    gCrop.file = (u8 *)GFile_LoadEx(file.Get(), data_020bcfb8, &gCrop.fileSize, 0);
    gCrop.cropId = -1;
}

void func_0203926c(void)
{
    delete[] gCrop.file;
}

void func_02039284(int season)
{
    gCrop.season = season;
}

void func_02039294(void)
{
    if (gCrop.season == 4) {
        gCrop.seasonPics = NULL;
        gCrop.seasonPals = NULL;
        return;
    }
    GFileObject pics;
    gCrop.seasonPics = (u8 *)GFile_Load(pics.Get(), data_020bccd0[gCrop.season], 0);
    GFileObject pals;
    gCrop.seasonPals = (u8 *)GFile_Load(pals.Get(), data_020bcce0[gCrop.season], 0);
}

void func_02039360(void)
{
    if (gCrop.seasonPics != NULL) {
        delete[] gCrop.seasonPics;
    }
    if (gCrop.seasonPals != NULL) {
        delete[] gCrop.seasonPals;
    }
}

void func_02039394(void)
{
    GFileObject pics;
    gCrop.othersPics = (u8 *)GFile_Load(pics.Get(), data_020bcfd0, 0);
    GFileObject pals;
    gCrop.othersPals = (u8 *)GFile_Load(pals.Get(), data_020bcff8, 0);
}

void func_02039424(void)
{
    delete[] gCrop.othersPics;
    delete[] gCrop.othersPals;
}

void func_02039448(void)
{
    GFileObject pics;
    gCrop.treePics = (u8 *)GFile_Load(pics.Get(), data_020bd020, 0);
    GFileObject pals;
    gCrop.treePals = (u8 *)GFile_Load(pals.Get(), data_020bd044, 0);
}

void func_020394d8(void)
{
    delete[] gCrop.treePics;
    delete[] gCrop.treePals;
}

void func_020394fc(void)
{
    GFileObject pics;
    gCrop.breakPics = (u8 *)GFile_Load(pics.Get(), data_020bd068, 0);
    GFileObject pals;
    gCrop.breakPals = (u8 *)GFile_Load(pals.Get(), data_020bd090, 0);
}

void func_0203958c(void)
{
    if (gCrop.breakPics != NULL) {
        delete[] gCrop.breakPics;
    }
    if (gCrop.breakPals != NULL) {
        delete[] gCrop.breakPals;
    }
    gCrop.breakPics = NULL;
    gCrop.breakPals = NULL;
}

// Fruit tree picture for the stages 1-4 (snow version in winter).
GSprite *func_020395d0(int stage, VecFx32 *pos)
{
    u8 i = (stage - 1) * 2;
    if (gCrop.season == 3) {
        i = i + 1;
    }
    GSprite *s = func_02029b88(data_020bcdb0[i], data_020bcdd0[i], *pos, 0);
    func_020293d0(s, 0x100);
    return s;
}

GSprite *func_02039648(VecFx32 *pos)
{
    GSprite *s = func_02029b88(data_020bd358, data_020bd37c, *pos, 0);
    func_020293c0(s, 0x200);
    func_020293d0(s, 0x100);
    return s;
}

GSprite *func_020396a0(VecFx32 *pos)
{
    GSprite *s = func_02029b88(data_020bd3a0, data_020bd3c8, *pos, 0);
    func_020293c0(s, 0x200);
    func_020293d0(s, 0x100);
    return s;
}

// Picture of an object of `kind` at `stage` (second picture of the crop if `second`).
#ifdef NONMATCHING
GSprite *func_020396f8(int kind, s8 stage, VecFx32 *pos, int second)
{
    ArchiveView picArc;
    ArchiveView palArc;
    GameDay today;
    u8 *pics;
    u8 *pals;
    int first;
    s8 base;
    s8 i;

    func_02035524(&today);
    if (func_0203a4bc(kind)) {
        if (kind >= 0x1c && gCrop.season == 3) {
            kind += 10;
        }
        first = 0x1a;
        pics = gCrop.breakPics;
        pals = gCrop.breakPals;
    } else if (func_0203a484(kind)) {
        first = 0x14;
        pics = gCrop.othersPics;
        pals = gCrop.othersPals;
    } else if (func_0203a460(kind)) {
        if (stage < 5) {
            return func_020395d0(stage, pos);
        }
        if (gCrop.season == 3) {
            return NULL;
        }
        pics = gCrop.treePics;
        pals = gCrop.treePals;
        first = 0xf;
    } else if (func_0203a4a0(kind)) {
        kind = func_0203a5e8(kind);
        pics = gCrop.treePics;
        pals = gCrop.treePals;
        first = 0xf;
    } else {
        pics = gCrop.seasonPics;
        pals = gCrop.seasonPals;
        first = (s8)(gCrop.season * 5);
    }
    _ZN7ArchiveC1EPv(&picArc, pics);
    _ZN7ArchiveC1EPv(&palArc, pals);
    i = 0;
    if (func_0203a484(kind)) {
        for (; first < kind; first++) {
            i += func_02039a80(first) * 2;
        }
        i += stage * 2 - 2;
        if (second) {
            i += 1;
        }
        base = i;
    } else if (!func_0203a4bc(kind)) {
        for (; first < kind; first++) {
            i += func_02039a80(first);
            if (func_0203a460(first)) {
                i += 1;
            }
        }
        base = i;
        i += stage - 1;
    } else {
        base = kind - first;
        i = base;
    }
    s8 pal = i;
    if (func_02036ff8(&palArc, i) == 0) {
        pal = base;
    }
    void *ncbr = func_02036fb0(&picArc, i);
    void *nclr = func_02036fb0(&palArc, pal);
    void *ncbrExtra = func_02037058(&picArc, i);
    void *nclrExtra = func_02037058(&palArc, pal);
    return func_02029c98(ncbr, nclr, *pos, ncbrExtra, nclrExtra, 0, 0);
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm GSprite *func_020396f8(int kind, s8 stage, VecFx32 *pos, int second) {
    stmdb sp!, {r4, r5, r6, r7, r8, r9, lr}
    sub sp, sp, #0x2c
    mov r9, r0
    add r0, sp, #0x14
    mov r8, r1
    mov r7, r2
    mov r6, r3
    bl func_02035524
    mov r0, r9
    bl func_0203a4bc
    cmp r0, #0x0
    beq _02039754
    cmp r9, #0x1c
    blt _02039740
    ldr r0, [pc, #0x260]
    ldr r0, [r0, #0x24]
    cmp r0, #0x3
    addeq r9, r9, #0xa
_02039740:
    ldr r0, [pc, #0x250]
    mov r4, #0x1a
    ldr r1, [r0, #0x38]
    ldr r5, [r0, #0x30]
    b _02039818
_02039754:
    mov r0, r9
    bl func_0203a484
    cmp r0, #0x0
    beq _02039778
    ldr r0, [pc, #0x22c]
    mov r4, #0x14
    ldr r1, [r0, #0x34]
    ldr r5, [r0, #0x20]
    b _02039818
_02039778:
    mov r0, r9
    bl func_0203a460
    cmp r0, #0x0
    beq _020397cc
    cmp r8, #0x5
    bge _020397a4
    mov r0, r8
    mov r1, r7
    bl func_020395d0
    add sp, sp, #0x2c
    ldmia sp!, {r4, r5, r6, r7, r8, r9, pc}
_020397a4:
    ldr r0, [pc, #0x1ec]
    ldr r1, [r0, #0x24]
    cmp r1, #0x3
    addeq sp, sp, #0x2c
    moveq r0, #0x0
    ldmeqia sp!, {r4, r5, r6, r7, r8, r9, pc}
    ldr r1, [r0, #0x2c]
    ldr r5, [r0, #0x28]
    mov r4, #0xf
    b _02039818
_020397cc:
    mov r0, r9
    bl func_0203a4a0
    cmp r0, #0x0
    beq _020397fc
    mov r0, r9
    bl func_0203a5e8
    ldr r2, [pc, #0x1ac]
    mov r9, r0
    ldr r1, [r2, #0x2c]
    ldr r5, [r2, #0x28]
    mov r4, #0xf
    b _02039818
_020397fc:
    ldr r2, [pc, #0x194]
    ldr r0, [r2, #0x24]
    ldr r1, [r2, #0x0]
    add r0, r0, r0, lsl #0x2
    mov r0, r0, lsl #0x18
    ldr r5, [r2, #0x14]
    mov r4, r0, asr #0x18
_02039818:
    add r0, sp, #0x24
    bl _ZN7ArchiveC1EPv
    add r0, sp, #0x1c
    mov r1, r5
    bl _ZN7ArchiveC1EPv
    mov r0, r9
    mov r5, #0x0
    bl func_0203a484
    cmp r0, #0x0
    beq _02039894
    cmp r4, r9
    bge _02039868
_02039848:
    mov r0, r4
    bl func_02039a80
    add r0, r5, r0, lsl #0x1
    mov r0, r0, lsl #0x18
    add r4, r4, #0x1
    cmp r4, r9
    mov r5, r0, asr #0x18
    blt _02039848
_02039868:
    mov r0, r8, lsl #0x1
    sub r0, r0, #0x2
    add r0, r5, r0
    mov r0, r0, lsl #0x18
    mov r5, r0, asr #0x18
    cmp r6, #0x0
    addne r0, r5, #0x1
    movne r0, r0, lsl #0x18
    movne r5, r0, asr #0x18
    mov r4, r5
    b _0203990c
_02039894:
    mov r0, r9
    bl func_0203a4bc
    cmp r0, #0x0
    bne _020398fc
    cmp r4, r9
    bge _020398e4
_020398ac:
    mov r0, r4
    bl func_02039a80
    add r0, r5, r0
    mov r1, r0, lsl #0x18
    mov r0, r4
    mov r5, r1, asr #0x18
    bl func_0203a460
    cmp r0, #0x0
    addne r0, r5, #0x1
    movne r0, r0, lsl #0x18
    add r4, r4, #0x1
    movne r5, r0, asr #0x18
    cmp r4, r9
    blt _020398ac
_020398e4:
    sub r0, r8, #0x1
    add r0, r5, r0
    mov r0, r0, lsl #0x18
    mov r4, r5
    mov r5, r0, asr #0x18
    b _0203990c
_020398fc:
    sub r0, r9, r4
    mov r0, r0, lsl #0x18
    mov r4, r0, asr #0x18
    mov r5, r4
_0203990c:
    add r0, sp, #0x1c
    mov r1, r5
    bl func_02036ff8
    cmp r0, #0x0
    mov r9, r5
    moveq r9, r4
    add r0, sp, #0x24
    mov r1, r5
    bl func_02036fb0
    mov r8, r0
    add r0, sp, #0x1c
    mov r1, r9
    bl func_02036fb0
    mov r6, r0
    add r0, sp, #0x24
    mov r1, r5
    bl func_02037058
    mov r4, r0
    mov r1, r9
    add r0, sp, #0x1c
    bl func_02037058
    str r4, [sp, #0x4]
    str r0, [sp, #0x8]
    mov r0, #0x0
    str r0, [sp, #0xc]
    str r0, [sp, #0x10]
    sub r3, sp, #0x8
    ldmia r7, {r0, r1, r2}
    stmia r3, {r0, r1, r2}
    mov r0, r8
    mov r1, r6
    ldmia r3, {r2, r3}
    bl func_02029c98
    add sp, sp, #0x2c
    ldmia sp!, {r4, r5, r6, r7, r8, r9, pc}
    dcd data_0214f6f4
}
#endif

GSprite *func_0203999c(VecFx32 *pos)
{
    GSprite *s = func_02029b88(data_020bd3f0, data_020bd418, *pos, 0);
    func_020293d0(s, 0x100);
    return s;
}

GSprite *func_020399e8(VecFx32 *pos)
{
    GSprite *s = func_02029b88(data_020bd440, data_020bd468, *pos, 0);
    func_020293d0(s, 0x100);
    return s;
}

GSprite *func_02039a34(VecFx32 *pos)
{
    GSprite *s = func_02029b88(data_020bd490, data_020bd4bc, *pos, 0);
    func_020293d0(s, 0x100);
    return s;
}

// Last stage of a crop.
int func_02039a80(int kind)
{
    if (!func_0203aa60(kind)) {
        return -1;
    }
    if (func_0203a47c(kind)) {
        return (s8)(gCrop.entry[3] - 2);
    }
    return (s8)(gCrop.entry[3] - 1);
}

u8 func_02039ae4(int kind, int stage)
{
    if (!func_0203aa60(kind)) {
        return 0;
    }
    if (stage > func_02039a80(kind)) {
        return 0;
    }
    return sStages[stage][5];
}

BOOL func_02039b24(int kind, int stage)
{
    if (!func_0203aa60(kind)) {
        return FALSE;
    }
    if (stage > func_02039a80(kind)) {
        return FALSE;
    }
    return sStages[stage][5] == 2;
}

// Can `kind` be planted in the current season?
BOOL func_02039b74(int kind)
{
    if (func_0203a47c(kind)) {
        return TRUE;
    }
    if (func_0203a484(kind) || func_0203a460(kind)) {
        if (gCrop.season != 3) {
            switch (kind) {
            case 0x17:
            case 0x18:
                return (u32)(gCrop.season - 1) <= 1;
            }
            return TRUE;
        }
        return FALSE;
    }
    switch (gCrop.season) {
    case 0:
        return func_0203a3c8(kind);
    case 1:
        return func_0203a3e4(kind);
    case 2:
        return func_0203a414(kind);
    }
    return FALSE;
}

BOOL func_02039c40(int kind, int stage, int a, int b, int c)
{
    if (!func_0203aa60(kind)) {
        return FALSE;
    }
    if (gCrop.season == 4) {
        return FALSE;
    }
    if (!func_0203a47c(kind) && gCrop.season == 3) {
        return FALSE;
    }
    if (func_02039fbc(kind, stage)) {
        return FALSE;
    }
    u8 *s = sStages[stage];
    u8 s0 = s[0];
    u8 s1 = s[1];
    u8 s2 = s[2];
    switch (s0) {
    case 0xff:
        return gCrop.season == 0;
    case 0xfe:
        return gCrop.season == 1;
    case 0xfd:
        return gCrop.season == 2;
    }
    if (a < s0 || b < s1 || c < s2) {
        return FALSE;
    }
    return TRUE;
}

fx32 func_02039d48(int kind, int stage, int a, int b)
{
    if (!func_0203aa60(kind)) {
        return 0;
    }
    u8 *s = sStages[stage];
    u32 s1 = s[1];
    u32 s2 = s[2];
    u32 s3 = s[3];
    u32 s4 = s[4];
    f32 fa;
    f32 fb;
    if (s3 == 0xff && s4 == 0xff) {
        fa = 0;
        fb = 0;
    } else {
        fa = ((f32)a - s1) / (int)(s3 - s1);
        fb = ((f32)b - s2) / (int)(s4 - s2);
    }
    f32 x;
    f32 y;
    if (kind == 0x17) {
        x = (100.0f - 50.0f * fa) / 4.0f;
        y = (100.0f - 50.0f * fb) / 4.0f;
    } else {
        x = (100.0f - 50.0f * fa) / func_02039a80(kind);
        y = (100.0f - 50.0f * fb) / func_02039a80(kind);
    }
    fx32 ix;
    fx32 iy;
    if (FX_Modf(FX_F32_TO_FX32(x), &ix) > 0) {
        ix += FX32_ONE;
    }
    if (FX_Modf(FX_F32_TO_FX32(y), &iy) > 0) {
        iy += FX32_ONE;
    }
    return (ix + iy) >> FX32_SHIFT;
}

BOOL func_02039fbc(int kind, int stage)
{
    if (!func_0203aa60(kind)) {
        return FALSE;
    }
    s8 last = func_02039a80(kind);
    return stage == last;
}

BOOL func_02039ff4(int kind)
{
    if (!func_0203aa60(kind)) {
        return FALSE;
    }
    if (func_0203a460(kind)) {
        return FALSE;
    }
    return gCrop.entryEnd[0] != -1;
}

int func_0203a044(int kind)
{
    if (!func_0203aa60(kind)) {
        return -1;
    }
    return gCrop.entryEnd[0];
}

BOOL func_0203a06c(int kind, int stage, int a, int b, int c)
{
    if (!func_0203aa60(kind)) {
        return FALSE;
    }
    u8 *s = sStages[stage];
    u8 s0 = s[0];
    u8 s3 = s[3];
    u8 s4 = s[4];
    if (!func_0203a460(kind) && !func_0203a47c(kind) && func_02039fbc(kind, stage) && a >= s0) {
        return TRUE;
    }
    return b >= s3 || c >= s4;
}

// Moves the sprite of a field object for its kind and stage.
void func_0203a100(int kind, int stage, s16 *x, s16 *y, int screen)
{
    switch (kind) {
    case 5:
        if (screen == 1) {
            *y -= 8;
        }
        break;
    case 6:
        if (screen == 1) {
            *y -= 8;
            *x -= 8;
        }
        break;
    case 9:
        if (screen == 1) {
            break;
        }
    case 10:
        if (screen == 1) {
            *y -= 8;
        }
        break;
    case 14:
        if (screen == 1) {
            *y -= 8;
        }
        break;
    case 20:
        if (screen == 0) {
            *y += 2;
        }
        break;
    case 15:
        if (screen == 1) {
            *y -= 12;
        }
        break;
    case 16:
        if (screen == 1) {
            *y -= 12;
        }
        break;
    case 17:
        if (screen == 1) {
            *y -= 12;
        }
        break;
    case 18:
        if (screen == 1) {
            *y -= 12;
        }
        break;
    case 19:
        if (screen == 1) {
            *y -= 12;
        }
        break;
    case 21:
        if (screen == 1) {
            if (stage == 4) {
                *x += 16;
            }
            if (stage == 5) {
                *x += 16;
            }
        }
        break;
    case 22:
        if (screen == 1) {
            if (stage == 4) {
                *y -= 24;
                *x -= 8;
            }
            if (stage == 5) {
                *y -= 24;
                *x -= 8;
            }
        }
        break;
    case 23:
        if (screen == 1) {
            if (stage == 3) {
                *y -= 16;
            }
            if (stage == 4) {
                *y -= 8;
            }
            if (stage == 5) {
                *y -= 8;
            }
        }
        break;
    case 24:
        if (screen == 1) {
            if (stage == 3) {
                *y -= 32;
            }
            if (stage == 4) {
                *y -= 32;
            }
            if (stage == 5) {
                *y -= 32;
            }
        }
        break;
    }
}

int func_0203a370(int kind, int stage)
{
    if (!func_0203aa60(kind)) {
        return -1;
    }
    return sStages[stage][2];
}

int func_0203a39c(int kind, int stage)
{
    if (!func_0203aa60(kind)) {
        return -1;
    }
    return sStages[stage][0];
}

BOOL func_0203a3c8(int kind)
{
    if (kind >= 0 && kind <= 4) {
        return TRUE;
    }
    return FALSE;
}

BOOL func_0203a3e4(int kind)
{
    if ((kind >= 5 && kind <= 9) || (kind >= 0x30 && kind <= 0x32)) {
        return TRUE;
    }
    return FALSE;
}

BOOL func_0203a414(int kind)
{
    if ((kind >= 10 && kind <= 14) || (kind >= 0x33 && kind <= 0x34)) {
        return TRUE;
    }
    return FALSE;
}

BOOL func_0203a444(int kind)
{
    if (kind >= 0 && kind <= 14) {
        return TRUE;
    }
    return FALSE;
}

BOOL func_0203a460(int kind)
{
    if (kind >= 15 && kind <= 19) {
        return TRUE;
    }
    return FALSE;
}

BOOL func_0203a47c(int kind)
{
    return FALSE;
}

BOOL func_0203a484(int kind)
{
    if (kind >= 0x14 && kind <= 0x19) {
        return TRUE;
    }
    return FALSE;
}

BOOL func_0203a4a0(int kind)
{
    if (kind >= 0x30 && kind <= 0x34) {
        return TRUE;
    }
    return FALSE;
}

BOOL func_0203a4bc(int kind)
{
    if (kind >= 0x1a && kind <= 0x2f) {
        return TRUE;
    }
    return FALSE;
}

BOOL func_0203a4d8(int kind)
{
    if (kind >= 0x1a && kind <= 0x20) {
        return TRUE;
    }
    return FALSE;
}

static inline BOOL IsKind22(int kind)
{
    if (kind >= 0x22 && kind <= 0x24) {
        return TRUE;
    }
    return FALSE;
}

static inline BOOL IsKind2c(int kind)
{
    if (kind >= 0x2c && kind <= 0x2e) {
        return TRUE;
    }
    return FALSE;
}

BOOL func_0203a4f4(int kind)
{
    return (IsKind22(kind) | IsKind2c(kind)) != 0;
}

BOOL func_0203a534(int kind)
{
    if (kind >= 0x35 && kind <= 0x49) {
        return TRUE;
    }
    return FALSE;
}

BOOL func_0203a550(int kind)
{
    return kind == 0x4a;
}

BOOL func_0203a560(int kind)
{
    if (kind >= 0x1e && kind <= 0x20) {
        return TRUE;
    }
    return FALSE;
}

// Fruit tree kind -> sapling kind.
int func_0203a57c(int kind)
{
    if (!func_0203a460(kind)) {
        return -1;
    }
    switch (kind) {
    case 15:
        return 0x30;
    case 16:
        return 0x31;
    case 17:
        return 0x32;
    case 18:
        return 0x33;
    case 19:
        return 0x34;
    }
    return -1;
}

// Sapling kind -> fruit tree kind.
int func_0203a5e8(int kind)
{
    if (!func_0203a4a0(kind)) {
        return -1;
    }
    switch (kind) {
    case 0x30:
        return 15;
    case 0x31:
        return 16;
    case 0x32:
        return 17;
    case 0x33:
        return 18;
    case 0x34:
        return 19;
    }
    return -1;
}

BOOL func_0203a654(int kind, int a, int b, int c)
{
    if (!func_0203aa60(kind)) {
        return FALSE;
    }
    u8 *s = sStages[(s8)func_02039a80(kind)];
    u8 s0 = s[0];
    u8 s1 = s[1];
    u8 s2 = s[2];
    if (a < s0 || b < s1 || c < s2) {
        return FALSE;
    }
    switch (kind) {
    case 15:
        return gCrop.season == 1;
    case 16:
        return gCrop.season == 1;
    case 17:
        return gCrop.season == 1;
    case 18:
        return gCrop.season == 2;
    case 19:
        return gCrop.season == 2;
    }
    return TRUE;
}

int func_0203a75c(int kind)
{
    if (!func_0203a460(kind)) {
        return -1;
    }
    switch (kind) {
    case 15:
        return 1;
    case 16:
        return 1;
    case 17:
        return 2;
    case 18:
        return 1;
    case 19:
        return 2;
    }
    return -1;
}

int func_0203a7c8(int kind)
{
    S8x10 t = data_020b28bc;
    if (func_0203a460(kind)) {
        return 12;
    }
    return t.v[kind - 0x1c];
}

int func_0203a828(int kind)
{
    S8x10 t = data_020b28c6;
    if (func_0203a460(kind)) {
        return 3;
    }
    return t.v[kind - 0x1c];
}

void func_0203a888(int kind, u16 *a, u16 *b)
{
    S32x10 ta = data_020b28d0;
    U8x10 tb = data_020b28f8;
    if (func_0203a460(kind)) {
        *a = ta.v[1];
        *b = tb.v[1];
        return;
    }
    *a = ta.v[kind - 0x1c];
    *b = tb.v[kind - 0x1c];
}

BOOL func_0203a93c(int kind)
{
    BOOL ret = FALSE;
    u32 n = kind - 0x1c;
    if (n <= 0x2d && (0x3fffFE0001D9ULL & (1ULL << n))) {
        ret = TRUE;
    }
    return ret;
}

BOOL func_0203a98c(int kind)
{
    BOOL ret = FALSE;
    u32 n = kind;
    if (n <= 0x1b && (0xc007fff & (1 << n))) {
        ret = TRUE;
    }
    return ret;
}

BOOL func_0203a9b4(int kind)
{
    BOOL ret = FALSE;
    u32 n = kind - 0xf;
    if (n <= 0x16 && (0x44c01f & (1 << n))) {
        ret = TRUE;
    }
    return ret;
}

int func_0203a9dc(int kind, int which)
{
    S32x75 t = data_020b2904;
    if (func_0203a460(kind)) {
        switch (which) {
        case 1:
            return 0x35;
        case 2:
            return 0x38;
        }
    }
    return t.v[kind];
}

// Selects the entry of crop `kind` in cropData.bin.
BOOL func_0203aa60(int kind)
{
    if (kind > 0x19) {
        return FALSE;
    }
    if (gCrop.cropId == kind) {
        return TRUE;
    }
    u32 n = 0;
    for (u32 i = 0; i < gCrop.fileSize; i++) {
        if (gCrop.file[i] == 0x99) {
            if (n != kind) {
                n++;
                continue;
            }
            s8 *p;
            int j;
            s8 *e = (s8 *)(gCrop.file + i);
            gCrop.entry = e;
            p = e + 4;
            for (j = 0; j < e[3]; j++) {
                sStages[j] = (u8 *)p;
                p += 6;
            }
            gCrop.entryEnd = p;
            break;
        }
    }
    gCrop.cropId = kind;
    return TRUE;
}

}
